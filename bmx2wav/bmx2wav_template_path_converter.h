#ifndef BMX2WAV_TEMPLATE_PATH_CONVERTER_H_
#define BMX2WAV_TEMPLATE_PATH_CONVERTER_H_

#include "bms_bms.h"

namespace Bmx2Wav {
  class TemplatePathConverter {
  public:
    explicit TemplatePathConverter( const std::string& input_filename,
                                    const std::string& output_filename,
                                    BmsBms& bms );

    void SetRemoveCanNotUseCharAsFilePath( bool flag );
    bool GetRemoveCanNotUseCharAsFilePath( void ) const;

    std::string Convert( const std::string& source );

  private:
    void ConvertBmsHeader( std::string& result, const char*& p );
    void ConvertStringFormatTime( std::string& result, const char*& p );

  private:
    std::string input_filename_;
    std::string output_filename_;
    BmsBms&     bms_;
    bool        remove_can_not_use_char_as_file_path_;
  };
}

#endif // BMX2WAV_TEMPLATE_PATH_CONVERTER_H_
