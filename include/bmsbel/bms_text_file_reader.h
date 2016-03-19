#ifndef BMSBEL_TEXT_READER_H_
#define BMSBEL_TEXT_READER_H_

#include <stdio.h>

#include <string>
#ifdef _WIN32
#include "iconv/iconv.h"
typedef const char ICONVCHR;
#else
#include <iconv.h>
typedef char ICONVCHR;
#endif

class BmsTextFileReader {
public:
  BmsTextFileReader(const std::string& filename, const char *encoding);

  ~BmsTextFileReader();

  bool ReadLine( std::string& buffer, bool chomp );

private:
  const std::string filename_;
  FILE* file_;
  iconv_t cd_;

  char buf_char[BmsConst::BMS_MAX_LINE_BUFFER];
  char buf_char_utf8[BmsConst::BMS_MAX_LINE_BUFFER];
  size_t len_char, len_char_utf8;
  ICONVCHR *buf_iconv_in;
  char *buf_iconv_out;
};

#endif // BMSBEL_TEXT_READER_H_
