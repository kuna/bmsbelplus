#include <sstream>

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_message_box.h"
#include "tt_path.h"
#include "tt_util.h"

#include "bms_parser.h"
#include "bms_exception.h"

#include "bmx2wav_exception.h"
#include "bmx2wav_template_path_converter.h"

#include "bmx2wav_converting_dialog.h"

using namespace Bmx2Wav;

namespace {
  class EverySelectDialog : public TtDialog {
  public:
    enum {
      ID_BUTTON_OK    = 10001,
      ID_BUTTON_ABORT,
    };

  public:
    explicit EverySelectDialog( unsigned int line, unsigned int max ) :
    line_( line ), max_( max ), selected_value_( 0 ) {}

    virtual DWORD GetStyle( void ) {
      return this->TtDialog::GetStyle() & (~WS_SYSMENU);
    }

    virtual bool Created( void ) {
      this->SetText( "ランダム値選択" );
      this->SetIcon( ::LoadIcon( ::GetModuleHandle( NULL ), "MAIN_ICON" ), true );

      {
        TtWindow::CreateParameter parameter( this );
        label1_.Create( parameter );
        label2_.Create( parameter );
        label3_.Create( parameter );
        edit_.Create( parameter );
        ok_button_.Create( parameter.AlterId( ID_BUTTON_OK ) );
        abort_button_.Create( parameter.AlterId( ID_BUTTON_ABORT ) );
      }

      this->SetSize( 300, 140 );
      label1_.SetPositionSize(         4,   4, 280, 36 );
      label2_.SetPositionSize(        32,  36, 140, 16 );
      label3_.SetPositionSize(        32,  54, 140, 16 );
      edit_.SetPositionSize(         180,  50,  48, 20 );
      ok_button_.SetPositionSize(     60,  84,  80, 20 );
      abort_button_.SetPositionSize( 160,  84,  80, 20 );

      label1_.SetText( "BMS 中で #RANDOM で乱数指定されました。\r\n値を指定して下さい。" );

      {
        std::string tmp;
        tmp.append( "行番号 : " );
        tmp.append( TtUtil::ToStringFrom( line_ ) );
        label2_.SetText( tmp );
      }
      {
        std::string tmp;
        tmp.append( "指定可能範囲 : 1 〜 " );
        tmp.append( TtUtil::ToStringFrom( max_ ) );
        label3_.SetText( tmp );
      }
      ok_button_.SetText( "OK" );
      abort_button_.SetText( "中止" );

      label1_.Show();
      label2_.Show();
      label3_.Show();
      edit_.Show();
      ok_button_.Show();
      abort_button_.Show();

      this->SetCenterRelativeToParent();

      edit_.SetFocus();
      return true;
    }

    virtual LRESULT WMCommandReceived( int id, int code, HWND child ) {
      switch ( id ) {
      case ID_BUTTON_OK: {
        int tmp;
        if ( NOT( TtUtil::StringToInteger( edit_.GetText(), &tmp, 10 ) ) ||
             tmp < 1 || tmp > static_cast<int>( max_ ) ) {
          TtMessageBoxOk box;
          box.SetMessage( "値を正しく指定して下さい。" );
          box.SetCaption( "エラー" );
          box.SetIcon( TtMessageBox::Icon::ERROR );
          box.ShowDialog( *this );
          return 0;
        }
        selected_value_ = tmp;
        this->EndDialog( true );
        return 0;
      }

      case ID_BUTTON_ABORT: {
        this->EndDialog( false );
      }

      }
      return this->TtForm::WMCommandReceived( id, code, child );
    }

    unsigned int GetSelectedValue( void ) { return selected_value_; }

  private:
    TtStatic label1_;
    TtStatic label2_;
    TtStatic label3_;
    TtEditNumber edit_;
    TtButton ok_button_;
    TtButton abort_button_;

    unsigned int line_;
    unsigned int max_;
    unsigned int selected_value_;
  };
}

// -- Converter::ProgressReceiver ----------------------------------------
void
ConvertingDialog::AtConvertStart( void )
{
  this->GetSystemMenu().GetItemById( SC_CLOSE ).SetEnabled( false );

  bms_file_label_.SetText( TtUtil::StringReplace(
    TtPath::CompactPath( converter_.GetParameterSet().input_filename_, 75 ), "&", "&&" ) );

  all_time_ = TtTime();

  detail_log_edit_ << "BMS File : " << converter_.GetParameterSet().input_filename_ << DetailLogEdit::PERIOD;
}

BmsParser::Reactor&
ConvertingDialog::GetBmsParserReactor( void )
{
  return *this;
}

void
ConvertingDialog::AtBmsParseStart( const std::string& filename )
{
  NOT_USE_VAR( filename );
  explanation_label_.SetText( "BMS ファイルを解析しています。" );
}


// -- BmsParser::Reactor ---------------------------------------------
std::string
ConvertingDialog::AtDuplicateHeader( BmsParser::Parser& parser,
                                     const std::string header,
                                     const std::string before,
                                     const std::string present )
{
  switch ( converter_.GetParameterSet().parse_duplicate_header_reaction_ ) {
  case Parameter::ParseDuplicateHeaderReaction::OVERWRITE:
    return present;

  case Parameter::ParseDuplicateHeaderReaction::REMAIN:
    return before;

  case Parameter::ParseDuplicateHeaderReaction::ERROR:
    return this->BmsParser::Reactor::AtDuplicateHeader( parser, header, before, present );

  case Parameter::ParseDuplicateHeaderReaction::ASK:
  case Parameter::ParseDuplicateHeaderReaction::DEFAULT:
  default:
    break;
  }
  TtMessageBoxYesNoCancel box;
  ( box.AppendMessage() << "ヘッダが二重に指定されました。\r\n" <<
    "行番号 : " << parser.GetInfo().line_number_ << "\r\n" <<
    "ヘッダ名 : " << header << "\r\n" <<
    "以前の指定 : " << before << "\r\n" <<
    "今回の指定 : " << present << "\r\n\r\n" <<
    "以前の指定を今回の指定で上書きしますか？" );
  box.SetCaption( "ヘッダの重複指定" );
  box.SetIcon( TtMessageBox::Icon::WARNING );
  switch ( box.ShowDialog( *this ) ) {
  case TtMessageBox::Result::YES:
    return present;

  case TtMessageBox::Result::NO:
    return before;

  case TtMessageBox::Result::CANCEL:
  default:
    return this->BmsParser::Reactor::AtDuplicateHeader( parser, header, before, present );
  }
}

double
ConvertingDialog::AtDuplicateBarChange( BmsParser::Parser& parser,
                                        unsigned int bar,
                                        double       before,
                                        double       present )
{
  switch ( converter_.GetParameterSet().parse_duplicate_bar_change_reaction_ ) {
  case Parameter::ParseDuplicateBarChangeReaction::OVERWRITE:
    return present;

  case Parameter::ParseDuplicateBarChangeReaction::REMAIN:
    return before;

  case Parameter::ParseDuplicateBarChangeReaction::ERROR:
    return this->BmsParser::Reactor::AtDuplicateBarChange( parser, bar, before, present );

  case Parameter::ParseDuplicateBarChangeReaction::ASK:
  case Parameter::ParseDuplicateBarChangeReaction::DEFAULT:
  default:
    break;
  }
  TtMessageBoxYesNoCancel box;
  ( box.AppendMessage() << "小節長変更が二重に指定されました。\r\n" <<
    "行番号 : " << parser.GetInfo().line_number_ << "\r\n" <<
    "小節番号 : " << bar << "\r\n" <<
    "以前の指定 : " << before << "\r\n" <<
    "今回の指定 : " << present << "\r\n\r\n" <<
    "以前の指定を今回の指定で上書きしますか？" );
  box.SetCaption( "小節長変更の重複指定" );
  box.SetIcon( TtMessageBox::Icon::WARNING );
  switch ( box.ShowDialog( *this ) ) {
  case TtMessageBox::Result::YES:
    return present;

  case TtMessageBox::Result::NO:
    return before;

  case TtMessageBox::Result::CANCEL:
  default:
    return this->BmsParser::Reactor::AtDuplicateBarChange( parser, bar, before, present );
  }
}

bool
ConvertingDialog::AtParseError( BmsParser::Parser& parser, BmsParseException& e, const std::string& line )
{
  NOT_USE_VAR( parser );
  switch ( converter_.GetParameterSet().parse_error_reaction_ ) {
  case Parameter::ParseErrorReaction::IGNORE:
    this->AtParseErrorIgnored( e, line );
    return true;

  case Parameter::ParseErrorReaction::ASK:
  case Parameter::ParseErrorReaction::DEFAULT:
  default:
    break;

  case Parameter::ParseErrorReaction::ERROR:
    return false;
  }
  TtMessageBoxYesNo box;
  ( box.AppendMessage() << "BMS ファイル解析中にエラーが発生しました。\r\n" <<
    "行番号 : " << e.GetLine() << "\r\n" <<
    "メッセージ : " << e.Message() << "\r\n" <<
    "行の内容 : " );
  {
    std::string tmp = line;
    if ( line.length() > 60 ) {
      tmp.clear();
      tmp.append( line.substr( 0, 29 ) );
      tmp.append( " ... " );
      tmp.append( line.substr( line.length() - 29 ) );
    }
    box.AppendMessage( tmp );
  }
  box.AppendMessage( "\r\n\r\nこの行を無視して続行しますか？" );
  box.SetCaption( "BMS ファイル解析エラー無視の確認" );
  box.SetIcon( TtMessageBox::Icon::WARNING );
  unsigned int result = box.ShowDialog( *this );
  if ( result == TtMessageBox::Result::YES ) {
    this->AtParseErrorIgnored( e, line );
  }
  return result == TtMessageBox::Result::YES;
}

unsigned int
ConvertingDialog::AtGenerateRandom( BmsParser::Parser& parser, unsigned int max )
{
  unsigned int random_value;
  switch ( converter_.GetParameterSet().random_kind_ ) {
  case Parameter::Random::EVERY_SELECT:
  case Parameter::Random::DEFAULT:
  default: {
    EverySelectDialog dx( parser.GetInfo().line_number_, max );
    if ( NOT( dx.ShowDialog( *this ) ) ) {
      throw ConvertAbortException();
    }
    random_value = dx.GetSelectedValue();
    break;
  }

  case Parameter::Random::CONSTANT:
    random_value = converter_.GetParameterSet().random_constant_value_;
    if ( random_value > max ) {
      random_value = 0;
    }
    break;

  case Parameter::Random::REAL_RANDOM:
    random_value = this->BmsParser::Reactor::AtGenerateRandom( parser, max );
    break;
  }
  this->AtRandomValueGenerated( parser, max, random_value );
  return random_value;
}

void
ConvertingDialog::AtLineDataRead( BmsParser::Parser& parser )
{
  NOT_USE_VAR( parser );
  this->AbortIfAbortMessageExistsInQueue();
}

void
ConvertingDialog::AtLineEnd( BmsParser::Parser& parser )
{
  NOT_USE_VAR( parser );
  this->AbortIfAbortMessageExistsInQueue();
}

void
ConvertingDialog::AtParseErrorIgnored( BmsParseException& e, const std::string& line )
{
  detail_log_edit_<< "BMS Parse Error : " << typeid( e ).name()        << DetailLogEdit::PERIOD;
  detail_log_edit_<< "line number : " << e.GetLine()                   << DetailLogEdit::PERIOD;
  detail_log_edit_<< "line data : " << line                            << DetailLogEdit::PERIOD;
  detail_log_edit_<< "--- message -----------------------------------" << DetailLogEdit::PERIOD;
  detail_log_edit_<<  e.Message()                                      << DetailLogEdit::PERIOD;
  detail_log_edit_<< "-----------------------------------------------" << DetailLogEdit::PERIOD;
  detail_log_edit_<< "this error ignored."                             << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtRandomValueGenerated( BmsParser::Parser& parser, unsigned int max, unsigned int random_value )
{
  detail_log_edit_<< "Line(" << parser.GetInfo().line_number_ << ")" <<
    " : #RANDOM " << max << " : Generate => " << random_value << DetailLogEdit::PERIOD;
}


// -- Converter::ProgressReceiver ----------------------------------------
void
ConvertingDialog::AtBmsParseEnd( const std::string& filename, BmsBms& bms )
{
  NOT_USE_VAR( filename );

  std::string tmp;
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
  bms_title_artist_label_.SetText( TtUtil::StringReplace( tmp, "&", "&&" ) );

  detail_log_edit_ << "TITLE : ";
  if ( bms.GetHeaders().IsExists( "TITLE" ) ) {
    detail_log_edit_ << bms.GetHeaders()["TITLE"];
  }
  detail_log_edit_ << DetailLogEdit::PERIOD;

  detail_log_edit_ << "ARTIST : ";
  if ( bms.GetHeaders().IsExists( "ARTIST" ) ) {
    detail_log_edit_ << bms.GetHeaders()["ARTIST"];
  }
  detail_log_edit_ << DetailLogEdit::PERIOD;

  detail_log_edit_ << "BPM : ";
  if ( bms.GetHeaders().IsExists( "BPM" ) ) {
    detail_log_edit_ << bms.GetHeaders()["BPM"];
  }
  detail_log_edit_ << DetailLogEdit::PERIOD;

  detail_log_edit_ << "最大小節 : " << bms.GetObjectExistsMaxBarPosition() << DetailLogEdit::PERIOD;
  detail_log_edit_ << "分解能 : " << bms.GetBarManager().GetBarDivisionCount() << DetailLogEdit::PERIOD;
  detail_log_edit_ << "WAV 登録数 : " << bms.GetRegistArraySet()["WAV"].GetExistCount() << DetailLogEdit::PERIOD;
  detail_log_edit_ << "BMP 登録数 : " << bms.GetRegistArraySet()["BMP"].GetExistCount() << DetailLogEdit::PERIOD;

  this->AbortIfAbortMessageExistsInQueue();
}

WavMaker::Reactor&
ConvertingDialog::GetWavMakerReactor( void )
{
  return *this;
}

// -- WavMaker::Reactor ----------------------------------------------
void
ConvertingDialog::AtOneTickRead( void )
{
  this->AbortIfAbortMessageExistsInQueue();
}

// -- Converter::ProgressReceiver ----------------------------------------
void
ConvertingDialog::AtWavFileReadStart( unsigned int file_count )
{
  explanation_label_.SetText( "WAV ファイルを読み込んでいます。" );

  wav_read_progress_.SetMaximum( static_cast<unsigned short>( file_count ) );
  wav_read_progress_.SetAmount( 0 );
  wav_read_progress_.SetStep( 1 );

  partial_time_ = TtTime();
}

void
ConvertingDialog::AtOneWavFileReadStart( BmsWord word, const std::string& filename )
{
  wav_read_label_.SetText( TtPath::CompactPath( filename, 75 ) );

  detail_log_edit_ << "WAV" << word << " read : " << filename << "  start ... ";

  this->AbortIfAbortMessageExistsInQueue();
}

void
ConvertingDialog::AtOneWavFileReadEnd( BmsWord word, const std::string& filename )
{
  NOT_USE_VAR( word );
  NOT_USE_VAR( filename );

  wav_read_progress_.StepIt();

  detail_log_edit_ << "end" << DetailLogEdit::PERIOD;

  this->AbortIfAbortMessageExistsInQueue();
}

bool
ConvertingDialog::AtWavFileNotExists( BmsWord word, const std::string& filename, bool wav_was_used )
{
  detail_log_edit_ << "WAV" << word << " file not exists : " << filename << DetailLogEdit::PERIOD;
  detail_log_edit_ << "but bmx2wav can ignore." << DetailLogEdit::PERIOD;
  detail_log_edit_ << "ignore WAV file lack : ";

  switch ( converter_.GetParameterSet().wav_file_not_found_reaction_ ) {
  case Parameter::WavFileNotFoundReaction::ERROR:
    detail_log_edit_ << "false" << DetailLogEdit::PERIOD;
    return false;

  case Parameter::WavFileNotFoundReaction::ASK_ONLY_ERROR:
    if ( NOT( wav_was_used ) ) {
      return true;
    }
    // walk through
  case Parameter::WavFileNotFoundReaction::MUST_ASK:
  case Parameter::WavFileNotFoundReaction::DEFAULT:
  default: {
    TtMessageBoxYesNo box;
    ( box.AppendMessage() << "以下の WAV ファイルが指定されましたが存在しませんでした。\r\n" <<
      "ヘッダ : WAV" << word << "\r\n" <<
      "ファイル名 : " << filename << "\r\n" <<
      ( wav_was_used ? "" : "\r\nしかし、BMS 内で使用されていないので正常に変換できます。\r\n") <<
      "エラーを無視して変換を続行しますか？" );
    box.SetCaption( "続行確認" );
    box.SetIcon( wav_was_used ? TtMessageBox::Icon::ERROR : TtMessageBox::Icon::WARNING );
    bool ret = box.ShowDialog( *this ) == TtMessageBox::Result::YES;
    detail_log_edit_ << ( ret ? "true" : "false" ) << DetailLogEdit::PERIOD;
    return ret;
  }

  case Parameter::WavFileNotFoundReaction::IGNORE:
    detail_log_edit_ << "true" << DetailLogEdit::PERIOD;
    return true;
  }
}

bool
ConvertingDialog::AtWavFileFormatError( BmsWord word, InvalidWavFileFormatException& e )
{
  detail_log_edit_ << "WAV" << word << " file format error : " << e.GetFileName() << DetailLogEdit::PERIOD;
  detail_log_edit_ << e.Message() << DetailLogEdit::PERIOD;
  detail_log_edit_ << "bmx2wav can try again reading by ignoring trivial error." << DetailLogEdit::PERIOD;
  detail_log_edit_ << "try again reading by ignoring trivial error : ";

  switch ( converter_.GetParameterSet().wav_format_error_reaction_ ) {
  case Parameter::WavFormatErrorReaction::ERROR:
    detail_log_edit_ << "false" << DetailLogEdit::PERIOD;
    return false;

  case Parameter::WavFormatErrorReaction::ASK:
  case Parameter::WavFormatErrorReaction::DEFAULT:
  default: {
    TtMessageBoxYesNo box;
    ( box.AppendMessage() << "以下の WAV ファイルの読み込み中にエラーが検出されました。\r\n" <<
      "ヘッダ : WAV" << word << "\r\n" <<
      e.Message() << "\r\n\r\n" <<
      "再度、些細なエラーをとても若干無視して読み込む事ができます。\r\n" <<
      "試してみますか？" );
    box.SetCaption( "続行確認" );
    box.SetIcon( TtMessageBox::Icon::WARNING );
    bool ret = box.ShowDialog( *this ) == TtMessageBox::Result::YES;
    detail_log_edit_ << ( ret ? "true" : "false" ) << DetailLogEdit::PERIOD;
    return ret;
  }

  case Parameter::WavFormatErrorReaction::IGNORE:
    detail_log_edit_ << "true" << DetailLogEdit::PERIOD;
    return true;
  }
}

bool
ConvertingDialog::AtWavFileError( BmsWord word, WavFileError& e )
{
  detail_log_edit_ << "WAV" << word << " file error : " << e.GetFileName() << DetailLogEdit::PERIOD;
  detail_log_edit_ << e.Message() << DetailLogEdit::PERIOD;
  detail_log_edit_ << "bmx2wav can ignore this wav file specify." << DetailLogEdit::PERIOD;
  detail_log_edit_ << "ignore this WAV file specify : ";

  switch ( converter_.GetParameterSet().wav_file_error_reaction_ ) {
  case Parameter::WavFileErrorReaction::ERROR:
    detail_log_edit_ << "false" << DetailLogEdit::PERIOD;
    return false;

  case Parameter::WavFileErrorReaction::ASK:
  case Parameter::WavFileErrorReaction::DEFAULT:
  default: {
    TtMessageBoxYesNo box;
    ( box.AppendMessage() << "以下の WAV ファイルの読み込み中にエラーが検出されました。\r\n" <<
      "ヘッダ : WAV" << word << "\r\n" <<
      e.Message() << "\r\n\r\n" <<
      "この WAV ファイル指定を無視して変換を続行しますか？" );
    box.SetCaption( "続行確認" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    bool ret = box.ShowDialog( *this ) == TtMessageBox::Result::YES;
    detail_log_edit_ << ( ret ? "true" : "false" ) << DetailLogEdit::PERIOD;
    return ret;
  }

  case Parameter::WavFileErrorReaction::IGNORE:
    detail_log_edit_ << "true" << DetailLogEdit::PERIOD;
    return true;
  }
}


void
ConvertingDialog::AtWavFileReadEnd( unsigned int file_count )
{
  double time = TtTime::GetNow() - partial_time_;

  std::ostringstream os;
  os << "WAV ファイルの読み込み完了(" << file_count << " file : " << time << " 秒)";
  wav_read_label_.SetText( os.str() );

  detail_log_edit_ << "WAV 読み込み時間 : " << time << " second" << DetailLogEdit::PERIOD;

  this->AbortIfAbortMessageExistsInQueue();
}

bool
ConvertingDialog::AtScrollObjectDefineError( BmsWord word, const std::string& name, const std::string& value )
{
  detail_log_edit_ << "Scroll Object Define Error : " << name << word << " : " << value << DetailLogEdit::PERIOD;
  detail_log_edit_ << "ignore this scroll object define : ";

  switch ( converter_.GetParameterSet().scroll_object_define_error_reaction_ ) {
  case Parameter::ScrollObjectDefineErrorReaction::ERROR:
    detail_log_edit_ << "false" << DetailLogEdit::PERIOD;
    return false;

  case Parameter::ScrollObjectDefineErrorReaction::ASK:
  case Parameter::ScrollObjectDefineErrorReaction::DEFAULT:
  default: {
    TtMessageBoxYesNo box;
    ( box.AppendMessage() << "以下のスクロールオブジェクト定義でエラーが検出されました。\r\n" <<
      "ヘッダ : " <<  name << word << "\r\n" <<
      "値 : " << value << "\r\n\r\n" <<
      "この定義を無視して変換を続行しますか？" );
    box.SetCaption( "続行確認" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    bool ret = box.ShowDialog( *this ) == TtMessageBox::Result::YES;
    detail_log_edit_ << ( ret ? "true" : "false" ) << DetailLogEdit::PERIOD;
    return ret;
  }

  case Parameter::ScrollObjectDefineErrorReaction::IGNORE:
    detail_log_edit_ << "true" << DetailLogEdit::PERIOD;
    return true;
  }
}

void
ConvertingDialog::AtMixStart( unsigned int start_position, unsigned int end_position )
{
  explanation_label_.SetText( "WAV ファイルを合成しています。" );

  wav_mix_label_.SetText( "WAV ファイルの合成" );

  mix_end_position_ = end_position;
  wav_mix_progress_.SetMaximum( 200 );
  wav_mix_progress_.SetAmount( 0 );
  wav_mix_progress_.SetStep( 1 );

  partial_time_ = TtTime();

  detail_log_edit_ << "変換開始位置 : " << start_position << DetailLogEdit::PERIOD;
  detail_log_edit_ << "変換終了位置 : " << end_position << DetailLogEdit::PERIOD;
}

bool
ConvertingDialog::AtScrollObjectEntryIsEmpty( BmsWord word, const std::string& name )
{
  detail_log_edit_ << "Scroll Object Entry Is Empty : " << name << word << DetailLogEdit::PERIOD;
  detail_log_edit_ << "ignore this scroll object : ";

  switch ( converter_.GetParameterSet().scroll_object_entry_is_empty_reaction_ ) {
  case Parameter::ScrollObjectEntryIsEmptyReaction::ERROR:
    detail_log_edit_ << "false" << DetailLogEdit::PERIOD;
    return false;

  case Parameter::ScrollObjectEntryIsEmptyReaction::ASK:
  case Parameter::ScrollObjectEntryIsEmptyReaction::DEFAULT:
  default: {
    TtMessageBoxYesNo box;
    ( box.AppendMessage() << "以下の未定義のスクロールオブジェクトが使用されました\r\n" <<
      "ヘッダ : " << name << word << "\r\n\r\n" <<
      "このオブジェクトを無視して変換を続行しますか？" );
    box.SetCaption( "続行確認" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    bool ret = box.ShowDialog( *this ) == TtMessageBox::Result::YES;
    detail_log_edit_ << ( ret ? "true" : "false" ) << DetailLogEdit::PERIOD;
    return ret;
  }

  case Parameter::ScrollObjectEntryIsEmptyReaction::IGNORE:
    detail_log_edit_ << "true" << DetailLogEdit::PERIOD;
    return true;
  }
}

void
ConvertingDialog::AtMixProgress( unsigned int current_position )
{
  if ( current_position == mix_end_position_ ||
       current_position % (mix_end_position_ > 200 ? (mix_end_position_ / 200) : 1) == 0 ) {
    wav_mix_progress_.SetAmount( static_cast<unsigned short>( ((current_position + 1) * 200) / (mix_end_position_ + 1) ) );
  }

  this->AbortIfAbortMessageExistsInQueue();
}

void
ConvertingDialog::AtMixEnd( unsigned int end_position )
{
  double time = TtTime::GetNow() - partial_time_;

  std::ostringstream os;
  os << "WAV ファイルの合成完了(" << end_position << " step : " << time << " 秒)";
  wav_mix_label_.SetText( os.str() );

  detail_log_edit_ << "合成時間 : " << time << " second" << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtNormalizeStart( void )
{
  explanation_label_.SetText( "音量のノーマライズ中です。" );

  partial_time_ = TtTime();

  detail_log_edit_ << "ノーマライズ種別 : ";
  switch ( converter_.GetParameterSet().normalize_kind_ ) {
  case Parameter::Normalize::PEAK:
    detail_log_edit_ << "Peak Normalize";
    break;

  case Parameter::Normalize::AVERAGE:
    detail_log_edit_ << "Average Normalize";
    break;

  case Parameter::Normalize::OVER:
  case Parameter::Normalize::DEFAULT:
    detail_log_edit_ << "Over Normalize : OverPPM : " << converter_.GetParameterSet().normalize_over_ppm_;
    break;

  default:
    detail_log_edit_ << "??????";
    break;
  }
  detail_log_edit_ << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtNormalizeEnd( double change_ratio )
{
  double time = TtTime::GetNow() - partial_time_;

  detail_log_edit_ << "ノーマライズの音量調整率 : " << change_ratio << " 倍" << DetailLogEdit::PERIOD;
  detail_log_edit_ << "processing time : " << time << " second" << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtCompressStart( double threshold, double ratio, unsigned int attack_time, unsigned int release_time, int look_ahead )
{
  explanation_label_.SetText( "コンプレッサ処理中です。" );

  partial_time_ = TtTime();

  detail_log_edit_ << "compress threshold : " << threshold  << DetailLogEdit::PERIOD;
  detail_log_edit_ << "compress ratio : " << ratio << DetailLogEdit::PERIOD;
  detail_log_edit_ << "compress attack time : " << attack_time << DetailLogEdit::PERIOD;
  detail_log_edit_ << "compress release time : " << release_time << DetailLogEdit::PERIOD;
  detail_log_edit_ << "compress look_ahead : " << look_ahead << DetailLogEdit::PERIOD;
  detail_log_edit_ << "compressor start ... ";
}

void
ConvertingDialog::AtCompressEnd( double threshold, double ratio, unsigned int attack_time, unsigned int release_time, int look_ahead )
{
  NOT_USE_VAR( threshold );
  NOT_USE_VAR( ratio );
  NOT_USE_VAR( attack_time );
  NOT_USE_VAR( release_time );
  NOT_USE_VAR( look_ahead );

  double time = TtTime::GetNow() - partial_time_;

  detail_log_edit_ << "end ( " << time << " second)" << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtVolumeChangeStart( int percentage )
{
  explanation_label_.SetText( "音量変更中です。" );

  partial_time_ = TtTime();

  detail_log_edit_ << "音量調整比率 " << percentage << " %   変更 start ... ";
}

void
ConvertingDialog::AtVolumeChangeEnd( int percentage )
{
  NOT_USE_VAR( percentage );

  double time = TtTime::GetNow() - partial_time_;

  detail_log_edit_ << "end (" << time << " second)" << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtWriteToFileStart( const std::string& filename )
{
  outputed_filename_ = filename;

  explanation_label_.SetText( "ファイルに書き込みをしています。" );

  partial_time_ = TtTime();

  detail_log_edit_ << "output WAV : " << filename << "  start ... ";
}

void
ConvertingDialog::AtWriteToFileEnd( const std::string& filename )
{
  NOT_USE_VAR( filename );

  double time = TtTime::GetNow() - partial_time_;

  detail_log_edit_ << "end (" << time << " second)" << DetailLogEdit::PERIOD;
}


void
ConvertingDialog::AtAfterProcessStart( TtProcess::CreateInfo& info )
{
  explanation_label_.SetText( "後処理を実行しています。" );

  partial_time_ = TtTime();

  detail_log_edit_ << "after process start" << DetailLogEdit::PERIOD;
  detail_log_edit_ << "execute File : " << info.GetFileName() << DetailLogEdit::PERIOD;
  detail_log_edit_ << "arguments : " << info.GetArguments() << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtCreatedProcess( TtProcess& process, TtProcess::CreateInfo& info )
{
  NOT_USE_VAR( info );

  detail_log_edit_ << "process start : process ID : " << static_cast<unsigned int>( process.GetProcessId() ) << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtWaitForProcessStart( TtProcess& process )
{
  NOT_USE_VAR( process );

  explanation_label_.SetText( "後処理を実行しています。終了を待っています。" );

  detail_log_edit_ << "waiting for process exit : start ... ";
}

void
ConvertingDialog::AtWaitForProcessEnd( TtProcess& process )
{
  NOT_USE_VAR( process );

  detail_log_edit_ << "end" << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtDeleteFileStart( const std::string& filename )
{
  explanation_label_.SetText( "WAV ファイルを削除しています。" );

  detail_log_edit_ << "delete wav file : " << filename << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtDeleteFileEnd( const std::string& filename, bool result )
{
  NOT_USE_VAR( filename );

  detail_log_edit_ << "delete wav file result : " << (result ? "success" : "fail") << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtAfterProcessEnd( TtProcess& process, TtProcess::CreateInfo& info, const std::string& outputed_filename, BmsBms& bms )
{
  NOT_USE_VAR( process );
  NOT_USE_VAR( info );

  explanation_label_.SetText( "後処理が終了しました。" );

  double time = TtTime::GetNow() - partial_time_;

  if ( converter_.GetParameterSet().change_play_file_ ) {
    TemplatePathConverter path_converter( converter_.GetParameterSet().input_filename_,
                                          outputed_filename,
                                          bms );
    path_converter.SetRemoveCanNotUseCharAsFilePath( true );
    outputed_filename_ = path_converter.Convert( converter_.GetParameterSet().play_file_path_ );
    detail_log_edit_ << "play file changed : " << outputed_filename_ << DetailLogEdit::PERIOD;
  }
  detail_log_edit_ << "after process end (" << time << " second)" << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtFailStartProcess( TtProcess::CreateInfo& info )
{
  TtMessageBoxOk box;
  ( box.AppendMessage() << "プログラムの起動に失敗しました。\r\n\r\n" <<
    "実行ファイル名 : " << info.GetFileName() << "\r\n" <<
    "引数 : " << info.GetArguments() << "\r\n\r\n" <<
    "※注意 : 以下の WAV ファイルは出力されています。\r\n\r\n" <<
    "WAV ファイル名 : " << outputed_filename_ );
  box.SetCaption( "プログラム起動失敗" );
  box.SetIcon( TtMessageBox::Icon::ERROR );
  box.ShowDialog( *this );

  detail_log_edit_ << "process start error occurred" << DetailLogEdit::PERIOD;
  detail_log_edit_ << "execute File : " << info.GetFileName() << DetailLogEdit::PERIOD;
  detail_log_edit_ << "arguments : " << info.GetArguments() << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtConvertEnd( void )
{
  double time = TtTime::GetNow() - all_time_;

  if ( NOT( auto_close_ ) ) {
    TtMessageBoxOk::Show( *this, "BMS の変換が終了しました。", "終了", TtMessageBox::Icon::INFORMATION );
  }

  std::ostringstream os;
  os << "変換は終了しました。(" << time << " second)";
  explanation_label_.SetText( os.str() );

  is_success_ = true;

  play_button_.SetEnabled( true );
  abort_button_.SetEnabled( false );

  detail_log_edit_ << "変換時間 : " << time << " second" << DetailLogEdit::PERIOD;
}

void
ConvertingDialog::AtConvertError( TtMessageBoxOk& box )
{
  if ( NOT( auto_close_ ) ) {
    box.ShowDialog( *this );
  }

  explanation_label_.SetText( "変換中にエラーが発生しました。" );

  detail_log_edit_ << DetailLogEdit::PERIOD;
  detail_log_edit_ << "=== エラー発生 ===" << DetailLogEdit::PERIOD;
  detail_log_edit_ << "--- メッセージ --------------------------------" << DetailLogEdit::PERIOD;
  detail_log_edit_ << box.GetMessage() << DetailLogEdit::PERIOD;
  detail_log_edit_ << "-----------------------------------------------" << DetailLogEdit::PERIOD;

  abort_button_.SetEnabled( false );
}

void
ConvertingDialog::AtConvertAbort( TtMessageBoxOk& box )
{
  box.ShowDialog( *this );

  explanation_label_.SetText( "変換は中断されました。" );

  detail_log_edit_ << DetailLogEdit::PERIOD;
  detail_log_edit_ << "変換は中断されました。" << DetailLogEdit::PERIOD;

  abort_button_.SetEnabled( false );
}

void
ConvertingDialog::AtThreadEnd( void )
{
  this->SetText( "BMX2WAV - 変換終了" );

  this->GetSystemMenu().GetItemById( SC_CLOSE ).SetEnabled( true );

  is_converting_ = false;

  if ( auto_close_ ) {
    this->PostMessage( WM_CLOSE );
  }
}
