#ifndef BMSBEL_TEXT_READER_H_
#define BMSBEL_TEXT_READER_H_

#include <stdio.h>

#include <string>

class BmsTextFileReader {
public:
  explicit BmsTextFileReader( const std::string& filename );

  ~BmsTextFileReader();

  bool ReadLine( std::string& buffer, bool chomp );
  int IsFileUTF8();

private:
  const std::string filename_;
  FILE* file_;
};

#endif // BMSBEL_TEXT_READER_H_
