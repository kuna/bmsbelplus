#include "tt_util.h"
#include "tt_file_dialog.h"
#include "tt_folder_browser_dialog.h"
#include "tt_path.h"
#include "tt_directory.h"
#include "tt_message_box.h"

#include "bms_parser.h"

#include "bmx2wav_config_property_sheet.h"
#include "bmx2wav_simple_config_dialog.h"

#include "bmx2wav_batch_dialog.h"

using namespace Bmx2Wav;


// -- BatchEntry ---------------------------------------------------------
BatchEntry::BatchEntry( void ) :
is_separator_( false ),
input_filename_(),
output_filename_(),
bms_title_(),
bms_artist_(),
bms_genre_(),
is_done_( false )
{
}


// -- BatchBmsList -------------------------------------------------------
BatchBmsList::BatchBmsList( BatchDialog& dialog ) :
dialog_( dialog )
{
}

LRESULT
BatchBmsList::WindowProcedure( UINT msg, WPARAM w_param, LPARAM l_param )
{
  if ( msg == WM_KEYDOWN && w_param == VK_DELETE ) {
    dialog_.WMCommandReceived( BatchDialog::ID_BUTTON_DELETE, 0, handle_ );
  }
  return this->TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS>::WindowProcedure( msg, w_param, l_param );
}


// -- BatchLogDialog -----------------------------------------------------
BatchLogDialog::BatchLogDialog( void )
{
}

void
BatchLogDialog::operator <<( Period period )
{
  NOT_USE_VAR( period );
  this->operator <<( "\r\n" );
}


DWORD
BatchLogDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
BatchLogDialog::Created( void )
{
  this->SetText( "バッチ処理ログ" );

  {
    TtWindow::CreateParameter parameter( this );
    log_edit_.Create( parameter );
  }

  log_edit_.SendMessage( EM_LIMITTEXT, 1024 * 1024 );
  log_edit_.SetReadOnly( true );

  log_edit_.Show();

  return true;
}

LRESULT
BatchLogDialog::WMCloseReceived( void )
{
  this->Hide();
  return 0;
}

LRESULT
BatchLogDialog::WMSizeReceived( int flag, int width, int height )
{
  switch ( flag ) {
  case SIZE_RESTORED:
  case SIZE_MAXIMIZED:
    log_edit_.SetPositionSize( 0, 0, width, height );
  }
  return this->TtDialogModeless::WMSizeReceived( flag, width, height );
}

// -- BatchLogger --------------------------------------------------------
BatchLogger::BatchLogger( const std::string& filename ) :
TtLoggerStandard( filename )
{
}

void
BatchLogger::AddTimeToBuffer( void )
{
  std::string buf;
  this->TtLoggerStandard::Prefix( buf );
  this->operator <<( buf );
}

void
BatchLogger::Put( const std::string& value )
{
  if ( file_stream_ == NULL ) {
    return;
  }
  this->TtLoggerStandard::Put( value );
}

void
BatchLogger::Prefix( std::string& str )
{
  NOT_USE_VAR( str );
}


// -- BatchDialog --------------------------------------------------------
BatchDialog::BatchDialog( Converter& converter ) :
converter_( converter ),
bms_list_( *this )
{
}


BatchDialog::~BatchDialog()
{
}

DWORD
BatchDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW | WS_EX_ACCEPTFILES | WS_EX_APPWINDOW;
}


bool
BatchDialog::Created( void )
{
  this->SetText( "BMX2WAV - バッチ処理" );

  {
    TtWindow::CreateParameter parameter( this );
    no_message_box_check_.Create( parameter.AlterId( ID_CHECK_NO_MESSAGE_BOX ) );
    force_convert_check_.Create( parameter );
    log_file_check_.Create( parameter.AlterId( ID_CHECK_LOG_FILE ) );
    log_file_path_edit_.Create( parameter );
    log_file_ref_button_.Create( parameter.AlterId( ID_BUTTON_LOG_FILE_REF ) );
    execute_button_.Create( parameter.AlterId( ID_BUTTON_EXECUTE ) );
    bms_list_.Create( parameter.AlterId( ID_LISTVIEW ) );
    up_button_.Create( parameter.AlterId( ID_BUTTON_UP ) );
    down_button_.Create( parameter.AlterId( ID_BUTTON_DOWN ) );
    delete_button_.Create( parameter.AlterId( ID_BUTTON_DELETE ) );
    add_button_.Create( parameter.AlterId( ID_BUTTON_ADD ) );
  }

  // これを実行すると WindowProcedure が呼ばれるので先頭で処理できない
  ::SetMenu( handle_, ::LoadMenu( ::GetModuleHandle( NULL ), "BATCH_MENU" ) );

  this->SetSize( 550, 340 );

  bms_list_.OverrideWindowProcedure();
  bms_list_.SetHasGridLines( true );
  bms_list_.SetFullRowSelect( true );

  no_message_box_check_.SetText( "メッセージボックスを出さない設定（安全側）にする(&S)" );
  force_convert_check_.SetText( "上記の設定を強行側（無視して変換など）にする(&F)" );
  log_file_check_.SetText( "ログを以下のファイルに出力する(&L)" );
  log_file_ref_button_.SetText( "参照 ..." );
  execute_button_.SetText( "変換実行(&E)" );
  up_button_.SetText( "上へ(&U)" );
  down_button_.SetText( "下へ(&D)" );
  delete_button_.SetText( "削除(&M)" );
  add_button_.SetText( "追加(&A)" );

  no_message_box_check_.Show();
  force_convert_check_.Show();
  log_file_check_.Show();
  log_file_path_edit_.Show();
  log_file_ref_button_.Show();
  execute_button_.Show();
  bms_list_.Show();
  up_button_.Show();
  down_button_.Show();
  delete_button_.Show();
  add_button_.Show();

  force_convert_check_.SetEnabled( no_message_box_check_.GetCheck() );
  log_file_path_edit_.SetEnabled( log_file_check_.GetCheck() );
  log_file_ref_button_.SetEnabled( log_file_check_.GetCheck() );

  this->SetCenterRelativeToParent();

  {
    TtListViewColumn no_column = bms_list_.MakeNewColumn();
    TtListViewColumn input_column = bms_list_.MakeNewColumn();
    TtListViewColumn output_column = bms_list_.MakeNewColumn();
    TtListViewColumn title_column = bms_list_.MakeNewColumn();
    TtListViewColumn artist_column = bms_list_.MakeNewColumn();
    TtListViewColumn genre_column = bms_list_.MakeNewColumn();

    no_column.SetText( "No." );
    input_column.SetText( "入力ファイル" );
    output_column.SetText( "出力ファイル" );
    title_column.SetText( "TITLE" );
    artist_column.SetText( "ARTIST" );
    genre_column.SetText( "GENRE" );

    no_column.SetWidth(      35 );
    input_column.SetWidth(  120 );
    output_column.SetWidth( 120 );
    title_column.SetWidth(   60 );
    artist_column.SetWidth(  60 );
    genre_column.SetWidth(   55 );
  }
  {
    log_dialog_.ShowDialog( *this );
  }
  return true;
}

void
BatchDialog::EndDialog( int result )
{
  this->TtDialog::EndDialog( result );
  log_dialog_.EndDialog( result );
}


LRESULT
BatchDialog::WindowProcedure( UINT msg, WPARAM w_param, LPARAM l_param )
{
  switch ( msg ) {
  case WM_SIZING: {
    const int min_width = 400;
    const int min_height = 268;
    RECT* rect = reinterpret_cast<RECT*>( l_param );
    if ( rect->right - rect->left < min_width ) {
      switch ( w_param ) {
      case WMSZ_RIGHT:
      case WMSZ_TOPRIGHT:
      case WMSZ_BOTTOMRIGHT:
        rect->right = rect->left + min_width;
        break;

      case WMSZ_LEFT:
      case WMSZ_TOPLEFT:
      case WMSZ_BOTTOMLEFT:
        rect->left = rect->right - min_width;
        break;
      };
    }
    if ( rect->bottom - rect->top < min_height ) {
      switch ( w_param ) {
      case WMSZ_TOP:
      case WMSZ_TOPLEFT:
      case WMSZ_TOPRIGHT:
        rect->top = rect->bottom - min_height;
        break;

      case WMSZ_BOTTOM:
      case WMSZ_BOTTOMLEFT:
      case WMSZ_BOTTOMRIGHT:
        rect->bottom = rect->top + min_height;
        break;
      }
    }
    return 0;
  }
  }
  return this->TtDialog::WindowProcedure( msg, w_param, l_param );
}


LRESULT
BatchDialog::WMSizeReceived( int flag, int width, int height )
{
  switch ( flag ) {
  case SIZE_RESTORED:
  case SIZE_MAXIMIZED:
    no_message_box_check_.SetPositionSize(          8,            8,         290,            16 );
    force_convert_check_.SetPositionSize(          16,           28,         290,            16 );
    log_file_check_.SetPositionSize(                8,           48,         240,            16 );
    log_file_path_edit_.SetPositionSize(           16,           68, width -  96,            20 );
    log_file_ref_button_.SetPositionSize(  width - 72,           68,          48,            20 );
    execute_button_.SetPositionSize(       width - 92,            8,          84,            48 );
    bms_list_.SetPositionSize(                      8,           96, width -  88,  height - 104 );
    up_button_.SetPositionSize(            width - 72,           98,          64,            24 );
    down_button_.SetPositionSize(          width - 72,          128,          64,            24 );
    delete_button_.SetPositionSize(        width - 72,          160,          64,            24 );
    add_button_.SetPositionSize(           width - 72,          192,          64,            24 );
  }
  return this->TtDialog::WMSizeReceived( flag, width, height );
}

LRESULT
BatchDialog::WMDropFilesReceived( HDROP drop )
{
  this->SetForground();

  unsigned int count = DragQueryFile( drop, 0xFFFFFFFF, NULL, NULL );
  for ( unsigned int i = 0; i < count; ++i ) {
    TtTemporalyString buf( 1024 );
    DragQueryFile( drop, i, buf.GetPtr(), buf.GetSize() );
    if ( TtPath::IsDirectory( buf.GetPtr() ) ) {
      TtMessageBoxYesNoCancel box;
      ( box.AppendMessage() << "以下のディレクトリを開きます。\r\n\r\n" <<
        buf.GetPtr() << "\r\n\r\n" <<
        "再帰的にサブフォルダも開きますか？" );
      box.SetCaption( "再帰的に開くのか確認" );
      box.SetIcon( TtMessageBox::Icon::QUESTION );
      switch ( box.ShowDialog( *this ) ) {
      case TtMessageBox::Result::YES:
        this->OpenDirectory( buf.GetPtr(), true );
        break;

      case TtMessageBox::Result::NO:
        this->OpenDirectory( buf.GetPtr(), false );
        break;

      case TtMessageBox::Result::CANCEL:
      default:
        break;
      }
    }
    else {
      this->AddEntry( this->MakeEntry( buf.GetPtr() ) );
    }
  }
  DragFinish( drop );

  return 0;
}



LRESULT
BatchDialog::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_MENU_SIMPLE_CONFIG: {
    SimpleConfigDialog dx( converter_.GetParameterSet() );
    dx.ShowDialog( *this );
    return 0;
  }

  case ID_MENU_CONFIG: {
    ConfigPropertySheet sheet( converter_.GetParameterSet() );
    sheet.ShowDialog( *this );
    return 0;
  }

  case ID_MENU_SHOW_LOG_DIALOG: {
    log_dialog_.SetPosition( this->GetRectangle().left, this->GetRectangle().bottom );
    log_dialog_.SetSize( this->GetWidth(), 200 );
    log_dialog_.Show();
    return 0;
  }

  case ID_MENU_OPEN_DIR: {
    this->ReadDirectory( false );
    return 0;
  }
  case ID_MENU_OPEN_DIR_RECURSIVE: {
    this->ReadDirectory( true );
    return 0;
  }

  case ID_MENU_DELETE_DONE_MENU: {
    std::vector<BatchEntry>::iterator it = entries_.begin();
    while ( it != entries_.end() ) {
      if ( it->is_done_ ) {
        it = entries_.erase( it );
        this->RepaintList();
        continue;
      }
      ++it;
    }
    this->RepaintList();
    return 0;
  }

  case ID_MENU_ADD_SEPARATOR: {
    this->AddSeparator();
    return 0;
  }

  case ID_CHECK_NO_MESSAGE_BOX : {
    force_convert_check_.SetEnabled( no_message_box_check_.GetCheck() );
    return 0;
  }

  case ID_CHECK_LOG_FILE : {
    log_file_path_edit_.SetEnabled( log_file_check_.GetCheck() );
    log_file_ref_button_.SetEnabled( log_file_check_.GetCheck() );
    return 0;
  }

  case ID_BUTTON_LOG_FILE_REF: {
    TtSaveFileDialog dx;
    dx.SetFileName( log_file_path_edit_.GetText() );
    dx.GetFilters().push_back( TtFileDialogFilter( "全てのファイル(*.*)", "*" ) );
    if ( dx.ShowDialog( *this ) ) {
      log_file_path_edit_.SetText( dx.GetFileName() );
    }
    return 0;
  }

  case ID_BUTTON_EXECUTE: {
    this->ExecuteBatch();
    return 0;
  }

  case ID_BUTTON_UP: {
    int index  = bms_list_.GetSelectedIndices()[0];
    if ( index > 0 && index < static_cast<int>( entries_.size() ) ) {
      BatchEntry tmp = entries_[index];
      entries_.erase( entries_.begin() + index );
      entries_.insert( entries_.begin() + index - 1, tmp );
      this->RepaintList();
      bms_list_.SetFocus();
      bms_list_.GetItem( index - 1 ).SetSelected( true );
    }
    return 0;
  }
  case ID_BUTTON_DOWN: {
    int index  = bms_list_.GetSelectedIndices()[0];
    if ( index >= 0 && index < static_cast<int>( entries_.size() ) - 1 ) {
      BatchEntry tmp = entries_[index];
      entries_.erase( entries_.begin() + index );
      entries_.insert( entries_.begin() + index + 1, tmp );
      this->RepaintList();
      bms_list_.SetFocus();
      bms_list_.GetItem( index + 1 ).SetSelected( true );
    }
    return 0;
  }
  case ID_BUTTON_DELETE: {
    if ( bms_list_.GetSelectedCount() == 0 ) {
      return 0;
    }
    int index = bms_list_.GetSelectedIndices()[0];
    entries_.erase( entries_.begin() + index );
    bms_list_.GetSelectedItems()[0].Remove();
    bms_list_.SetFocus();
    int select_pos = static_cast<unsigned int>( index ) >= entries_.size() ? static_cast<int>( entries_.size() ) - 1 : index;
    if ( select_pos >= 0 ) {
      bms_list_.GetItem( select_pos ).SetSelected( true );
    }
    return 0;
  }
  case ID_BUTTON_ADD: {
    TtOpenFileDialog dx;
    dx.GetFilters().push_back( TtFileDialogFilter( "BMS ファイル(*.bms;*.bme;*.bml;*.pms)", "bms" ) );
    dx.GetFilters().back().GetExtensions().push_back( "bme" );
    dx.GetFilters().back().GetExtensions().push_back( "bml" );
    dx.GetFilters().back().GetExtensions().push_back( "pms" );
    dx.GetFilters().push_back( TtFileDialogFilter( "全てのファイル(*.*)", "*" ) );
    if ( dx.ShowDialog( *this ) ) {
      this->AddEntry( this->MakeEntry( dx.GetFileName() ) );
    }
    return 0;
  }

  }
  return this->TtDialog::WMCommandReceived( id, code, child );
}


void
BatchDialog::RepaintList( void )
{
  bms_list_.LockDraw();
  bms_list_.Clear();
  unsigned int count = 0;
  for ( std::vector<BatchEntry>::const_iterator it = entries_.begin(); it != entries_.end(); ++it ) {
    count += 1;
    TtListViewItem item = bms_list_.MakeNewItem();
    item.SetText( TtUtil::ToStringFrom( count ) + ( it->is_done_ ? " 済" : "" ) );
    item.SetSubItemText( 1, it->input_filename_ );
    item.SetSubItemText( 2, it->output_filename_ );
    item.SetSubItemText( 3, it->bms_title_ );
    item.SetSubItemText( 4, it->bms_artist_ );
    item.SetSubItemText( 5, it->bms_genre_ );
  }
  bms_list_.UnlockDraw();
}


void
BatchDialog::AddEntry( const BatchEntry& entry )
{
  bms_list_.LockDraw();
  entries_.push_back( entry );
  TtListViewItem item = bms_list_.MakeNewItem();
  item.SetText( TtUtil::ToStringFrom( entries_.size() ) + ( entry.is_done_ ? " 済" : "" ) );
  item.SetSubItemText( 1, entry.input_filename_ );
  item.SetSubItemText( 2, entry.output_filename_ );
  item.SetSubItemText( 3, entry.bms_title_ );
  item.SetSubItemText( 4, entry.bms_artist_ );
  item.SetSubItemText( 5, entry.bms_genre_ );
  bms_list_.UnlockDraw();
  bms_list_.SendMessage( WM_PAINT );
}

void
BatchDialog::AddSeparator( void )
{
  if ( entries_.size() != 0 ) {
    BatchEntry entry;
    entry.is_separator_ = true;
    entry.input_filename_  = "----------";
    entry.output_filename_ = "----------";
    entry.bms_title_       = "-----";
    entry.bms_artist_      = "-----";
    entry.bms_genre_       = "-----";
    this->AddEntry( entry );
  }
}


static bool
read_directory_filter( const std::string& path )
{
  if ( path.size() < 4 ) {
    return false;
  }
  bool s_flag = false;
  std::string::const_reverse_iterator s_it = path.rbegin();

  if ( *s_it == 's' ) {
    s_flag = true;
  }
  else if ( *s_it != 'e' && *s_it != 'l' ) {
    return false;
  }
  ++s_it;

  if ( *s_it != 'm' ) {
    return false;
  }
  ++s_it;

  if ( *s_it != 'b' && ( *s_it != 'p' || NOT( s_flag ) ) ) {
    return false;
  }
  ++s_it;

  if ( *s_it != '.' ) {
    return false;
  }
  return true;
}

void
BatchDialog::ReadDirectory( bool is_recursive )
{
  TtFolderBrowserDialog dx;
  if ( dx.ShowDialog( *this ) ) {
    this->OpenDirectory( dx.GetSelectedPath(), is_recursive );
  }
}


void
BatchDialog::OpenDirectory( const std::string& path, bool is_recursive )
{
  TtDirectory dir( path );
  if ( is_recursive ) {
    TtMessageBoxYesNo box;
    ( box.AppendMessage() << "以下のディレクトリから再帰的に開きます。\r\n\r\n" <<
      dir.GetPath() << "\r\n\r\n" <<
      "場合によっては開くのに時間がかかりますが、続行しますか？" );
    box.SetCaption( "ディレクトリを開くの確認" );
    box.SetIcon( TtMessageBox::Icon::QUESTION );
    if ( box.ShowDialog( *this ) != TtMessageBox::Result::YES ) {
      return;
    }
  }
  std::vector<std::string> list = is_recursive ?
    dir.GetEntriesRecursive( "*", read_directory_filter ) : dir.GetEntries( "*", read_directory_filter );
  std::string prev_dir = "";
  for ( std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it ) {
    if ( prev_dir != TtPath::DirName( *it ) ) {
      this->AddSeparator();
      prev_dir = TtPath::DirName( *it );
    }
    this->AddEntry( this->MakeEntry( *it ) );
  }
  TtMessageBoxOk box;
  box.AppendMessage() << list.size() << " 個のファイルを開きました。";
  box.SetCaption( "開いたファイルの数の報告" );
  box.SetIcon( TtMessageBox::Icon::INFORMATION );
  box.ShowDialog( *this );
}



BatchEntry
BatchDialog::MakeEntry( const std::string& filename )
{
  BatchEntry entry;
  entry.input_filename_ = filename;
  entry.output_filename_ = TtPath::ChangeExtension( filename, "wav" );

  BmsBms bms;
  BmsRandom::RootStatement root_statement;
  BmsParser::StartInfo info( bms, root_statement, BmsParser::Reactor::DefaultObject );
  info.make_syntax_tree_ = false;

  // BGA チャンネル無視
  info.ignore_channel_set_.insert( BmsWord( 4 ) );
  info.ignore_channel_set_.insert( BmsWord( 6 ) );
  info.ignore_channel_set_.insert( BmsWord( 7 ) );
  info.ignore_channel_set_.insert( BmsWord( 10 ) );
  info.ignore_channel_set_.insert( BmsWord( 365 ) );

  try {
    BmsParser::Parse( filename, info );
    if ( bms.GetHeaders().IsExists( "TITLE" ) ) {
      entry.bms_title_ = bms.GetHeaders()["TITLE"];
    }
    if ( bms.GetHeaders().IsExists( "ARTIST" ) ) {
      entry.bms_artist_ = bms.GetHeaders()["ARTIST"];
    }
    if ( bms.GetHeaders().IsExists( "GENRE" ) ) {
      entry.bms_genre_ = bms.GetHeaders()["GENRE"];
    }
  }
  catch ( BmsFileOpenException& e ) {
    entry.bms_title_ = "取得失敗";
    entry.bms_artist_ = "取得失敗";
    entry.bms_genre_ = "取得失敗";
  }
  catch ( BmsParseException& e ) {
    entry.bms_title_ = "取得失敗";
    entry.bms_artist_ = "取得失敗";
    entry.bms_genre_ = "取得失敗";
  }
  return entry;
}


void
BatchDialog::ExecuteBatch( void )
{
  {
    TtMessageBoxYesNo box;
    box.SetMessage( "一括変換処理を実施します。よろしいですか？" );
    box.SetCaption( "一括変換処理実行確認" );
    box.SetIcon( TtMessageBox::Icon::QUESTION );
    if ( box.ShowDialog( *this ) != TtMessageBox::Result::YES ) {
      return;
    }
  }
  class ParameterSetRecoverer {
  public:
    explicit ParameterSetRecoverer( ParameterSet& original ) :
    original_( original ), save_( original ) {}

    ~ParameterSetRecoverer() {
      original_ = save_;
    }
  private:
    ParameterSet& original_;
    ParameterSet  save_;
  } recoverer( converter_.GetParameterSet() );

  if ( no_message_box_check_.GetCheck() ) {
    using namespace Parameter;
    ParameterSet& tmp( converter_.GetParameterSet() );
    if ( force_convert_check_.GetCheck() ) {
      tmp.different_long_note_enclosed_object_is_error_ = false;
      tmp.parse_duplicate_header_reaction_       = ParseDuplicateHeaderReaction::OVERWRITE;
      tmp.parse_duplicate_bar_change_reaction_   = ParseDuplicateBarChangeReaction::OVERWRITE;
      tmp.parse_error_reaction_                  = ParseErrorReaction::IGNORE;
      tmp.wav_file_not_found_reaction_           = WavFileNotFoundReaction::IGNORE;
      tmp.wav_format_error_reaction_             = WavFormatErrorReaction::IGNORE;
      tmp.wav_file_error_reaction_               = WavFileErrorReaction::IGNORE;
      tmp.scroll_object_define_error_reaction_   = ScrollObjectDefineErrorReaction::IGNORE;
      tmp.scroll_object_entry_is_empty_reaction_ = ScrollObjectEntryIsEmptyReaction::IGNORE;
      tmp.check_wav_file_exists_   = false;
      tmp.check_bmp_file_exists_   = false;
      tmp.check_stage_file_exists_ = false;
      tmp.check_wav_used_          = false;
      tmp.check_bmp_used_          = false;
      tmp.check_bpm_used_          = false;
      tmp.check_stop_used_         = false;
    }
    else {
      tmp.different_long_note_enclosed_object_is_error_ = true;
      tmp.parse_duplicate_header_reaction_       = ParseDuplicateHeaderReaction::ERROR;
      tmp.parse_duplicate_bar_change_reaction_   = ParseDuplicateBarChangeReaction::ERROR;
      tmp.parse_error_reaction_                  = ParseErrorReaction::ERROR;
      tmp.wav_file_not_found_reaction_           = WavFileNotFoundReaction::ERROR;
      tmp.wav_format_error_reaction_             = WavFormatErrorReaction::ERROR;
      tmp.wav_file_error_reaction_               = WavFileErrorReaction::ERROR;
      tmp.scroll_object_define_error_reaction_   = ScrollObjectDefineErrorReaction::ERROR;
      tmp.scroll_object_entry_is_empty_reaction_ = ScrollObjectEntryIsEmptyReaction::ERROR;
      tmp.check_wav_file_exists_   = false;
      tmp.check_bmp_file_exists_   = false;
      tmp.check_stage_file_exists_ = false;
      tmp.check_wav_used_          = false;
      tmp.check_bmp_used_          = false;
      tmp.check_bpm_used_          = false;
      tmp.check_stop_used_         = false;
    }
  }

  BatchLogger logger( log_file_path_edit_.GetText() );
  if ( log_file_check_.GetCheck() ) {
    try {
      logger.Open();
    }
    catch ( TtFileOpenException& e ) {
      TtMessageBoxYesNo box;
      ( box.AppendMessage() << "ログファイルを開こうとして失敗しました。\r\n\r\n" <<
        "File : " << e.GetFileName() << "\r\n" <<
        "Message : " << strerror( e.GetErrorNumber() ) << "\r\n\r\n" <<
        "ログを出力せずにバッチ処理を続行しますか？" );
      box.SetCaption( "ログファイルオープン失敗" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      if ( box.ShowDialog( *this ) != TtMessageBox::Result::YES ) {
        return;
      }
    }
  }

  this->WMCommandReceived( ID_MENU_SHOW_LOG_DIALOG, 0, 0 );
  unsigned int try_count = 0;
  unsigned int done_count = 0;
  unsigned int success_count = 0;
  for ( std::vector<BatchEntry>::iterator it = entries_.begin(); it != entries_.end(); ++it ) {
    if ( NOT( it->is_separator_ ) ) {
      ++try_count;
    }
  }
  for ( std::vector<BatchEntry>::iterator it = entries_.begin(); it != entries_.end(); ++it ) {
    if ( it->is_separator_ ) {
      continue;
    }
    converter_.GetParameterSet().input_filename_ = it->input_filename_;
    converter_.GetParameterSet().output_filename_ = it->output_filename_;
    {
      log_dialog_ << "-- " << (done_count + 1) << " / " << try_count << " ----------------------------------------" << BatchLogDialog::PERIOD;
      log_dialog_ << "BMS File : " << converter_.GetParameterSet().input_filename_ << BatchLogDialog::PERIOD;
      log_dialog_ << it->bms_title_ << " / " << it->bms_artist_ << BatchLogDialog::PERIOD;
      log_dialog_ << "converting ... ";

      logger << "\n-- " << (done_count + 1) << " / " << try_count << " ----------------------------------------" << TtLogger::FLUSH;
      logger.AddTimeToBuffer();
      logger << " start." << TtLogger::FLUSH;
    }
    ConvertingDialog dx( converter_ );
    dx.SetAutoClose( true );
    if ( dx.ShowDialog( *this ) ) {
      it->is_done_ = true;
      ++success_count;
      log_dialog_ << "done.";
    }
    else {
      log_dialog_ << "error or abort.";
    }
    log_dialog_ << BatchLogDialog::PERIOD;
    logger.Put( dx.GetDetailLogText() );
    logger.AddTimeToBuffer();
    logger << " done." << TtLogger::FLUSH;
    this->RepaintList();
    ++done_count;
  }
  TtMessageBoxOk box;
  box.AppendMessage() << try_count << " 件中 " << success_count << " 件のファイルの変換に成功しました。";
  box.SetCaption( "一括変換完了" );
  box.SetIcon( TtMessageBox::Icon::INFORMATION );
  {
    log_dialog_ << "--------------------------------------------------" << BatchLogDialog::PERIOD;
    log_dialog_ << box.GetMessage() << BatchLogDialog::PERIOD;

    logger << try_count << " 件中 " << success_count << " 件のファイルの変換に成功しました。" << TtLogger::FLUSH;
  }
  box.ShowDialog( *this );
  this->RepaintList();
}
