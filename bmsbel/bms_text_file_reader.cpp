#include <errno.h>

#include "bmsbel/bms_define.h"
#include "bmsbel/bms_exception.h"

#include "bmsbel/bms_text_file_reader.h"
#include "bmsbel/bms_util.h"

BmsTextFileReader::BmsTextFileReader(const std::string& filename, const char *encoding) :
filename_(filename),
cd_(iconv_open(BmsBelOption::DEFAULT_UNICODE_ENCODING, encoding)),
buf_iconv_in((ICONVCHR*)buf_char),
buf_iconv_out((char*)buf_char_utf8)
{
	if (!BmsUtil::OpenFile(&file_, filename.c_str(), "r")) {
		throw BmsFileOpenException(filename, errno);
	}
	if ((void*)cd_ == (void*)-1) {
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
BmsTextFileReader::ReadLine(std::string& buffer, bool chomp)
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
	len_char_utf8 = BmsConst::BMS_MAX_LINE_BUFFER;
	buf_iconv_in = (ICONVCHR*)buf_char;
	buf_iconv_out = (char*)buf_char_utf8;
	int l = iconv(cd_, &buf_iconv_in, &len_char, &buf_iconv_out, &len_char_utf8);
	if (l < 0) {
		// decoding error might be occured
	}
	*buf_iconv_out = *(buf_iconv_out + 1) = 0;	// end character: L'\0'
	buf_char_utf8[BmsConst::BMS_MAX_LINE_BUFFER - len_char_utf8] = 0;
	buffer = buf_char_utf8;

	return true;
}
