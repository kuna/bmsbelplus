#include <errno.h>

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_exception.h"

#include "bmsbel\bms_text_file_reader.h"

//
// global buffer for class
//
char utf8BOM[3] = { 0xEF, 0xBB, 0xBF };

BmsTextFileReader::BmsTextFileReader(const std::wstring& filename, const char *encoding) :
filename_(filename),
file_(_wfopen(filename.c_str(), L"r")),
cd_(iconv_open(BmsBelOption::DEFAULT_UNICODE_ENCODING, encoding)),
buf_iconv_in((const char*)buf_char),
buf_iconv_out((char*)buf_wchar)
{
	if (file_ == NULL) {
		throw BmsFileOpenException(filename, errno);
	}
	if ((int)cd_ == -1) {
		throw BmsFileNotSupportedEncoding(filename, errno);
	}
}

BmsTextFileReader::~BmsTextFileReader()
{
	fclose(file_);
	iconv_close(cd_);
}

int BmsTextFileReader::IsFileUTF8(const std::wstring& filename) {
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
		iconv(cd, 0, 0, &but_out_iconv, &len_wchar);
		iconv(cd, &buf_iconv, &len_char, &but_out_iconv, &len_wchar);
		if (errno) {
			return errno;	// failed to convert UTF8->wchar. maybe, SHIFT_JIS?
		}
	}
	iconv_close(cd);
	return 0;
}

//
// TODO: things to be changed into wstring
// - class BmsHeader
// - ReadLine argument
// - class BmsRegist
// - BmsUtil -> float, int converter
//
bool
BmsTextFileReader::ReadLine(std::wstring& buffer, bool chomp)
{
	buffer.clear();

	int i;
	for (i = 0; i < BmsConst::BMS_MAX_LINE_BUFFER-1; i++) {
		int c;
		c = fgetc(file_);
		if (c == EOF) {
			if (ferror(file_)) {
				throw BmsFileAccessException(filename_, errno);
			}
			return NOT(i == 0);
		}
		if (c == '\n') {
			if (!chomp) {
				buf_char[i++] = '\n';
			}
			break;
		}
		buf_char[i] = static_cast<char>(c);
	}
	buf_char[i] = 0;

	// convert encoding
	len_char = i;
	len_wchar = BmsConst::BMS_MAX_LINE_BUFFER*2;
	buf_iconv_in = (const char*)buf_char;
	buf_iconv_out = (char*)buf_wchar;
	int l = iconv(cd_, &buf_iconv_in, &len_char, &buf_iconv_out, &len_wchar);
	if (l < 0) {
		// decoding error might be occured
	}
	*buf_iconv_out = *(buf_iconv_out + 1) = 0;	// end character: L'\0'
	buf_wchar[(BmsConst::BMS_MAX_LINE_BUFFER*2 - len_wchar)/2] = 0;
	buffer = buf_wchar;

	return true;
}
