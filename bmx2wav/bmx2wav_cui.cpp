#include <stdio.h>

#include <vector>
#include <memory>

#include "ttl_define.h"
#include "tt_command_line_analyzer.h"
#include "tt_path.h"
#include "tt_util.h"

#include "bmx2wav.h"

#include "bmx2wav_cui.h"

using namespace Bmx2Wav;

int
Console::Main( int argc, char* argv[] )
{
  Converter converter;

  bool is_detail;
  {
    TtCommandLineOptionAnalyzer      analyzer;
    TtCommandLineOptionBoolean       version_option( "version" );
    TtCommandLineOptionSingleOperand config_option( "config" );
    TtCommandLineOptionBoolean       detail_option( "detail" );

    version_option.AddOptionString( "v" );
    config_option.AddOptionString( "c" );
    detail_option.AddOptionString( "d" );

    analyzer.Regist( version_option );
    analyzer.Regist( config_option );
    analyzer.Regist( detail_option );

    std::vector<std::string> rest;
    try {
      rest = analyzer.AnalyzeForConsole( argc - 1, argv + 1 );
    }
    catch ( TtCommandLineOptionException& e ) {
      fprintf( stderr, "コマンドラインオプションが不正です。info : %s\n", e.Dump().c_str() );
      Console::OutputUsage();
      return 1;
    }

    if ( version_option.GetResult() ) {
      Console::OutputVersion();
      return 0;
    }

    switch ( rest.size() ) {
    case 1:
      converter.GetParameterSet().input_filename_ = rest[0];
      converter.GetParameterSet().output_filename_ = TtPath::ChangeExtension( rest[0], "wav" );
      break;

    case 2:
      converter.GetParameterSet().input_filename_ = rest[0];
      converter.GetParameterSet().output_filename_ = rest[1];
      break;

    default:
      fprintf( stderr, "コマンドライン引数が不正です。\n" );
      Console::OutputUsage();
      return 1;
    }

    if ( config_option.IsSpecified() ) {
      if ( NOT( TtPath::FileExists( config_option.GetSingleOperand() ) ) ) {
        fprintf( stderr, "指定した設定ファイルは存在しません。\n" );
        fprintf( stderr, "file : %s\n", config_option.GetSingleOperand().c_str() );
        return 1;
      }
      converter.GetParameterSet().ReadFromFile( config_option.GetSingleOperand() );
    }
    is_detail = detail_option.GetResult();
  }

  std::auto_ptr<Outputer> ptr( is_detail ?
                               static_cast<Outputer*>( new DetailOutputer( converter ) ):
                               static_cast<Outputer*>( new NormalOutputer( converter ) ) );
  converter.SetProgressReceiver( ptr.get() );

  try {
    converter.Convert();
    return 0;
  }
  catch ( TtFileOpenException& e ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "ファイルを開くのに失敗しました。\n" );
    fprintf( stderr, "ファイル名 : %s\n", e.GetFileName().c_str() );
    fprintf( stderr, "メッセージ : %s\n", TtUtil::GetWindowsSystemErrorMessage( e.GetErrorNumber() ).c_str() );
  }
  catch ( BmsParseException& e ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "BMS ファイル解析中にエラーが発生しました。\n" );
    fprintf( stderr, "行番号 : %d\n", e.GetLine() );
    fprintf( stderr, "メッセージ : %s\n", e.Message().c_str() );
  }
  catch ( BmsException& e ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "BMS のエラーが発生しました。\n" );
    fprintf( stderr, "メッセージ : %s\n", e.Message().c_str() );
  }
  catch ( ConvertAbortException& e ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "%s\n", e.Message().c_str() );
  }
  catch ( Bmx2WavConvertException& e ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "変換中にエラーが発生しました。\n" );
    fprintf( stderr, "%s\n", e.Message().c_str() );
  }
  catch ( Bmx2WavInternalException& e ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "内部エラー\n" );
    fprintf( stderr, "%s\n", e.Message().c_str() );
  }
  catch ( TtException& e ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "エラーが発生しました。\n" );
    fprintf( stderr, "DUMP : %s\n", e.Dump().c_str() );
  }
  catch ( std::bad_alloc& e ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "メモリ不足が発生しました。\n" );
    fprintf( stderr, "メッセージ : %s\n", e.what() );
  }
  catch ( ... ) {
    fprintf( stderr, "\n" );
    fprintf( stderr, "予期せぬ例外が発生しました。\n" );
  }
  return 1;
}

void
Console::OutputVersion( void )
{
  fprintf( stderr, "BMX2WAV Console version %s (%s)\n", BMX2WAV_VERSION, TTL_DATE_STRING.c_str() );
}

void
Console::OutputUsage( void )
{
  fprintf( stderr, "Usage: bmx2wavc.exe [options] input_bms_filename [output_filename]\n" );
}

// -- Console::CountProgressCalculator -----------------------------------
void Console::CountProgressCalculator::Initialize( unsigned int start, unsigned int end, unsigned int width )
{
  start_   = start;
  end_     = end;
  width_   = width;
  current_ = start;
}

unsigned int
Console::CountProgressCalculator::Add( unsigned int x )
{
  int tmp = this->Calc( current_ + x ) - this->Calc( current_ );
  current_ += x;
  return tmp;
}

unsigned int
Console::CountProgressCalculator::Calc( unsigned int a )
{
  return static_cast<unsigned int>( static_cast<double>( a ) / static_cast<double>( end_ - start_ ) * static_cast<double>( width_ ) );
}


// -- Console::RangeProgressCalculator -----------------------------------
void Console::RangeProgressCalculator::Initialize( unsigned int start, unsigned int end, unsigned int width )
{
  start_   = start;
  end_     = end;
  width_   = width;
  current_ = start;
}

unsigned int
Console::RangeProgressCalculator::DoneAt( unsigned int x )
{
  int tmp = this->Calc( x + 1 ) - this->Calc( current_ );
  current_ = x + 1;
  return tmp;
}

unsigned int
Console::RangeProgressCalculator::Calc( unsigned int a )
{
  return static_cast<unsigned int>( static_cast<double>( a ) / static_cast<double>( end_ - start_ + 1 ) * static_cast<double>( width_ ) );
}

// -- Console::Outputer::StreamOut -------------------------------------
Console::Outputer::StreamOut::StreamOut( FILE* stream ) :
stream_( stream )
{
}


Console::Outputer::StreamOut&
Console::Outputer::StreamOut::operator <<( const std::string& str )
{
  fprintf( stream_, "%s", str.c_str() );

  return *this;
}

Console::Outputer::StreamOut&
Console::Outputer::StreamOut::operator <<( int value )
{
  return this->operator <<( TtUtil::ToStringFrom( value ) );
}

Console::Outputer::StreamOut&
Console::Outputer::StreamOut::operator <<( unsigned int value )
{
  return this->operator <<( TtUtil::ToStringFrom( value ) );
}

Console::Outputer::StreamOut&
Console::Outputer::StreamOut::operator <<( double value )
{
  return this->operator <<( TtUtil::ToStringFrom( value ) );
}

Console::Outputer::StreamOut&
Console::Outputer::StreamOut::operator <<( BmsWord& word )
{
  return this->operator <<( word.ToCharPtr() );
}

void
Console::Outputer::StreamOut::operator <<( Period period )
{
  NOT_USE_VAR( period );

  this->operator <<( "\n" );
}


// -- Console::Outputer --------------------------------------------------
Console::Outputer::Outputer( Converter& converter, FILE* stream ) :
converter_( converter ),
stream_out_( stream )
{
}

// -- Console::NormalOutputer --------------------------------------------
Console::NormalOutputer::NormalOutputer( Converter& converter ) :
Outputer( converter, stderr )
{
}

void
Console::NormalOutputer::AtConvertStart( void )
{
  stream_out_ << "BMS File : " << converter_.GetParameterSet().input_filename_ << StreamOut::PERIOD;
  stream_out_ << "WAV File : " << converter_.GetParameterSet().output_filename_ << StreamOut::PERIOD;

  all_time_ = TtTime();
}

void
Console::NormalOutputer::AtBmsParseStart( const std::string& filename )
{
  NOT_USE_VAR( filename );
  stream_out_ << "BMS ファイルを解析しています。... ";
}

void
Console::NormalOutputer::AtBmsParseEnd( const std::string& filename, BmsBms& bms )
{
  NOT_USE_VAR( filename );

  stream_out_ << "完了" << StreamOut::PERIOD;

  std::string tmp( "TITLE / ARTIST : " );
  if ( bms.GetHeaders().IsExists( "TITLE" ) ) {
    tmp.append( bms.GetHeaders()["TITLE"] );
  }
  else {
    tmp.append( "??" );
  }
  tmp.append( " / " );
  if ( bms.GetHeaders().IsExists( "ARTIST" ) ) {
    tmp.append( bms.GetHeaders()["ARTIST"] );
  }
  else {
    tmp.append( "??" );
  }
  stream_out_ << tmp << StreamOut::PERIOD;
}

void
Console::NormalOutputer::AtWavFileReadStart( unsigned int file_count )
{
  stream_out_ << "WAV ファイルを読み込んでいます。" << StreamOut::PERIOD;
  stream_out_ << "|------|------|------|------|------|------|------|------|------|------|" << StreamOut::PERIOD;
  stream_out_ << "*";

  wav_read_progress_calculator_.Initialize( 0, file_count, 70 );

  partial_time_ = TtTime();
}

void
Console::NormalOutputer::AtOneWavFileReadStart( BmsWord word, const std::string& filename )
{
  NOT_USE_VAR( filename );
}

void
Console::NormalOutputer::AtOneWavFileReadEnd( BmsWord word, const std::string& filename )
{
  NOT_USE_VAR( word );
  NOT_USE_VAR( filename );

  for ( int i = wav_read_progress_calculator_.Add( 1 ); i != 0; --i ) {
    stream_out_ << "*";
  }
}

bool
Console::NormalOutputer::AtWavFileFormatError( BmsWord word, InvalidWavFileFormatException& e )
{
  NOT_USE_VAR( word );
  NOT_USE_VAR( e );
  return true;
}

void
Console::NormalOutputer::AtWavFileReadEnd( unsigned int file_count )
{
  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << StreamOut::PERIOD;
  stream_out_ << "WAV ファイルの読み込み完了。(" << file_count << " file : " << time << " 秒)" << StreamOut::PERIOD;
}

void
Console::NormalOutputer::AtMixStart( unsigned int start_position, unsigned int end_position )
{
  stream_out_ << "WAV ファイルを合成しています。" << StreamOut::PERIOD;
  stream_out_ << "|------|------|------|------|------|------|------|------|------|------|" << StreamOut::PERIOD;
  stream_out_ << "*";

  mix_progress_calculator_.Initialize( start_position, end_position, 70 );

  partial_time_ = TtTime();
}

void
Console::NormalOutputer::AtMixProgress( unsigned int current_position )
{
  for ( int i = mix_progress_calculator_.DoneAt( current_position ); i != 0; --i ) {
    stream_out_ << "*";
  }
}

void
Console::NormalOutputer::AtMixEnd( unsigned int end_position )
{
  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << StreamOut::PERIOD;
  stream_out_ << "WAV ファイルの合成完了(" << end_position << " step : " << time << " 秒)" << StreamOut::PERIOD;
}

void
Console::NormalOutputer::AtNormalizeStart( void )
{
  stream_out_ << "音量のノーマライズ中です。 ... ";
}

void
Console::NormalOutputer::AtNormalizeEnd( double change_ratio )
{
  NOT_USE_VAR( change_ratio );

  stream_out_ << "完了" << StreamOut::PERIOD;
}

void
Console::NormalOutputer::AtVolumeChangeStart( int percentage )
{
  NOT_USE_VAR( percentage );

  stream_out_ << "音量変更中です。 ... ";
}

void
Console::NormalOutputer::AtVolumeChangeEnd( int percentage )
{
  NOT_USE_VAR( percentage );

  stream_out_ << "完了" << StreamOut::PERIOD;

}

void
Console::NormalOutputer::AtWriteToFileStart( const std::string& filename )
{
  NOT_USE_VAR( filename );

  stream_out_ << "ファイルに書き込みをしています。 ... ";
}

void
Console::NormalOutputer::AtWriteToFileEnd( const std::string& filename )
{
  NOT_USE_VAR( filename );

  stream_out_ << "完了" << StreamOut::PERIOD;
}


void
Console::NormalOutputer::AtAfterProcessStart( TtProcess::CreateInfo& info )
{
  NOT_USE_VAR( info );

  stream_out_ << "後処理を実行しています。"  << StreamOut::PERIOD;

  partial_time_ = TtTime();
}


void
Console::NormalOutputer::AtCreatedProcess( TtProcess& process, TtProcess::CreateInfo& info )
{
  NOT_USE_VAR( process );
  NOT_USE_VAR( info );

  stream_out_ << "プログラムが実行されました。" << StreamOut::PERIOD;
  fflush( stdout );
}

void
Console::NormalOutputer::AtWaitForProcessStart( TtProcess& process )
{
  NOT_USE_VAR( process );

  stream_out_ << "プログラムの終了を待っています。 ... " << StreamOut::PERIOD;
  fflush( stdout );
}

void
Console::NormalOutputer::AtWaitForProcessEnd( TtProcess& process )
{
  NOT_USE_VAR( process );

  stream_out_ << StreamOut::PERIOD;
  stream_out_ << "完了" << StreamOut::PERIOD;
}

void
Console::NormalOutputer::AtDeleteFileStart( const std::string& filename )
{
  NOT_USE_VAR( filename );

  stream_out_ << "WAV ファイルを削除しています。 ... ";
}

void
Console::NormalOutputer::AtDeleteFileEnd( const std::string& filename, bool result )
{
  NOT_USE_VAR( filename );
  NOT_USE_VAR( result );

  stream_out_ << (result ? "完了" : "失敗") << StreamOut::PERIOD;
}

void
Console::NormalOutputer::AtAfterProcessEnd( TtProcess& process, TtProcess::CreateInfo& info, const std::string& outputed_filename, BmsBms& bms )
{
  NOT_USE_VAR( process );
  NOT_USE_VAR( info );
  NOT_USE_VAR( outputed_filename );
  NOT_USE_VAR( bms );

  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << "後処理が終了しました。(" << time << " 秒)" << StreamOut::PERIOD;
}

void
Console::NormalOutputer::AtFailStartProcess( TtProcess::CreateInfo& info )
{
  NOT_USE_VAR( info );

  stream_out_ << "プログラムの起動に失敗しました。" << StreamOut::PERIOD;
}

void
Console::NormalOutputer::AtConvertEnd( void )
{
  double time = TtTime::GetNow() - all_time_;

  fprintf( stdout, "変換は終了しました。(%f second)\n", time );
}

// -- Console::DetailOutputer --------------------------------------------
Console::DetailOutputer::DetailOutputer( Converter& converter ) :
Outputer( converter, stderr )
{
}

void
Console::DetailOutputer::AtConvertStart( void )
{
  all_time_ = TtTime();

  stream_out_ << "BMS File : " << converter_.GetParameterSet().input_filename_ << StreamOut::PERIOD;
}

BmsParser::Reactor&
Console::DetailOutputer::GetBmsParserReactor( void )
{
  return *this;
}

void
Console::DetailOutputer::AtRandomValueGenerated( BmsParser::Parser& parser, unsigned int max, unsigned int random_value )
{
  stream_out_<< "Line(" << parser.GetInfo().line_number_ << ")" <<
    " : #RANDOM " << max << " : Generate => " << random_value << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtBmsParseEnd( const std::string& filename, BmsBms& bms )
{
  NOT_USE_VAR( filename );

  stream_out_ << "TITLE : ";
  if ( bms.GetHeaders().IsExists( "TITLE" ) ) {
    stream_out_ << bms.GetHeaders()["TITLE"];
  }
  stream_out_ << StreamOut::PERIOD;

  stream_out_ << "ARTIST : ";
  if ( bms.GetHeaders().IsExists( "ARTIST" ) ) {
    stream_out_ << bms.GetHeaders()["ARTIST"];
  }
  stream_out_ << StreamOut::PERIOD;

  stream_out_ << "BPM : ";
  if ( bms.GetHeaders().IsExists( "BPM" ) ) {
    stream_out_ << bms.GetHeaders()["BPM"];
  }
  stream_out_ << StreamOut::PERIOD;

  stream_out_ << "最大小節 : " << bms.GetObjectExistsMaxBarPosition() << StreamOut::PERIOD;
  stream_out_ << "分解能 : " << bms.GetBarManager().GetBarDivisionCount() << StreamOut::PERIOD;
  stream_out_ << "WAV 登録数 : " << bms.GetRegistArraySet()["WAV"].GetExistCount() << StreamOut::PERIOD;
  stream_out_ << "BMP 登録数 : " << bms.GetRegistArraySet()["BMP"].GetExistCount() << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtWavFileReadStart( unsigned int file_count )
{
  NOT_USE_VAR( file_count );

  partial_time_ = TtTime();
}

void
Console::DetailOutputer::AtOneWavFileReadStart( BmsWord word, const std::string& filename )
{
  stream_out_ << "WAV" << word << " read : " << filename << "  start ... ";
}

void
Console::DetailOutputer::AtOneWavFileReadEnd( BmsWord word, const std::string& filename )
{
  NOT_USE_VAR( word );
  NOT_USE_VAR( filename );

  stream_out_ << "end" << StreamOut::PERIOD;
}

bool
Console::DetailOutputer::AtWavFileNotExists( BmsWord word, const std::string& filename, bool wav_was_used )
{
  NOT_USE_VAR( wav_was_used );

  stream_out_ << "WAV" << word << " file not exists : " << filename << StreamOut::PERIOD;
  stream_out_ << "but bmx2wav can ignore." << StreamOut::PERIOD;
  stream_out_ << "ignore WAV file lack : false" << StreamOut::PERIOD;

  return false;
}

bool
Console::DetailOutputer::AtWavFileFormatError( BmsWord word, InvalidWavFileFormatException& e )
{
  stream_out_ << "WAV" << word << " file format error : " << e.GetFileName() << StreamOut::PERIOD;
  stream_out_ << e.Message() << StreamOut::PERIOD;
  stream_out_ << "bmx2wav can try again reading by ignoring trivial error." << StreamOut::PERIOD;
  stream_out_ << "try again reading by ignoring trivial error : true" << StreamOut::PERIOD;
  return true;
}

bool
Console::DetailOutputer::AtWavFileError( BmsWord word, WavFileError& e )
{
  stream_out_ << "WAV" << word << " file error : " << e.GetFileName() << StreamOut::PERIOD;
  stream_out_ << e.Message() << StreamOut::PERIOD;
  stream_out_ << "bmx2wav can ignore this wav file specify." << StreamOut::PERIOD;
  stream_out_ << "ignore this WAV file specify : false" << StreamOut::PERIOD;
  return false;
}

void
Console::DetailOutputer::AtWavFileReadEnd( unsigned int file_count )
{
  NOT_USE_VAR( file_count );

  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << "WAV 読み込み時間 : " << time << " second" << StreamOut::PERIOD;
}

bool
Console::DetailOutputer::AtScrollObjectDefineError( BmsWord word, const std::string& name, const std::string& value )
{
  stream_out_ << "Scroll Object Define Error : " << name << word << " : " << value << StreamOut::PERIOD;
  stream_out_ << "ignore this scroll object define : false" << StreamOut::PERIOD;
  return false;
}

void
Console::DetailOutputer::AtMixStart( unsigned int start_position, unsigned int end_position )
{
  partial_time_ = TtTime();

  stream_out_ << "変換開始位置 : " << start_position << StreamOut::PERIOD;
  stream_out_ << "変換終了位置 : " << end_position << StreamOut::PERIOD;
}

bool
Console::DetailOutputer::AtScrollObjectEntryIsEmpty( BmsWord word, const std::string& name )
{
  stream_out_ << "Scroll Object Entry Is Empty : " << name << word << StreamOut::PERIOD;
  stream_out_ << "ignore this scroll object : false" << StreamOut::PERIOD;
  return false;
}

void
Console::DetailOutputer::AtMixEnd( unsigned int end_position )
{
  NOT_USE_VAR( end_position );

  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << "合成時間 : " << time << " second" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtNormalizeStart( void )
{
  partial_time_ = TtTime();

  stream_out_ << "ノーマライズ種別 : ";

  switch ( converter_.GetParameterSet().normalize_kind_ ) {
  case Parameter::Normalize::PEAK:
    stream_out_ << "Peak Normalize";
    break;

  case Parameter::Normalize::AVERAGE:
    stream_out_ << "Average Normalize";
    break;

  case Parameter::Normalize::OVER:
  case Parameter::Normalize::DEFAULT:
    stream_out_ << "Over Normalize : OverPPM : " << converter_.GetParameterSet().normalize_over_ppm_;
    break;

  default:
    stream_out_ << "??????";
    break;
  }
  stream_out_ << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtNormalizeEnd( double change_ratio )
{
  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << "ノーマライズの音量調整率 : " << change_ratio << " 倍" << StreamOut::PERIOD;
  stream_out_ << "processing time : " << time << " second" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtCompressStart( double threshold, double ratio, unsigned int attack_time, unsigned int release_time, int look_ahead )
{
  partial_time_ = TtTime();

  stream_out_ << "compress threshold : " << threshold  << StreamOut::PERIOD;
  stream_out_ << "compress ratio : " << ratio << StreamOut::PERIOD;
  stream_out_ << "compress attack time : " << attack_time << StreamOut::PERIOD;
  stream_out_ << "compress release time : " << release_time << StreamOut::PERIOD;
  stream_out_ << "compress look_ahead : " << look_ahead << StreamOut::PERIOD;
  stream_out_ << "compressor start ... ";
}

void
Console::DetailOutputer::AtCompressEnd( double threshold, double ratio, unsigned int attack_time, unsigned int release_time, int look_ahead )
{
  NOT_USE_VAR( threshold );
  NOT_USE_VAR( ratio );
  NOT_USE_VAR( attack_time );
  NOT_USE_VAR( release_time );
  NOT_USE_VAR( look_ahead );

  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << "end ( " << time << " second)" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtVolumeChangeStart( int percentage )
{
  partial_time_ = TtTime();

  stream_out_ << "音量調整比率 " << percentage << " %   変更 start ... ";
}

void
Console::DetailOutputer::AtVolumeChangeEnd( int percentage )
{
  NOT_USE_VAR( percentage );

  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << "end (" << time << " second)" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtWriteToFileStart( const std::string& filename )
{
  partial_time_ = TtTime();

  stream_out_ << "output WAV : " << filename << "  start ... ";
}

void
Console::DetailOutputer::AtWriteToFileEnd( const std::string& filename )
{
  NOT_USE_VAR( filename );

  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << "end (" << time << " second)" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtAfterProcessStart( TtProcess::CreateInfo& info )
{
  partial_time_ = TtTime();

  stream_out_ << "after process start" << StreamOut::PERIOD;
  stream_out_ << "execute File : " << info.GetFileName() << StreamOut::PERIOD;
  stream_out_ << "arguments : " << info.GetArguments() << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtCreatedProcess( TtProcess& process, TtProcess::CreateInfo& info )
{
  NOT_USE_VAR( info );

  stream_out_ << "process start : process ID : " << static_cast<unsigned int>( process.GetProcessId() ) << StreamOut::PERIOD;
  fflush( stdout );
}

void
Console::DetailOutputer::AtWaitForProcessStart( TtProcess& process )
{
  NOT_USE_VAR( process );

  stream_out_ << "waiting for process exit : start ... " << StreamOut::PERIOD;
  fflush( stdout );
}

void
Console::DetailOutputer::AtWaitForProcessEnd( TtProcess& process )
{
  NOT_USE_VAR( process );

  stream_out_ << StreamOut::PERIOD;
  stream_out_ << "end" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtDeleteFileStart( const std::string& filename )
{
  stream_out_ << "delete wav file : " << filename << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtDeleteFileEnd( const std::string& filename, bool result )
{
  NOT_USE_VAR( filename );

  stream_out_ << "delete wav file result : " << (result ? "success" : "fail") << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtAfterProcessEnd( TtProcess& process, TtProcess::CreateInfo& info, const std::string& outputed_filename, BmsBms& bms )
{
  NOT_USE_VAR( process );
  NOT_USE_VAR( info );
  NOT_USE_VAR( outputed_filename );
  NOT_USE_VAR( bms );

  double time = TtTime::GetNow() - partial_time_;

  stream_out_ << "after process end (" << time << " second)" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtFailStartProcess( TtProcess::CreateInfo& info )
{
  stream_out_ << "process start error occurred" << StreamOut::PERIOD;
  stream_out_ << "execute File : " << info.GetFileName() << StreamOut::PERIOD;
  stream_out_ << "arguments : " << info.GetArguments() << StreamOut::PERIOD;
}


void
Console::DetailOutputer::AtConvertEnd( void )
{
  double time = TtTime::GetNow() - all_time_;

  stream_out_ << "変換時間 : " << time << " second" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtConvertError( TtMessageBoxOk& box )
{
  stream_out_ << StreamOut::PERIOD;
  stream_out_ << "=== エラー発生 ===" << StreamOut::PERIOD;
  stream_out_ << "--- メッセージ --------------------------------" << StreamOut::PERIOD;
  stream_out_ << box.GetMessage() << StreamOut::PERIOD;
  stream_out_ << "-----------------------------------------------" << StreamOut::PERIOD;
}

void
Console::DetailOutputer::AtConvertAbort( TtMessageBoxOk& box )
{
  NOT_USE_VAR( box );

  stream_out_ << StreamOut::PERIOD;
  stream_out_ << "変換は中断されました。" << StreamOut::PERIOD;
}
