#ifndef BMSBEL_TEXT_READER_H_
#define BMSBEL_TEXT_READER_H_

#include <stdio.h>

#include <string>
#include "iconv\iconv.h"

class BmsTextFileReader {
public:
  BmsTextFileReader(const std::wstring& filename, const char *encoding);

  ~BmsTextFileReader();

  bool ReadLine( std::wstring& buffer, bool chomp );

private:
  const std::wstring filename_;
  FILE* file_;
  iconv_t cd_;

  char buf_char[BmsConst::BMS_MAX_LINE_BUFFER];
  wchar_t buf_wchar[BmsConst::BMS_MAX_LINE_BUFFER];
  size_t len_char, len_wchar;
  const char *buf_iconv_in;
  char *buf_iconv_out;
};

#endif // BMSBEL_TEXT_READER_H_
