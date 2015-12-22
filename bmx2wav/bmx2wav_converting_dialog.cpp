#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_message_box.h"
#include "tt_util.h"

#include "bms_exception.h"

#include "bmx2wav_exception.h"

#include "bmx2wav_converting_dialog.h"

using namespace Bmx2Wav;


// -- ConvertingDialog::Thread -------------------------------------------
ConvertingDialog::Thread::Thread( ConvertingDialog& dialog, Converter& converter ) :
dialog_( dialog ),
converter_( converter )
{
}

unsigned int
ConvertingDialog::Thread::Run( void )
{
  try {
    converter_.Convert();
  }
  catch ( TtFileOpenException& e ) {
    TtMessageBoxOk box;
    ( box.AppendMessage() << "ファイルを開くのに失敗しました。\r\n" <<
      "ファイル名 : " << e.GetFileName() << "\r\n" <<
      "メッセージ : " << TtUtil::GetWindowsSystemErrorMessage( e.GetErrorNumber() ) );
    box.SetCaption( "ファイルオープンエラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    dialog_.AtConvertError( box );
  }
  catch ( BmsParseException& e ) {
    TtMessageBoxOk box;
    ( box.AppendMessage() << "BMS ファイル解析中にエラーが発生しました。\r\n" <<
      "行番号 : " << e.GetLine() << "\r\n" <<
      "メッセージ : " << e.Message() );
    box.SetCaption( "BMS ファイル解析エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    dialog_.AtConvertError( box );
  }
  catch ( BmsException& e ) {
    TtMessageBoxOk box;
    ( box.AppendMessage() << "BMS のエラーが発生しました。\r\n\r\n" <<
      "メッセージ : " << e.Message() );
    box.SetCaption( "BMS エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    dialog_.AtConvertError( box );
  }
  catch ( ConvertAbortException& e ) {
    TtMessageBoxOk box;
    box.SetMessage( e.Message() );
    box.SetIcon( TtMessageBox::Icon::INFORMATION );
    box.SetCaption( "変換中断" );
    dialog_.AtConvertAbort( box );
  }
  catch ( Bmx2WavConvertException& e ) {
    TtMessageBoxOk box;
    box.AppendMessage() << "変換中にエラーが発生しました。\r\n" << e.Message();
    box.SetCaption( "変換エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    dialog_.AtConvertError( box );
  }
  catch ( Bmx2WavInternalException& e ) {
    TtMessageBoxOk box;
    box.AppendMessage() << "内部エラー\r\n" << e.Message();
    box.SetCaption( "内部エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    dialog_.AtConvertError( box );
  }
  catch ( TtException& e ) {
    TtMessageBoxOk box;
    box.AppendMessage() << "エラーが発生しました。\r\n" << "DUMP : " << e.Dump();
    box.SetCaption( "エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    dialog_.AtConvertError( box );
  }
  catch ( std::bad_alloc& e ) {
    TtMessageBoxOk box;
    box.AppendMessage() << "メモリ不足が発生しました。\r\n" << "メッセージ : " << e.what();
    box.SetCaption( "メモリエラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    dialog_.AtConvertError( box );
  }
  catch ( ... ) {
    TtMessageBoxOk box;
    box.SetMessage( "予期せぬ例外が発生しました。" );
    box.SetCaption( "エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    dialog_.AtConvertError( box );
  }

  dialog_.AtThreadEnd();
  return 0;
}


// -- ConvertingDialog ---------------------------------------------------
ConvertingDialog::ConvertingDialog( Converter& converter ) :
wav_read_progress_(),
wav_mix_progress_(),

converter_( converter ),
thread_( *this, converter ),
abort_message_queue_(),

mix_end_position_( 0 ),

is_converting_( false ),
is_detail_log_showing_( false ),
is_success_( false ),
auto_close_( false ),

outputed_filename_(),
detail_log_text_()
{
}

ConvertingDialog::~ConvertingDialog()
{
}

void
ConvertingDialog::SetAutoClose( bool flag )
{
  auto_close_ = flag;
}

bool
ConvertingDialog::GetAutoClose( void ) const
{
  return auto_close_;
}

std::string
ConvertingDialog::GetDetailLogText( void )
{
  return detail_log_text_;
}


TtThread&
ConvertingDialog::GetRunThread( void )
{
  return thread_;
}

bool
ConvertingDialog::Created( void )
{
  this->SetText( "BMX2WAV - 変換中" );
  this->SetIcon( ::LoadIcon( ::GetModuleHandle( NULL ), "MAIN_ICON" ), true );

  {
    TtWindow::CreateParameter parameter( this );
    bms_file_label_.Create( parameter );
    bms_title_artist_label_.Create( parameter );
    explanation_label_.Create( parameter );
    wav_read_label_.Create( parameter );
    wav_read_progress_.Create( parameter );
    wav_mix_label_.Create( parameter );
    wav_mix_progress_.Create( parameter );
    play_button_.Create( parameter.AlterId( ID_BUTTON_PLAY ) );
    abort_button_.Create( parameter.AlterId( ID_BUTTON_ABORT ) );
    show_detail_log_button_.Create( parameter.AlterId( ID_BUTTON_SHOW_DETAIL_LOG ) );
    detail_log_edit_.Create( parameter );
  }

  this->SetSize( 450, 264 );
  bms_file_label_.SetPositionSize(           4,   8, 436,  16 );
  bms_title_artist_label_.SetPositionSize(   4,  24, 436,  16 );
  explanation_label_.SetPositionSize(       24,  48, 436,  16 );
  wav_read_label_.SetPositionSize(           4,  76, 436,  16 );
  wav_read_progress_.SetPositionSize(        4,  96, 436,  30 );
  wav_mix_label_.SetPositionSize(            4, 136, 436,  16 );
  wav_mix_progress_.SetPositionSize(         4, 156, 436,  30 );
  play_button_.SetPositionSize(              4, 204, 140,  24 );
  abort_button_.SetPositionSize(           158, 204,  90,  24 );
  show_detail_log_button_.SetPositionSize( 300, 204, 140,  24 );
  detail_log_edit_.SetPositionSize(          4, 242, 436, 176 );

  play_button_.SetText( "出力ファイルを開く" );
  abort_button_.SetText( "変換を中断" );
  show_detail_log_button_.SetText( "詳細ログを表示 >>" );

  play_button_.SetEnabled( false );

  detail_log_edit_.SendMessage( EM_LIMITTEXT, 1024 * 1024 );
  detail_log_edit_.SetReadOnly( true );

  bms_file_label_.Show();
  bms_title_artist_label_.Show();
  explanation_label_.Show();
  wav_read_label_.Show();
  wav_read_progress_.Show();
  wav_mix_label_.Show();
  wav_mix_progress_.Show();
  play_button_.Show();
  abort_button_.Show();
  show_detail_log_button_.Show();
  detail_log_edit_.Show();

  this->SetCenterRelativeToParent();

  converter_.SetProgressReceiver( this );

  is_converting_ = true;
  thread_.Start();
  return true;
}

LRESULT
ConvertingDialog::WMCloseReceived( void )
{
  if ( is_converting_ ) {
    return 0;
  }
  this->GetRunThread().Join();
  detail_log_text_ = TtUtil::StringReplace( detail_log_edit_.GetText(), "\r", "" );
  this->EndDialog( is_success_ ? 1 : 0 );
  return 0;
}

LRESULT
ConvertingDialog::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_BUTTON_SHOW_DETAIL_LOG: {
    if ( is_detail_log_showing_ ) {
      is_detail_log_showing_ = false;
      this->SetSize( 450, 264 );
      show_detail_log_button_.SetText( "詳細ログを表示 >> " );
    }
    else {
      is_detail_log_showing_ = true;
      this->SetSize( 450, 450 );
      show_detail_log_button_.SetText( "詳細ログを隠す << " );
    }
    return 0;
  }

  case ID_BUTTON_PLAY: {
    int ret = reinterpret_cast<int>( ::ShellExecute(
      NULL, "open", outputed_filename_.c_str(), NULL, NULL, 0 ) );
    if ( ret <= 32 ) {
      DWORD error_code = ::GetLastError();
      TtMessageBoxOk box;
      ( box.AppendMessage() << "ファイルを開くのに失敗しました。\r\n\r\n" <<
        "ファイル名 : " << outputed_filename_ << "\r\n" <<
        "メッセージ : " << TtUtil::GetWindowsSystemErrorMessage( error_code ) );
      box.SetCaption( "ファイルオープンエラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
    return 0;
  }

  case ID_BUTTON_ABORT: {
    abort_message_queue_.PostMessage( 0 );
    return 0;
  }

  }
  return this->TtDialog::WMCommandReceived( id, code, child );
}

void
ConvertingDialog::AbortIfAbortMessageExistsInQueue( void )
{
  if ( abort_message_queue_.HasMessage() ) {
    throw ConvertAbortException();
  }
}

// -- ConvertingDialog::DetailLogEdit ------------------------------------
ConvertingDialog::DetailLogEdit::DetailLogEdit( void ) :
TtWindowWithStyle<TtEditMultiline, WS_VSCROLL>()
{
}


ConvertingDialog::DetailLogEdit&
ConvertingDialog::DetailLogEdit::operator <<( const std::string& str )
{
  int tmp = this->GetTextLength();
  this->SendMessage( EM_SETSEL, tmp, tmp );
  this->SendMessage( EM_REPLACESEL, 0, reinterpret_cast<WPARAM>( str.c_str() ) );
  return *this;
}

ConvertingDialog::DetailLogEdit&
ConvertingDialog::DetailLogEdit::operator <<( int value )
{
  return this->operator <<( TtUtil::ToStringFrom( value ) );
}

ConvertingDialog::DetailLogEdit&
ConvertingDialog::DetailLogEdit::operator <<( unsigned int value )
{
  return this->operator <<( TtUtil::ToStringFrom( value ) );
}

ConvertingDialog::DetailLogEdit&
ConvertingDialog::DetailLogEdit::operator <<( double value )
{
  return this->operator <<( TtUtil::ToStringFrom( value ) );
}

ConvertingDialog::DetailLogEdit&
ConvertingDialog::DetailLogEdit::operator <<( BmsWord& word )
{
  return this->operator <<( word.ToCharPtr() );
}

void
ConvertingDialog::DetailLogEdit::operator <<( Period period )
{
  NOT_USE_VAR( period );

  this->operator <<( "\r\n" );
}
