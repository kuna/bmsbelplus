#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#include <algorithm>

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_util.h"
#include "bmsbel\bms_exception.h"
#include "iconv\iconv.h"

namespace {
	const char HEX36_TABLE[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

char
BmsUtil::CharacterToNumber(char c)
{
	switch (c) {
	case '0':           return 0;
	case '1':           return 1;
	case '2':           return 2;
	case '3':           return 3;
	case '4':           return 4;
	case '5':           return 5;
	case '6':           return 6;
	case '7':           return 7;
	case '8':           return 8;
	case '9':           return 9;
	case 'a': case 'A': return 10;
	case 'b': case 'B': return 11;
	case 'c': case 'C': return 12;
	case 'd': case 'D': return 13;
	case 'e': case 'E': return 14;
	case 'f': case 'F': return 15;
	case 'g': case 'G': return 16;
	case 'h': case 'H': return 17;
	case 'i': case 'I': return 18;
	case 'j': case 'J': return 19;
	case 'k': case 'K': return 20;
	case 'l': case 'L': return 21;
	case 'm': case 'M': return 22;
	case 'n': case 'N': return 23;
	case 'o': case 'O': return 24;
	case 'p': case 'P': return 25;
	case 'q': case 'Q': return 26;
	case 'r': case 'R': return 27;
	case 's': case 'S': return 28;
	case 't': case 'T': return 29;
	case 'u': case 'U': return 30;
	case 'v': case 'V': return 31;
	case 'w': case 'W': return 32;
	case 'x': case 'X': return 33;
	case 'y': case 'Y': return 34;
	case 'z': case 'Z': return 35;
	default:            return -1;
	}
}


bool
BmsUtil::IsDigit(char c)
{
	char tmp = BmsUtil::CharacterToNumber(c);
	return tmp >= 0 && tmp <= 9;
}

bool
BmsUtil::IsNotDigit(char c)
{
	return NOT(BmsUtil::IsDigit(c));
}

bool
BmsUtil::IsAlphabet(char c)
{
	char tmp = BmsUtil::CharacterToNumber(c);
	return tmp >= 10 && tmp <= 35;
}

bool
BmsUtil::IsNotAlphabet(char c)
{
	return NOT(BmsUtil::IsAlphabet(c));
}

bool
BmsUtil::IsHex36(char c)
{
	return BmsUtil::CharacterToNumber(c) >= 0;
}

bool
BmsUtil::IsNotHex36(char c)
{
	return NOT(BmsUtil::IsHex36(c));
}



char
BmsUtil::IntegerToHex36(int x)
{
	return HEX36_TABLE[x % BmsConst::HEX36_MAX_COUNT];
}


int
BmsUtil::GCD(int x, int y)
{
	if (x == 0 || y == 0) { return 0; }
	x = x > 0 ? x : -x;
	y = y > 0 ? y : -y;
	while (x != y) {
		if (x > y) { x -= y; }
		else         { y -= x; }
	}
	return x;
}

int
BmsUtil::LCM(int x, int y)
{
	if (x == 0 || y == 0) { return 0; }
	return x * y / BmsUtil::GCD(x, y);
}


unsigned int
BmsUtil::GetDenominator(double x, unsigned int max)
{
	const double epsilon = 0.0000000001;
	if (x < 0.0) {
		x = fabs(x);
	}
	if (x > 1.0) {
		x -= floor(x);
	}
	if (x < epsilon) {
		return 1;
	}

	for (unsigned int i = 1; i < max; ++i) {
		if (fabs(x * i - floor(x * i + 0.5)) < epsilon) {
			return i;
		}
	}
	return max;
}


bool
BmsUtil::StringToInteger(const std::string& str, int* ret, unsigned int base)
{
	char* endptr;
	errno = 0;
	*ret = strtol(str.c_str(), &endptr, base);
	return *endptr == '\0' && errno == 0;
}

bool
BmsUtil::StringToFloat(const std::string& str, double* ret)
{
	char* endptr;
	errno = 0;
	*ret = strtod(str.c_str(), &endptr);
	return *endptr == '\0' && errno == 0;
}

void
BmsUtil::StringToUpper(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
}

bool
BmsUtil::StringToInteger(const std::wstring& str, int* ret, unsigned int base)
{
	wchar_t* endptr;
	errno = 0;
	*ret = wcstol(str.c_str(), &endptr, base);
	return *endptr == L'\0' && errno == 0;
}

bool
BmsUtil::StringToFloat(const std::wstring& str, double* ret)
{
	wchar_t* endptr;
	errno = 0;
	*ret = wcstod(str.c_str(), &endptr);
	return *endptr == L'\0' && errno == 0;
}

void
BmsUtil::StringToUpper(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
}

//
// global buffer for class
//
char utf8BOM[3] = { 0xEF, 0xBB, 0xBF };
int BmsUtil::IsFileUTF8(const std::wstring& filename) {
	FILE *file = _wfopen(filename.c_str(), L"r");
	if (NOT(file)) {
		throw BmsFileOpenException(filename, errno);
	}

	// buffers
	char buf_char[BmsConst::BMS_MAX_LINE_BUFFER];
	wchar_t buf_wchar[BmsConst::BMS_MAX_LINE_BUFFER];
	size_t len_char, len_wchar;

	// check 4 byte to see is it UTF8 BOM
	fseek(file, 0, SEEK_SET);
	if (fread(buf_char, 1, 3, file) == 3 && memcmp(buf_char, utf8BOM, 3) == 0) {
		return true;
	}
	// attempt to convert few lines
	fseek(file, 0, SEEK_SET);
	iconv_t cd = iconv_open(BmsBelOption::DEFAULT_UNICODE_ENCODING, "UTF-8");
	if ((int)cd == -1) {
		// conversion is not supported
		return -1;
	}
	for (int i = 0; i < BmsBelOption::CONVERT_ATTEMPT_LINES && NOT(feof(file)); i++) {
		fgets(buf_char, BmsConst::BMS_MAX_LINE_BUFFER, file);
		len_char = strlen(buf_char);
		const char *buf_iconv = buf_char;
		char *but_out_iconv = (char*)buf_wchar;
		len_wchar = BmsConst::BMS_MAX_LINE_BUFFER;		// available characters for converting
		int iconv_ret = iconv(cd, &buf_iconv, &len_char, &but_out_iconv, &len_wchar);
		*but_out_iconv = *(but_out_iconv + 1) = 0;		// NULL terminal character
		if (errno || iconv_ret < 0) {
			iconv_close(cd);
			return 0;	// failed to convert UTF8->wchar. maybe, SHIFT_JIS?
		}
	}
	iconv_close(cd);
	return 1;
}