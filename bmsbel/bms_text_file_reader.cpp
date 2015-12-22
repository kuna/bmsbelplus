#include <errno.h>

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_exception.h"

#include "bmsbel\bms_text_file_reader.h"

// must define it to use lib file
#define USING_STATIC_LIBICONV	1
#include "iconv\iconv.h"


BmsTextFileReader::BmsTextFileReader(const std::string& filename) :
filename_(filename),
file_(fopen(filename.c_str(), "r"))
{
	if (file_ == NULL) {
		throw BmsFileOpenException(filename, errno);
	}
}

BmsTextFileReader::~BmsTextFileReader()
{
	fclose(file_);
}

int BmsTextFileReader::IsFileUTF8() {
	// check 4 byte to see is it UTF8 BOM
	char utf8BOM[3] = { 0xEF, 0xBB, 0xBF };
	char buf[1024];			size_t len;
	wchar_t buf_out[1024];	size_t len_out;
	fseek(file_, 0, SEEK_SET);
	if (fread(buf, 1, 3, file_) == 3 && memcmp(buf, utf8BOM, 3) == 0) {
		return true;
	}
	// attempt to convert few lines
	fseek(file_, 0, SEEK_SET);
	iconv_t cd = iconv_open(BmsBelOption::DEFAULT_UNICODE_ENCODING, "UTF-8");
	if ((int)cd == -1) {
		// conversion is not supported
		return -1;
	}
	for (int i = 0; i < BmsBelOption::CONVERT_ATTEMPT_LINES && NOT(feof(file_)); i++) {
		fgets(buf, 1024, file_);
		len = strlen(buf);
		const char *buf_iconv = buf;
		char *but_out_iconv = (char*)buf_out;
		len_out = 1024;		// available characters for converting
		iconv(cd, 0, 0, &but_out_iconv, &len_out);
		iconv(cd, &buf_iconv, &len, &but_out_iconv, &len_out);
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
BmsTextFileReader::ReadLine(std::string& buffer, bool chomp)
{
	buffer.clear();
	for (;;) {
		int c;
		c = fgetc(file_);
		if (c == EOF) {
			if (ferror(file_)) {
				throw BmsFileAccessException(filename_, errno);
			}
			return NOT(buffer.empty());
		}
		if (c == '\n') {
			if (!chomp) {
				buffer.append(1, '\n');
			}
			return true;
		}
		buffer.append(1, static_cast<char>(c));
	}
}
