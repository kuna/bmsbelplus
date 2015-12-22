#include "ttl_define.h"
#include "tt_ini_file.h"

#include "bmx2wav_converter_parameter.h"

#pragma warn -8018

using namespace Bmx2Wav;


// -- Normalize ----------------------------------------------------------
template <>
TtEnumTable<Parameter::Normalize>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::Normalize::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( PEAK );
  REGIST( AVERAGE );
  REGIST( OVER );
  REGIST( NONE );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::Normalize>
TtEnumBase<Parameter::Normalize>::TABLE;

// -- Random -------------------------------------------------------------
template <>
TtEnumTable<Parameter::Random>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::Random::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( EVERY_SELECT );
  REGIST( CONSTANT );
  REGIST( REAL_RANDOM );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::Random>
TtEnumBase<Parameter::Random>::TABLE;

// -- WavFileNotFoundReaction --------------------------------------------
template <>
TtEnumTable<Parameter::WavFileNotFoundReaction>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::WavFileNotFoundReaction::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( ERROR );
  REGIST( ASK_ONLY_ERROR );
  REGIST( MUST_ASK );
  REGIST( IGNORE );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::WavFileNotFoundReaction>
TtEnumBase<Parameter::WavFileNotFoundReaction>::TABLE;

// -- WavFormatErrorReaction ---------------------------------------------
template <>
TtEnumTable<Parameter::WavFormatErrorReaction>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::WavFormatErrorReaction::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( ERROR );
  REGIST( ASK );
  REGIST( IGNORE );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::WavFormatErrorReaction>
TtEnumBase<Parameter::WavFormatErrorReaction>::TABLE;

// -- WavFileErrorReaction -----------------------------------------------
template <>
TtEnumTable<Parameter::WavFileErrorReaction>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::WavFileErrorReaction::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( ERROR );
  REGIST( ASK );
  REGIST( IGNORE );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::WavFileErrorReaction>
TtEnumBase<Parameter::WavFileErrorReaction>::TABLE;

// -- ParseDuplicateHeaderReaction ---------------------------------------
template <>
TtEnumTable<Parameter::ParseDuplicateHeaderReaction>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::ParseDuplicateHeaderReaction::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( OVERWRITE );
  REGIST( REMAIN );
  REGIST( ERROR );
  REGIST( ASK );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::ParseDuplicateHeaderReaction>
TtEnumBase<Parameter::ParseDuplicateHeaderReaction>::TABLE;

// -- ParseDuplicateBarChangeReaction ------------------------------------
template <>
TtEnumTable<Parameter::ParseDuplicateBarChangeReaction>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::ParseDuplicateBarChangeReaction::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( OVERWRITE );
  REGIST( REMAIN );
  REGIST( ERROR );
  REGIST( ASK );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::ParseDuplicateBarChangeReaction>
TtEnumBase<Parameter::ParseDuplicateBarChangeReaction>::TABLE;

// -- ParseErrorReaction -------------------------------------------------
template <>
TtEnumTable<Parameter::ParseErrorReaction>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::ParseErrorReaction::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( ERROR );
  REGIST( ASK );
  REGIST( IGNORE );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::ParseErrorReaction>
TtEnumBase<Parameter::ParseErrorReaction>::TABLE;

// -- ScrollObjectDefineErrorReaction ------------------------------------
template <>
TtEnumTable<Parameter::ScrollObjectDefineErrorReaction>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::ScrollObjectDefineErrorReaction::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( ERROR );
  REGIST( ASK );
  REGIST( IGNORE );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::ScrollObjectDefineErrorReaction>
TtEnumBase<Parameter::ScrollObjectDefineErrorReaction>::TABLE;

// -- ScrollObjectEntryIsEmptyReaction -----------------------------------
template <>
TtEnumTable<Parameter::ScrollObjectEntryIsEmptyReaction>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Regist( Parameter::ScrollObjectEntryIsEmptyReaction::NAME, #NAME )
  REGIST( DEFAULT );
  REGIST( ERROR );
  REGIST( ASK );
  REGIST( IGNORE );
#undef  REGIST
};

template <>
TtEnumTable<Parameter::ScrollObjectEntryIsEmptyReaction>
TtEnumBase<Parameter::ScrollObjectEntryIsEmptyReaction>::TABLE;


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( ConverterParameters );

  DEFINE_PARAMETER_NAME_STRING( CancelAtResounding );
  DEFINE_PARAMETER_NAME_STRING( ErrorInNotEntriedWavWasUsed );
  DEFINE_PARAMETER_NAME_STRING( NormalizeKind );
  DEFINE_PARAMETER_NAME_STRING( Volume );
  DEFINE_PARAMETER_NAME_STRING( RandomKind );
  DEFINE_PARAMETER_NAME_STRING( RandomConstantValue );
  DEFINE_PARAMETER_NAME_STRING( IgnoreBgaChannel );
  DEFINE_PARAMETER_NAME_STRING( WavFileNotFoundReaction );
  DEFINE_PARAMETER_NAME_STRING( WavFormatErrorReaction );
  DEFINE_PARAMETER_NAME_STRING( WavFileErrorReaction );
  DEFINE_PARAMETER_NAME_STRING( CheckWavFileExists );
  DEFINE_PARAMETER_NAME_STRING( CheckBmpFileExists );
  DEFINE_PARAMETER_NAME_STRING( CheckWavUsed );
  DEFINE_PARAMETER_NAME_STRING( CheckBmpUsed );
  DEFINE_PARAMETER_NAME_STRING( CheckBpmUsed );
  DEFINE_PARAMETER_NAME_STRING( CheckStopUsed );
  DEFINE_PARAMETER_NAME_STRING( DifferentLongNoteEnclosedObjectIsError );
  DEFINE_PARAMETER_NAME_STRING( NotSoundObjectSpecifiedByLNOBJ );
  DEFINE_PARAMETER_NAME_STRING( NotUseLowPassFilter );
  DEFINE_PARAMETER_NAME_STRING( ShowCompressor );
  DEFINE_PARAMETER_NAME_STRING( UseCompressor );
  DEFINE_PARAMETER_NAME_STRING( CompressThreshold );
  DEFINE_PARAMETER_NAME_STRING( CompressRatio );
  DEFINE_PARAMETER_NAME_STRING( CompressAttackTime );
  DEFINE_PARAMETER_NAME_STRING( CompressReleaseTime );
  DEFINE_PARAMETER_NAME_STRING( CompressLookAhead );
  DEFINE_PARAMETER_NAME_STRING( ConvertStartBarNumber );
  DEFINE_PARAMETER_NAME_STRING( ConvertEndBarNumber );
  DEFINE_PARAMETER_NAME_STRING( TrimingStartBarNumber );
  DEFINE_PARAMETER_NAME_STRING( TrimingEndBarNumber );
  DEFINE_PARAMETER_NAME_STRING( ParseDuplicateHeaderReaction );
  DEFINE_PARAMETER_NAME_STRING( ParseDuplicateBarChangeReaction );
  DEFINE_PARAMETER_NAME_STRING( ParseErrorReaction );
  DEFINE_PARAMETER_NAME_STRING( DoTriming );
  DEFINE_PARAMETER_NAME_STRING( CheckStageFileExists );
  DEFINE_PARAMETER_NAME_STRING( ScrollObjectDefineErrorReaction );
  DEFINE_PARAMETER_NAME_STRING( ScrollObjectEntryIsEmptyReaction );
  DEFINE_PARAMETER_NAME_STRING( UseOutputTemplate );
  DEFINE_PARAMETER_NAME_STRING( OutputTemplateSource );
  DEFINE_PARAMETER_NAME_STRING( RemoveCanNotUseCharAsFilePath );
  DEFINE_PARAMETER_NAME_STRING( NormalizeOverPPM );
  DEFINE_PARAMETER_NAME_STRING( ExecuteAfterProcess );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessExecuteFile );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessExecuteArguments );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessWaitForProcessExit );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessDeleteWavFile );
  DEFINE_PARAMETER_NAME_STRING( ChangePlayFile );
  DEFINE_PARAMETER_NAME_STRING( PlayFilePath );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessStartOnBackground );
}

// -- ParameterSet -------------------------------------------------------
ParameterSet::ParameterSet( void ) :
input_filename_( "" ),
output_filename_( "" ),

// BMS
ignore_bga_channel_( true ),
different_long_note_enclosed_object_is_error_( false ),
parse_duplicate_header_reaction_( Parameter::ParseDuplicateHeaderReaction::DEFAULT ),
parse_duplicate_bar_change_reaction_( Parameter::ParseDuplicateBarChangeReaction::DEFAULT ),
parse_error_reaction_( Parameter::ParseErrorReaction::DEFAULT ),

// WAV
not_use_low_pass_filter_( false ),
wav_file_not_found_reaction_( Parameter::WavFileNotFoundReaction::DEFAULT ),
wav_format_error_reaction_( Parameter::WavFormatErrorReaction::DEFAULT ),
wav_file_error_reaction_( Parameter::WavFileErrorReaction::DEFAULT ),

// 変換
cancel_at_resounding_( false ),
not_sound_object_specified_by_lnobj_( false ),
error_in_not_entried_wav_was_used_( false ),
convert_start_bar_number_( 0 ),
convert_end_bar_number_( 999 ),
do_triming_( false ),
triming_start_bar_number_( 0 ),
triming_end_bar_number_( 999 ),
scroll_object_define_error_reaction_( Parameter::ScrollObjectDefineErrorReaction::DEFAULT ),
scroll_object_entry_is_empty_reaction_( Parameter::ScrollObjectEntryIsEmptyReaction::DEFAULT ),

// 音量
normalize_kind_( Parameter::Normalize::DEFAULT ),
normalize_over_ppm_( 100 ),
volume_( 100 ),

// RANDOM
random_kind_( Parameter::Random::DEFAULT ),
random_constant_value_( 1 ),

// 検証
check_wav_file_exists_( false ),
check_bmp_file_exists_( false ),
check_stage_file_exists_( false ),
check_wav_used_( false ),
check_bmp_used_( false ),
check_bpm_used_( false ),
check_stop_used_( false ),

// コンプレッサ
show_compressor_( false ),
use_compressor_( false ),
compress_threshold_( 0.125 ),
compress_ratio_( 0.5 ),
compress_attack_time_( 2205 ),
compress_release_time_( 2205 ),
compress_look_ahead_( 441 ),

// 出力テンプレート
use_output_template_( false ),
output_template_source_( "%O" ),
remove_can_not_use_char_as_file_path_( true ),

// 後処理
execute_after_process_( false ),
after_process_execute_file_( "" ),
after_process_execute_arguments_( "" ),
after_process_start_on_background_( false ),
after_process_wait_for_process_exit_( true ),
after_process_delete_wav_file_( false ),
change_play_file_( false ),
play_file_path_( "%O" )
{
}


void
ParameterSet::ReadFromFile( void )
{
  TtIniFile ini_file;
  this->ReadFromIniFile( ini_file );
}

void
ParameterSet::ReadFromFile( const std::string& filename )
{
  TtIniFile ini_file( filename );
  this->ReadFromIniFile( ini_file );
}

void
ParameterSet::ReadFromIniFile( TtIniFile& ini_file )
{
  using namespace Parameter;
  TtIniSection section = ini_file[Tag::ConverterParameters];

  // BMS
  ignore_bga_channel_ = section.GetBoolean( Tag::IgnoreBgaChannel, true );
  different_long_note_enclosed_object_is_error_ = section.GetBoolean( Tag::DifferentLongNoteEnclosedObjectIsError, false );
  parse_duplicate_header_reaction_     = TtEnum<ParseDuplicateHeaderReaction>::Parse( section.GetString( Tag::ParseDuplicateHeaderReaction ) );
  parse_duplicate_bar_change_reaction_ = TtEnum<ParseDuplicateBarChangeReaction>::Parse( section.GetString( Tag::ParseDuplicateBarChangeReaction ) );
  parse_error_reaction_                = TtEnum<ParseErrorReaction>::Parse( section.GetString( Tag::ParseErrorReaction ) );

  // WAV
  not_use_low_pass_filter_     = section.GetBoolean( Tag::NotUseLowPassFilter,     false );
  wav_file_not_found_reaction_ = TtEnum<WavFileNotFoundReaction>::Parse( section.GetString( Tag::WavFileNotFoundReaction ) );
  wav_format_error_reaction_   = TtEnum<WavFormatErrorReaction>::Parse( section.GetString( Tag::WavFormatErrorReaction ) );
  wav_file_error_reaction_     = TtEnum<WavFileErrorReaction>::Parse( section.GetString( Tag::WavFileErrorReaction ) );

  // 変換
  cancel_at_resounding_                  = section.GetBoolean( Tag::CancelAtResounding,               false );
  not_sound_object_specified_by_lnobj_   = section.GetBoolean( Tag::NotSoundObjectSpecifiedByLNOBJ,   false );
  error_in_not_entried_wav_was_used_     = section.GetBoolean( Tag::ErrorInNotEntriedWavWasUsed,      false );
  convert_start_bar_number_              = section.GetInteger( Tag::ConvertStartBarNumber,            0 );
  convert_end_bar_number_                = section.GetInteger( Tag::ConvertEndBarNumber,              999 );
  do_triming_                            = section.GetBoolean( Tag::DoTriming,                        false );
  triming_start_bar_number_              = section.GetInteger( Tag::TrimingStartBarNumber,            0 );
  triming_end_bar_number_                = section.GetInteger( Tag::TrimingEndBarNumber,              999 );
  scroll_object_define_error_reaction_   = TtEnum<ScrollObjectDefineErrorReaction>::Parse( section.GetString( Tag::ScrollObjectDefineErrorReaction ) );
  scroll_object_entry_is_empty_reaction_ = TtEnum<ScrollObjectEntryIsEmptyReaction>::Parse( section.GetString( Tag::ScrollObjectEntryIsEmptyReaction ) );

  // 音量
  normalize_kind_     = TtEnum<Normalize>::Parse( section.GetString( Tag::NormalizeKind ) );
  normalize_over_ppm_ = section.GetInteger( Tag::NormalizeOverPPM, 100 );
  volume_             = section.GetInteger( Tag::Volume,           100 );

  // RANDOM
  random_kind_           = TtEnum<Random>::Parse( section.GetString( Tag::RandomKind ) );
  random_constant_value_ = section.GetInteger( Tag::RandomConstantValue, 1 );

  // 検証
  check_wav_file_exists_   = section.GetBoolean( Tag::CheckWavFileExists,   false );
  check_bmp_file_exists_   = section.GetBoolean( Tag::CheckBmpFileExists,   false );
  check_stage_file_exists_ = section.GetBoolean( Tag::CheckStageFileExists, false );
  check_wav_used_          = section.GetBoolean( Tag::CheckWavUsed,         false );
  check_bmp_used_          = section.GetBoolean( Tag::CheckBmpUsed,         false );
  check_bpm_used_          = section.GetBoolean( Tag::CheckBpmUsed,         false );
  check_stop_used_         = section.GetBoolean( Tag::CheckStopUsed,        false );

  // コンプレッサ
  show_compressor_       = section.GetBoolean( Tag::ShowCompressor,      false );
  use_compressor_        = section.GetBoolean( Tag::UseCompressor,       false );
  compress_threshold_    = section.GetDouble(  Tag::CompressThreshold,   0.125 );
  compress_ratio_        = section.GetDouble(  Tag::CompressRatio,       0.5 );
  compress_attack_time_  = section.GetInteger( Tag::CompressAttackTime,  2205 );
  compress_release_time_ = section.GetInteger( Tag::CompressReleaseTime, 2205 );
  compress_look_ahead_   = section.GetInteger( Tag::CompressLookAhead,   441 );

  // 出力テンプレート
  use_output_template_                  = section.GetBoolean( Tag::UseOutputTemplate,             false );
  output_template_source_               = section.GetString(  Tag::OutputTemplateSource,          "%O" );
  remove_can_not_use_char_as_file_path_ = section.GetBoolean( Tag::RemoveCanNotUseCharAsFilePath, true );

  // 後処理
  execute_after_process_               = section.GetBoolean( Tag::ExecuteAfterProcess,            false );
  after_process_execute_file_          = section.GetString(  Tag::AfterProcessExecuteFile,        "" );
  after_process_execute_arguments_     = section.GetString(  Tag::AfterProcessExecuteArguments,   "" );
  after_process_start_on_background_   = section.GetBoolean( Tag::AfterProcessStartOnBackground,  false );
  after_process_wait_for_process_exit_ = section.GetBoolean( Tag::AfterProcessWaitForProcessExit, true );
  after_process_delete_wav_file_       = section.GetBoolean( Tag::AfterProcessDeleteWavFile,      false );
  change_play_file_                    = section.GetBoolean( Tag::ChangePlayFile,                 false );
  play_file_path_                      = section.GetString(  Tag::PlayFilePath,                   "%O" );

  ini_file.Flush();
}

void
ParameterSet::WriteToFile( void ) const
{
  TtIniFile ini_file;
  this->WriteToIniFile( ini_file );
}

void
ParameterSet::WriteToFile( const std::string& filename ) const
{
  TtIniFile ini_file( filename );
  this->WriteToIniFile( ini_file );
}

void
ParameterSet::WriteToIniFile( TtIniFile& ini_file ) const
{
  using namespace Parameter;
  TtIniSection section = ini_file[Tag::ConverterParameters];

  // BMS
  section.SetBoolean( Tag::IgnoreBgaChannel, ignore_bga_channel_ );
  section.SetBoolean( Tag::DifferentLongNoteEnclosedObjectIsError, different_long_note_enclosed_object_is_error_ );
  section.SetString(  Tag::ParseDuplicateHeaderReaction,    parse_duplicate_header_reaction_.ToString() );
  section.SetString(  Tag::ParseDuplicateBarChangeReaction, parse_duplicate_bar_change_reaction_.ToString() );
  section.SetString(  Tag::ParseErrorReaction,              parse_error_reaction_.ToString() );

  // WAV
  section.SetBoolean( Tag::NotUseLowPassFilter,     not_use_low_pass_filter_ );
  section.SetString(  Tag::WavFileNotFoundReaction, wav_file_not_found_reaction_.ToString() );
  section.SetString(  Tag::WavFormatErrorReaction,  wav_format_error_reaction_.ToString() );
  section.SetString(  Tag::WavFileErrorReaction,    wav_file_error_reaction_.ToString() );

  // 変換
  section.SetBoolean( Tag::CancelAtResounding,               cancel_at_resounding_ );
  section.SetBoolean( Tag::NotSoundObjectSpecifiedByLNOBJ,   not_sound_object_specified_by_lnobj_ );
  section.SetBoolean( Tag::ErrorInNotEntriedWavWasUsed,      error_in_not_entried_wav_was_used_ );
  section.SetInteger( Tag::ConvertStartBarNumber,            convert_start_bar_number_ );
  section.SetInteger( Tag::ConvertEndBarNumber,              convert_end_bar_number_ );
  section.SetBoolean( Tag::DoTriming,                        do_triming_ );
  section.SetInteger( Tag::TrimingStartBarNumber,            triming_start_bar_number_ );
  section.SetInteger( Tag::TrimingEndBarNumber,              triming_end_bar_number_ );
  section.SetString(  Tag::ScrollObjectDefineErrorReaction,  scroll_object_define_error_reaction_.ToString() );
  section.SetString(  Tag::ScrollObjectEntryIsEmptyReaction, scroll_object_entry_is_empty_reaction_.ToString() );

  // 音量
  section.SetString(  Tag::NormalizeKind,    normalize_kind_.ToString() );
  section.SetInteger( Tag::NormalizeOverPPM, normalize_over_ppm_ );
  section.SetInteger( Tag::Volume,           volume_ );

  // RANDOM
  section.SetString(  Tag::RandomKind,          random_kind_.ToString() );
  section.SetInteger( Tag::RandomConstantValue, random_constant_value_ );

  // 検証
  section.SetBoolean( Tag::CheckWavFileExists,   check_wav_file_exists_ );
  section.SetBoolean( Tag::CheckBmpFileExists,   check_bmp_file_exists_ );
  section.SetBoolean( Tag::CheckStageFileExists, check_stage_file_exists_ );
  section.SetBoolean( Tag::CheckWavUsed,         check_wav_used_ );
  section.SetBoolean( Tag::CheckBmpUsed,         check_bmp_used_ );
  section.SetBoolean( Tag::CheckBpmUsed,         check_bpm_used_ );
  section.SetBoolean( Tag::CheckStopUsed,        check_stop_used_ );

  // コンプレッサ
  section.SetBoolean( Tag::ShowCompressor,      show_compressor_ );
  section.SetBoolean( Tag::UseCompressor,       use_compressor_ );
  section.SetDouble(  Tag::CompressThreshold,   compress_threshold_ );
  section.SetDouble(  Tag::CompressRatio,       compress_ratio_ );
  section.SetInteger( Tag::CompressAttackTime,  compress_attack_time_ );
  section.SetInteger( Tag::CompressReleaseTime, compress_release_time_ );
  section.SetInteger( Tag::CompressLookAhead,   compress_look_ahead_ );

  // 出力テンプレート
  section.SetBoolean( Tag::UseOutputTemplate,             use_output_template_ );
  section.SetString(  Tag::OutputTemplateSource,          output_template_source_ );
  section.SetBoolean( Tag::RemoveCanNotUseCharAsFilePath, remove_can_not_use_char_as_file_path_ );

  // 後処理
  section.SetBoolean( Tag::ExecuteAfterProcess,            execute_after_process_ );
  section.SetString(  Tag::AfterProcessExecuteFile,        after_process_execute_file_ );
  section.SetString(  Tag::AfterProcessExecuteArguments,   after_process_execute_arguments_ );
  section.SetBoolean( Tag::AfterProcessStartOnBackground,  after_process_start_on_background_ );
  section.SetBoolean( Tag::AfterProcessWaitForProcessExit, after_process_wait_for_process_exit_ );
  section.SetBoolean( Tag::AfterProcessDeleteWavFile,      after_process_delete_wav_file_ );
  section.SetBoolean( Tag::ChangePlayFile,                 change_play_file_ );
  section.SetString(  Tag::PlayFilePath,                   play_file_path_ );

  ini_file.Flush();
}
