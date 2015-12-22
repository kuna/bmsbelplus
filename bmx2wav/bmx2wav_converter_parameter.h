#ifndef BMX2WAV_CONVERTER_PARAMETER_H_
#define BMX2WAV_CONVERTER_PARAMETER_H_

#include <map>

#include "tt_ini_file.h"
#include "tt_enum.h"

#undef ERROR
#undef IGNORE

namespace Bmx2Wav {
  namespace Parameter {
    // -- Normalize ------------------------------------------------------
    class Normalize {
    public:
      enum Value {
        DEFAULT,
        PEAK,
        AVERAGE,
        OVER,
        NONE,
      };
    };

    // -- Random ---------------------------------------------------------
    class Random {
    public:
      enum Value {
        DEFAULT,
        EVERY_SELECT,
        CONSTANT,
        REAL_RANDOM,
      };
    };

    // -- WavFileNotFoundReaction ----------------------------------------
    class WavFileNotFoundReaction {
    public:
      enum Value {
        DEFAULT,
        ERROR,
        ASK_ONLY_ERROR,
        MUST_ASK,
        IGNORE,
      };
    };

    // -- WavFormatErrorReaction -----------------------------------------
    class WavFormatErrorReaction {
    public:
      enum Value {
        DEFAULT,
        ERROR,
        ASK,
        IGNORE,
      };
    };

    // -- WavFileErrorReaction -------------------------------------------
    class WavFileErrorReaction {
    public:
      enum Value {
        DEFAULT,
        ERROR,
        ASK,
        IGNORE,
      };
    };

    // -- ParseDuplicateHeaderReaction -----------------------------------
    class ParseDuplicateHeaderReaction {
    public:
      enum Value {
        DEFAULT,
        OVERWRITE,
        REMAIN,
        ERROR,
        ASK,
      };
    };

    // -- ParseDuplicateBarChangeReaction --------------------------------
    class ParseDuplicateBarChangeReaction {
    public:
      enum Value {
        DEFAULT,
        OVERWRITE,
        REMAIN,
        ERROR,
        ASK,
      };
    };

    // -- ParseErrorReaction ---------------------------------------------
    class ParseErrorReaction {
    public:
      enum Value {
        DEFAULT,
        ERROR,
        ASK,
        IGNORE,
      };
    };

    // -- ScrollObjectDefineErrorReaction --------------------------------
    class ScrollObjectDefineErrorReaction {
    public:
      enum Value {
        DEFAULT,
        ERROR,
        ASK,
        IGNORE,
      };
    };

    // -- ScrollObjectEntryIsEmptyReaction -------------------------------
    class ScrollObjectEntryIsEmptyReaction {
    public:
      enum Value {
        DEFAULT,
        ERROR,
        ASK,
        IGNORE,
      };
    };
  }

  // -- ParameterSet -----------------------------------------------------
  class ParameterSet {
  public:
    explicit ParameterSet( void );

    void ReadFromFile( void );
    void ReadFromFile( const std::string& filename );
  private:
    void ReadFromIniFile( TtIniFile& ini_file );

  public:
    void WriteToFile( void ) const;
    void WriteToFile( const std::string& filename ) const;
  private:
    void WriteToIniFile( TtIniFile& ini_file ) const;

  public:
    // 入出力
    std::string input_filename_;
    std::string output_filename_;

    // BMS
    bool      ignore_bga_channel_;
    bool      different_long_note_enclosed_object_is_error_;
    TtEnum<Parameter::ParseDuplicateHeaderReaction>    parse_duplicate_header_reaction_;
    TtEnum<Parameter::ParseDuplicateBarChangeReaction> parse_duplicate_bar_change_reaction_;
    TtEnum<Parameter::ParseErrorReaction>              parse_error_reaction_;

    // WAV
    bool      not_use_low_pass_filter_;
    TtEnum<Parameter::WavFileNotFoundReaction> wav_file_not_found_reaction_;
    TtEnum<Parameter::WavFormatErrorReaction>  wav_format_error_reaction_;
    TtEnum<Parameter::WavFileErrorReaction>    wav_file_error_reaction_;

    // 変換
    bool         cancel_at_resounding_;
    bool         not_sound_object_specified_by_lnobj_;
    bool         error_in_not_entried_wav_was_used_;
    unsigned int convert_start_bar_number_;
    unsigned int convert_end_bar_number_;
    bool         do_triming_;
    unsigned int triming_start_bar_number_;
    unsigned int triming_end_bar_number_;
    TtEnum<Parameter::ScrollObjectDefineErrorReaction>  scroll_object_define_error_reaction_;
    TtEnum<Parameter::ScrollObjectEntryIsEmptyReaction> scroll_object_entry_is_empty_reaction_;

    // 音量
    TtEnum<Parameter::Normalize> normalize_kind_;
    unsigned int                 normalize_over_ppm_;
    int                          volume_;

    // RANDOM
    TtEnum<Parameter::Random> random_kind_;
    int                       random_constant_value_;

    // 検証
    bool check_wav_file_exists_;
    bool check_bmp_file_exists_;
    bool check_stage_file_exists_;
    bool check_wav_used_;
    bool check_bmp_used_;
    bool check_bpm_used_;
    bool check_stop_used_;

    // コンプレッサ
    bool         show_compressor_;
    bool         use_compressor_;
    double       compress_threshold_;
    double       compress_ratio_;
    unsigned int compress_attack_time_;
    unsigned int compress_release_time_;
    int          compress_look_ahead_;

    // 出力テンプレート
    bool        use_output_template_;
    std::string output_template_source_;
    bool        remove_can_not_use_char_as_file_path_;

    // 後処理
    bool        execute_after_process_;
    std::string after_process_execute_file_;
    std::string after_process_execute_arguments_;
    bool        after_process_start_on_background_;
    bool        after_process_wait_for_process_exit_;
    bool        after_process_delete_wav_file_;
    bool        change_play_file_;
    std::string play_file_path_;

    // 未分類
  };
}

#endif BMX2WAV_CONVERTER_PARAMETER_H_
