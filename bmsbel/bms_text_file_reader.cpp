#include <errno.h>

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_exception.h"

#include "bmsbel\bms_text_file_reader.h"

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
