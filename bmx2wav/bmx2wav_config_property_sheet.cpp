#include "tt_window_util.h"
#include "tt_file_dialog.h"
#include "tt_message_box.h"
#include "tt_util.h"

#include "bmx2wav_config_property_sheet.h"

using namespace Bmx2Wav;

// -- BmsPage ------------------------------------------------------------
ConfigPropertySheet::BmsPage::BmsPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "BMS" ),
parameter_set_( parameter_set )
{
  this->PresetSizeAtPageCreate( 268, 220 );
}

bool
ConfigPropertySheet::BmsPage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    ignore_bga_channel_check_.Create( parameter );
    different_long_note_enclosed_object_is_error_check_.Create( parameter );
    parse_duplicate_header_reaction_label_.Create( parameter );
    parse_duplicate_header_reaction_combo_.Create( parameter );
    parse_duplicate_bar_change_reaction_label_.Create( parameter );
    parse_duplicate_bar_change_reaction_combo_.Create( parameter );
    parse_error_reaction_label_.Create( parameter );
    parse_error_reaction_combo_.Create( parameter );
  }

  ignore_bga_channel_check_.SetPositionSize(                             8,  16, 384,  16 );
  different_long_note_enclosed_object_is_error_check_.SetPositionSize(   8,  36, 384,  16 );
  parse_duplicate_header_reaction_label_.SetPositionSize(                8,  60, 280,  16 );
  parse_duplicate_header_reaction_combo_.SetPositionSize(               16,  80, 280,  80 );
  parse_duplicate_bar_change_reaction_label_.SetPositionSize(            8, 110, 280,  16 );
  parse_duplicate_bar_change_reaction_combo_.SetPositionSize(           16, 130, 280,  80 );
  parse_error_reaction_label_.SetPositionSize(                           8, 160, 280,  16 );
  parse_error_reaction_combo_.SetPositionSize(                          16, 180, 280,  60 );

  ignore_bga_channel_check_.SetText( "BGA チャンネルの行は読み込みを無視する(&I)" );
  different_long_note_enclosed_object_is_error_check_.SetText( "ロングノートの終端が始端と違う番号の場合、変換を中止する(&L)" );
  {
    using Parameter::ParseDuplicateHeaderReaction;
    parse_duplicate_header_reaction_label_.SetText( "同一ヘッダが重複した場合の動作(&O)" );
    ComboBoxForEnum<ParseDuplicateHeaderReaction>& tmp = parse_duplicate_header_reaction_combo_;
    tmp.SetAlias( ParseDuplicateHeaderReaction::DEFAULT, ParseDuplicateHeaderReaction::ASK );
    tmp.PushWithData( "後で指定されたのを有効にする", ParseDuplicateHeaderReaction::OVERWRITE );
    tmp.PushWithData( "先に指定されたのを有効にする", ParseDuplicateHeaderReaction::REMAIN );
    tmp.PushWithData( "読み込み行エラーにする",       ParseDuplicateHeaderReaction::ERROR );
    tmp.PushWithData( "これらを毎回ユーザに尋ねる",   ParseDuplicateHeaderReaction::ASK );
  }
  {
    using Parameter::ParseDuplicateBarChangeReaction;
    parse_duplicate_bar_change_reaction_label_.SetText( "小節長変更が重複した場合の動作(&P)" );
    ComboBoxForEnum<ParseDuplicateBarChangeReaction>& tmp = parse_duplicate_bar_change_reaction_combo_;
    tmp.SetAlias( ParseDuplicateBarChangeReaction::DEFAULT, ParseDuplicateBarChangeReaction::ASK );
    tmp.PushWithData( "後で指定されたのを有効にする", ParseDuplicateBarChangeReaction::OVERWRITE );
    tmp.PushWithData( "先に指定されたのを有効にする", ParseDuplicateBarChangeReaction::REMAIN );
    tmp.PushWithData( "読み込み行エラーにする",       ParseDuplicateBarChangeReaction::ERROR );
    tmp.PushWithData( "これらを毎回ユーザに尋ねる",   ParseDuplicateBarChangeReaction::ASK );
  }
  {
    using Parameter::ParseErrorReaction;
    parse_error_reaction_label_.SetText( "読み込み行エラーが発生した場合の動作(&J)" );
    ComboBoxForEnum<ParseErrorReaction>& tmp = parse_error_reaction_combo_;
    tmp.SetAlias( ParseErrorReaction::DEFAULT, ParseErrorReaction::ASK );
    tmp.PushWithData( "エラーとし変換を中止する",               ParseErrorReaction::ERROR );
    tmp.PushWithData( "続行するか中止するか毎回ユーザに尋ねる", ParseErrorReaction::ASK );
    tmp.PushWithData( "その行をなかったことにして続行する",     ParseErrorReaction::IGNORE );
  }

  ignore_bga_channel_check_.Show();
  different_long_note_enclosed_object_is_error_check_.Show();
  parse_duplicate_header_reaction_label_.Show();
  parse_duplicate_header_reaction_combo_.Show();
  parse_duplicate_bar_change_reaction_label_.Show();
  parse_duplicate_bar_change_reaction_combo_.Show();
  parse_error_reaction_label_.Show();
  parse_error_reaction_combo_.Show();

  ignore_bga_channel_check_.SetCheck( parameter_set_.ignore_bga_channel_ );
  different_long_note_enclosed_object_is_error_check_.SetCheck( parameter_set_.different_long_note_enclosed_object_is_error_ );
  parse_duplicate_header_reaction_combo_.SetSelectByItemData( parameter_set_.parse_duplicate_header_reaction_ );
  parse_duplicate_bar_change_reaction_combo_.SetSelectByItemData( parameter_set_.parse_duplicate_bar_change_reaction_ );
  parse_error_reaction_combo_.SetSelectByItemData( parameter_set_.parse_error_reaction_ );

  return true;
}


bool
ConfigPropertySheet::BmsPage::AtApply( void )
{
  parameter_set_.ignore_bga_channel_ = ignore_bga_channel_check_.GetCheck();
  parameter_set_.different_long_note_enclosed_object_is_error_ = different_long_note_enclosed_object_is_error_check_.GetCheck();
  parameter_set_.parse_duplicate_header_reaction_ = parse_duplicate_header_reaction_combo_.GetSelectedItemData();
  parameter_set_.parse_duplicate_bar_change_reaction_ = parse_duplicate_bar_change_reaction_combo_.GetSelectedItemData();
  parameter_set_.parse_error_reaction_ = parse_error_reaction_combo_.GetSelectedItemData();

  return true;
}


// -- ConvertPage --------------------------------------------------------
ConfigPropertySheet::ConvertPage::ConvertPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "変換" ),
parameter_set_( parameter_set )
{
}

bool
ConfigPropertySheet::ConvertPage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    cancel_at_resounding_check_.Create( parameter );
    not_sound_object_specified_by_lnobj_check_.Create( parameter );
    error_in_not_entried_wav_was_used_check_.Create( parameter );
    start_label_.Create( parameter );
    end_label_.Create( parameter );
    convert_start_end_bar_number_label_.Create( parameter );
    convert_start_bar_number_edit_.Create( parameter );
    convert_end_bar_number_edit_.Create( parameter );
    do_triming_check_.Create( parameter.AlterId( ID_CHECK_DO_TRIMING ) );
    triming_start_bar_number_edit_.Create( parameter );
    triming_end_bar_number_edit_.Create( parameter );
    scroll_object_define_error_reaction_label_.Create( parameter );
    scroll_object_define_error_reaction_combo_.Create( parameter );
    scroll_object_entry_is_empty_reaction_label_.Create( parameter );
    scroll_object_entry_is_empty_reaction_combo_.Create( parameter );
  }

  cancel_at_resounding_check_.SetPositionSize(                    8,  16, 384, 16 );
  not_sound_object_specified_by_lnobj_check_.SetPositionSize(     8,  36, 384, 16 );
  error_in_not_entried_wav_was_used_check_.SetPositionSize(       8,  56, 384, 16 );
  start_label_.SetPositionSize(                                 228,  84,  80, 16 );
  end_label_.SetPositionSize(                                   308,  84,  80, 16 );
  convert_start_end_bar_number_label_.SetPositionSize(           88, 114, 150, 16 );
  convert_start_bar_number_edit_.SetPositionSize(               240, 110,  40, 20 );
  convert_end_bar_number_edit_.SetPositionSize(                 320, 110,  40, 20 );
  do_triming_check_.SetPositionSize(                              8, 154, 200, 16 );
  triming_start_bar_number_edit_.SetPositionSize(               240, 150,  40, 20 );
  triming_end_bar_number_edit_.SetPositionSize(                 320, 150,  40, 20 );
  scroll_object_define_error_reaction_label_.SetPositionSize(     8, 188, 400, 20 );
  scroll_object_define_error_reaction_combo_.SetPositionSize(    16, 208, 360, 60 );
  scroll_object_entry_is_empty_reaction_label_.SetPositionSize(   8, 238, 400, 20 );
  scroll_object_entry_is_empty_reaction_combo_.SetPositionSize(  16, 258, 360, 60 );

  cancel_at_resounding_check_.SetText( "WAV が鳴り終わる前に同じ WAV が鳴った場合、前の WAV を消す(&D)" );
  not_sound_object_specified_by_lnobj_check_.SetText( "#LNOBJ で指定した WAV は鳴らさない(&O)" );
  error_in_not_entried_wav_was_used_check_.SetText( "登録されていない WAV のオブジェクトを使用した場合エラーにする(&F)" );
  start_label_.SetText( "開始小節番号" );
  end_label_.SetText( "終了小節番号" );
  convert_start_end_bar_number_label_.SetText( "WAV を演奏する範囲(&P)" );
  do_triming_check_.SetText( "出力する WAV の一部を切り出す(&C)" );
  {
    using Parameter::ScrollObjectDefineErrorReaction;
    scroll_object_define_error_reaction_label_.SetText( "#BPM?? や #STOP?? で指定した値が不正だった場合の動作(&G)" );
    ComboBoxForEnum<ScrollObjectDefineErrorReaction>& tmp = scroll_object_define_error_reaction_combo_;
    tmp.SetAlias( ScrollObjectDefineErrorReaction::DEFAULT, ScrollObjectDefineErrorReaction::ASK );
    tmp.PushWithData( "エラーとし変換を中止する",               ScrollObjectDefineErrorReaction::ERROR );
    tmp.PushWithData( "続行するか中止するか毎回ユーザに尋ねる", ScrollObjectDefineErrorReaction::ASK );
    tmp.PushWithData( "その指定がなかったことにして続行する",   ScrollObjectDefineErrorReaction::IGNORE );
  }
  {
    using Parameter::ScrollObjectEntryIsEmptyReaction;
    scroll_object_entry_is_empty_reaction_label_.SetText( "#BPM?? や #STOP?? の指定が無くそのオブジェクトを使用した場合の動作(&E)" );
    ComboBoxForEnum<ScrollObjectEntryIsEmptyReaction>& tmp = scroll_object_entry_is_empty_reaction_combo_;
    tmp.SetAlias( ScrollObjectEntryIsEmptyReaction::DEFAULT, ScrollObjectEntryIsEmptyReaction::ASK );
    tmp.PushWithData( "エラーとし変換を中止する",                     ScrollObjectEntryIsEmptyReaction::ERROR );
    tmp.PushWithData( "続行するか中止するか毎回ユーザに尋ねる",       ScrollObjectEntryIsEmptyReaction::ASK );
    tmp.PushWithData( "そのオブジェクトをなかったことにして続行する", ScrollObjectEntryIsEmptyReaction::IGNORE );
  }

  cancel_at_resounding_check_.Show();
  not_sound_object_specified_by_lnobj_check_.Show();
  error_in_not_entried_wav_was_used_check_.Show();
  start_label_.Show();
  end_label_.Show();
  convert_start_end_bar_number_label_.Show();
  convert_start_bar_number_edit_.Show();
  convert_end_bar_number_edit_.Show();
  do_triming_check_.Show();
  triming_start_bar_number_edit_.Show();
  triming_end_bar_number_edit_.Show();
  scroll_object_define_error_reaction_label_.Show();
  scroll_object_define_error_reaction_combo_.Show();
  scroll_object_entry_is_empty_reaction_label_.Show();
  scroll_object_entry_is_empty_reaction_combo_.Show();

  cancel_at_resounding_check_.SetCheck( parameter_set_.cancel_at_resounding_ );
  not_sound_object_specified_by_lnobj_check_.SetCheck( parameter_set_.not_sound_object_specified_by_lnobj_ );
  error_in_not_entried_wav_was_used_check_.SetCheck( parameter_set_.error_in_not_entried_wav_was_used_ );
  convert_start_bar_number_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.convert_start_bar_number_ ) );
  convert_end_bar_number_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.convert_end_bar_number_ ) );
  do_triming_check_.SetCheck( parameter_set_.do_triming_ );
  triming_start_bar_number_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.triming_start_bar_number_ ) );
  triming_end_bar_number_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.triming_end_bar_number_ ) );
  scroll_object_define_error_reaction_combo_.SetSelectByItemData( parameter_set_.scroll_object_define_error_reaction_ );
  scroll_object_entry_is_empty_reaction_combo_.SetSelectByItemData( parameter_set_.scroll_object_entry_is_empty_reaction_ );

  this->WMCommandReceived( ID_CHECK_DO_TRIMING, 0, 0 );

  return true;
}

bool
ConfigPropertySheet::ConvertPage::AtKillActive( void )
{
  TtMessageBoxOk box;
  box.SetCaption( "エラー" );
  box.SetIcon( TtMessageBox::Icon::ERROR );
  int start;
  int end;
  if ( NOT( TtUtil::StringToInteger( convert_start_bar_number_edit_.GetText(), &start, 10 ) ) ||
       ( start < 0 || start > 999 ) ) {
    box.SetMessage( "変換開始小節番号の値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  if ( NOT( TtUtil::StringToInteger( convert_end_bar_number_edit_.GetText(), &end, 10 ) ) ||
       ( end < 0 || end > 999 ) ) {
    box.SetMessage( "変換終了小節番号の値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  if ( start > end ) {
    box.SetMessage( "変換終了小節番号が変換終了小節番号よりも後になっています。" );
    box.ShowDialog( *this );
    return false;
  }
  if ( NOT( TtUtil::StringToInteger( triming_start_bar_number_edit_.GetText(), &start, 10 ) ) ||
       ( start < 0 || start > 999 ) ) {
    box.SetMessage( "WAV 開始小節番号の値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  if ( NOT( TtUtil::StringToInteger( triming_end_bar_number_edit_.GetText(), &end, 10 ) ) ||
       ( end < 0 || end > 999 ) ) {
    box.SetMessage( "WAV 終了小節番号の値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  if ( start > end ) {
    box.SetMessage( "WAV 終了小節番号が WAV 終了小節番号よりも後になっています。" );
    box.ShowDialog( *this );
    return false;
  }
  return true;
}

bool
ConfigPropertySheet::ConvertPage::AtApply( void )
{
  if ( NOT( this->AtKillActive() ) ) {
    return false;
  }

  parameter_set_.cancel_at_resounding_                  = cancel_at_resounding_check_.GetCheck();
  parameter_set_.not_sound_object_specified_by_lnobj_   = not_sound_object_specified_by_lnobj_check_.GetCheck();
  parameter_set_.error_in_not_entried_wav_was_used_     = error_in_not_entried_wav_was_used_check_.GetCheck();
  TtUtil::StringToInteger( convert_start_bar_number_edit_.GetText(), &static_cast<int>( parameter_set_.convert_start_bar_number_ ), 10 );
  TtUtil::StringToInteger( convert_end_bar_number_edit_.GetText(),   &static_cast<int>( parameter_set_.convert_end_bar_number_ ), 10 );
  parameter_set_.do_triming_                            = do_triming_check_.GetCheck();
  TtUtil::StringToInteger( triming_start_bar_number_edit_.GetText(), &static_cast<int>( parameter_set_.triming_start_bar_number_ ), 10 );
  TtUtil::StringToInteger( triming_end_bar_number_edit_.GetText(),   &static_cast<int>( parameter_set_.triming_end_bar_number_ ), 10 );
  parameter_set_.scroll_object_define_error_reaction_   = scroll_object_define_error_reaction_combo_.GetSelectedItemData();
  parameter_set_.scroll_object_entry_is_empty_reaction_ = scroll_object_entry_is_empty_reaction_combo_.GetSelectedItemData();
  return true;
}

LRESULT
ConfigPropertySheet::ConvertPage::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_CHECK_DO_TRIMING:
    triming_start_bar_number_edit_.SetEnabled( do_triming_check_.GetCheck() );
    triming_end_bar_number_edit_.SetEnabled( do_triming_check_.GetCheck() );
    return 0;
  }
  return this->TtForm::WMCommandReceived( id, code, child );
}

// -- VolumePage ---------------------------------------------------------
ConfigPropertySheet::VolumePage::VolumePage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "音量" ),
parameter_set_( parameter_set )
{
}

bool
ConfigPropertySheet::VolumePage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    normalize_kind_peak_radio_.Create( parameter.AlterId( ID_RADIO_PEAK ).AlterGroupStart( true ) );
    normalize_kind_average_radio_.Create( parameter.AlterId( ID_RADIO_AVERAGE ) );
    normalize_kind_none_radio_.Create( parameter.AlterId( ID_RADIO_NONE ) );
    normalize_kind_over_radio_.Create( parameter.AlterId( ID_RADIO_OVER ) );
    normalize_kind_group_.Create( parameter.AlterGroupStart( true ) );
    normalize_over_ppm_label_.Create( parameter );
    normalize_over_ppm_edit_.Create( parameter );
    volume_label_.Create( parameter );
    volume_edit_.Create( parameter );
  }

  normalize_kind_peak_radio_.SetPositionSize(     16,  28, 100,  16 );
  normalize_kind_average_radio_.SetPositionSize( 136,  28, 100,  16 );
  normalize_kind_none_radio_.SetPositionSize(    256,  28, 100,  16 );
  normalize_kind_over_radio_.SetPositionSize(     16,  56, 108,  16 );
  normalize_kind_group_.SetPositionSize(           8,   8, 384,  80 );
  normalize_over_ppm_label_.SetPositionSize(     128,  57, 114,  16 );
  normalize_over_ppm_edit_.SetPositionSize(      244,  53,  60,  20 );
  volume_label_.SetPositionSize(                   8, 100,  70,  16 );
  volume_edit_.SetPositionSize(                   72,  96,  40,  20 );

  normalize_kind_peak_radio_.SetText( "ピーク(&P)" );
  normalize_kind_average_radio_.SetText( "平均(&A)");
  normalize_kind_none_radio_.SetText( "やんない(&Y)" );
  normalize_kind_over_radio_.SetText( "微少標本廃棄(&O)" );
  normalize_kind_group_.SetText( "ノーマライズ" );
  normalize_over_ppm_label_.SetText( "廃棄標本率(ppm)(&N):" );
  volume_label_.SetText( "音量(%)(&V) :" );

  normalize_kind_peak_radio_.Show();
  normalize_kind_average_radio_.Show();
  normalize_kind_none_radio_.Show();
  normalize_kind_over_radio_.Show();
  normalize_kind_group_.Show();
  normalize_over_ppm_edit_.Show();
  normalize_over_ppm_label_.Show();
  volume_label_.Show();
  volume_edit_.Show();

  {
    using Parameter::Normalize;
    TtValueToRadioCheck< TtEnum<Normalize> > table( normalize_kind_over_radio_ );
    table.Regist( Normalize::PEAK,    normalize_kind_peak_radio_ );
    table.Regist( Normalize::AVERAGE, normalize_kind_average_radio_ );
    table.Regist( Normalize::NONE,    normalize_kind_none_radio_ );
    table.Regist( Normalize::OVER,    normalize_kind_over_radio_ );
    table.SetCheck( parameter_set_.normalize_kind_ );
  }
  normalize_over_ppm_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.normalize_over_ppm_ ) );
  normalize_over_ppm_label_.SetEnabled( normalize_kind_over_radio_.GetCheck() );
  normalize_over_ppm_edit_.SetEnabled( normalize_kind_over_radio_.GetCheck() );
  volume_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.volume_ ) );

  return true;
}

bool
ConfigPropertySheet::VolumePage::AtKillActive( void )
{
  int tmp;
  if ( NOT( TtUtil::StringToInteger( volume_edit_.GetText(), &tmp, 10 ) ) ) {
    TtMessageBoxOk box;
    box.SetMessage( "音量の値を正しく入力して下さい。" );
    box.SetCaption( "エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  };
  if ( NOT( TtUtil::StringToInteger( normalize_over_ppm_edit_.GetText(), &tmp, 10 ) ) ) {
    TtMessageBoxOk box;
    box.SetMessage( "自動超過ノーマライズの数値を正しく入力して下さい。" );
    box.SetCaption( "エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  };
  return true;
}

bool
ConfigPropertySheet::VolumePage::AtApply( void )
{
  if ( NOT( this->AtKillActive() ) ) {
    return false;
  }

  {
    using Parameter::Normalize;
    TtRadioCheckToValue< TtEnum<Normalize> > table( Normalize::DEFAULT );
    table.Regist( normalize_kind_peak_radio_,    Normalize::PEAK );
    table.Regist( normalize_kind_average_radio_, Normalize::AVERAGE );
    table.Regist( normalize_kind_none_radio_,    Normalize::NONE );
    table.Regist( normalize_kind_over_radio_,    Normalize::OVER );
    parameter_set_.normalize_kind_ = table.GetValue();
  }
  TtUtil::StringToInteger( volume_edit_.GetText(), &parameter_set_.volume_, 10 );
  TtUtil::StringToInteger( normalize_over_ppm_edit_.GetText(), &static_cast<int>( parameter_set_.normalize_over_ppm_ ), 10 );
  return true;
}

LRESULT
ConfigPropertySheet::VolumePage::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_RADIO_OVER:
  case ID_RADIO_PEAK:
  case ID_RADIO_AVERAGE:
  case ID_RADIO_NONE:
    normalize_over_ppm_label_.SetEnabled( normalize_kind_over_radio_.GetCheck() );
    normalize_over_ppm_edit_.SetEnabled( normalize_kind_over_radio_.GetCheck() );
    return 0;
  }
  return this->TtForm::WMCommandReceived( id, code, child );
}

// -- RandomPage ---------------------------------------------------------
ConfigPropertySheet::RandomPage::RandomPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "RANDOM" ),
parameter_set_( parameter_set )
{
}

bool
ConfigPropertySheet::RandomPage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    random_kind_every_select_radio_.Create( parameter.AlterId( ID_RADIO_EVERY_SELECT ).AlterGroupStart( true ) );
    random_kind_constant_radio_.Create( parameter.AlterId( ID_RADIO_CONSTANT ) );
    random_kind_real_random_radio_.Create( parameter.AlterId( ID_RADIO_REAL_RANDOM ) );
    random_kind_group_.Create( parameter.AlterGroupStart( true ) );
    random_constant_number_label_.Create( parameter );
    random_constant_number_edit_.Create( parameter );
  }

  random_kind_every_select_radio_.SetPositionSize(  18,  28, 180,  16 );
  random_kind_constant_radio_.SetPositionSize(      18,  56, 180,  16 );
  random_kind_real_random_radio_.SetPositionSize(   18, 104, 180,  16 );
  random_kind_group_.SetPositionSize(                8,   8, 232, 124 );
  random_constant_number_label_.SetPositionSize(    48,  80,  40,  20 );
  random_constant_number_edit_.SetPositionSize(     88,  76,  50,  20 );

  random_kind_every_select_radio_.SetText( "その都度自分で選ぶ(&S)" );
  random_kind_constant_radio_.SetText( "定数を発生させる(&C)" );
  random_kind_real_random_radio_.SetText( "本当に乱数を発生させる(&R)" );
  random_kind_group_.SetText( "ランダム値発生処理(#RANDOM)" );
  random_constant_number_label_.SetText( "値(&N) :" );

  random_kind_every_select_radio_.Show();
  random_kind_constant_radio_.Show();
  random_kind_real_random_radio_.Show();
  random_kind_group_.Show();
  random_constant_number_label_.Show();
  random_constant_number_edit_.Show();

  {
    using Parameter::Random;
    TtValueToRadioCheck< TtEnum<Random> > table( random_kind_every_select_radio_ );
    table.Regist( Random::EVERY_SELECT, random_kind_every_select_radio_ );
    table.Regist( Random::CONSTANT,     random_kind_constant_radio_ );
    table.Regist( Random::REAL_RANDOM,  random_kind_real_random_radio_ );
    table.SetCheck( parameter_set_.random_kind_ );
  }
  random_constant_number_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.random_constant_value_ ) );
  random_constant_number_label_.SetEnabled( random_kind_constant_radio_.GetCheck() );
  random_constant_number_edit_.SetEnabled( random_kind_constant_radio_.GetCheck() );

  return true;
}

bool
ConfigPropertySheet::RandomPage::AtKillActive( void )
{
  int tmp;
  if ( NOT( TtUtil::StringToInteger( random_constant_number_edit_.GetText(), &tmp, 10 ) ) ) {
    TtMessageBoxOk box;
    box.SetMessage( "ランダム値発生処理の定数値を正しく入力して下さい。" );
    box.SetCaption( "エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  };
  return true;
}

bool
ConfigPropertySheet::RandomPage::AtApply( void )
{
  if ( NOT( this->AtKillActive() ) ) {
    return false;
  }

  {
    using Parameter::Random;
    TtRadioCheckToValue< TtEnum<Random> > table(     Random::DEFAULT );
    table.Regist( random_kind_every_select_radio_, Random::EVERY_SELECT );
    table.Regist( random_kind_constant_radio_,     Random::CONSTANT );
    table.Regist( random_kind_real_random_radio_,  Random::REAL_RANDOM );
    parameter_set_.random_kind_ = table.GetValue();
  }
  TtUtil::StringToInteger( random_constant_number_edit_.GetText(), &parameter_set_.random_constant_value_, 10 );
  return true;
}

LRESULT
ConfigPropertySheet::RandomPage::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_RADIO_CONSTANT:
  case ID_RADIO_EVERY_SELECT:
  case ID_RADIO_REAL_RANDOM:
    random_constant_number_label_.SetEnabled( random_kind_constant_radio_.GetCheck() );
    random_constant_number_edit_.SetEnabled( random_kind_constant_radio_.GetCheck() );
    return 0;
  }
  return this->TtForm::WMCommandReceived( id, code, child );
}


// -- OutputTemplatePage -------------------------------------------------
ConfigPropertySheet::OutputTemplatePage::OutputTemplatePage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "出力" ),
parameter_set_( parameter_set )
{
}

bool
ConfigPropertySheet::OutputTemplatePage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    use_output_template_check_.Create( parameter.AlterId( ID_CHECK_USE_OUTPUT_TEMPLATE ) );
    output_template_source_edit_.Create( parameter );
    remove_can_not_use_char_as_file_path_edit_.Create( parameter );
    output_template_help1_.Create( parameter );
    output_template_help2_.Create( parameter );
    output_template_help3_.Create( parameter );
  }

  use_output_template_check_.SetPositionSize(                   8,  16, 384,  16 );
  output_template_source_edit_.SetPositionSize(                16,  40, 428,  20 );
  remove_can_not_use_char_as_file_path_edit_.SetPositionSize(  16,  72, 384,  16 );
  output_template_help1_.SetPositionSize(                       8,  96, 180,  80 );
  output_template_help2_.SetPositionSize(                     200,  96, 180,  60 );
  output_template_help3_.SetPositionSize(                       8, 152, 420,  40 );

  use_output_template_check_.SetText( "出力ファイルのパスを以下のテンプレートを使用する(&O)" );
  remove_can_not_use_char_as_file_path_edit_.SetText( "ファイルパスに使用できない文字があった場合削除する(&R)" );
  output_template_help1_.SetText( "%I ： 入力ファイルパス\r\n%J ： 入力ファイル名(拡張子なし)\r\n%K ： 入力ファイルディレクトリパス\r\n%L ： 入力ファイルディレクトリ名" );
  output_template_help2_.SetText( "%O ： 出力ファイルパス\r\n%P ： 出力ファイル名(拡張子なし)\r\n%Q ： 出力ファイルディレクトリパス\r\n%R ： 出力ファイルディレクトリ名" );
  output_template_help3_.SetText( "%#@@@# ： @@@ に BMS のヘッダ名を入れるとその値が入ります（例 ： %#TITLE# ）\r\n%=@@@= ： @@@ を strftime() に渡します。（例 ： %=%Y/%m/%d %H:%M:%S= ）" );

  use_output_template_check_.Show();
  output_template_source_edit_.Show();
  remove_can_not_use_char_as_file_path_edit_.Show();
  output_template_help1_.Show();
  output_template_help2_.Show();
  output_template_help3_.Show();

  use_output_template_check_.SetCheck( parameter_set_.use_output_template_ );
  output_template_source_edit_.SetText( parameter_set_.output_template_source_ );
  remove_can_not_use_char_as_file_path_edit_.SetCheck( parameter_set_.remove_can_not_use_char_as_file_path_ );

  this->WMCommandReceived( ID_CHECK_USE_OUTPUT_TEMPLATE, 0, 0 );

  return true;
}

bool
ConfigPropertySheet::OutputTemplatePage::AtApply( void )
{
  parameter_set_.use_output_template_ = use_output_template_check_.GetCheck();
  parameter_set_.output_template_source_ = output_template_source_edit_.GetText();
  parameter_set_.remove_can_not_use_char_as_file_path_ = remove_can_not_use_char_as_file_path_edit_.GetCheck();

  return true;
}


LRESULT
ConfigPropertySheet::OutputTemplatePage::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_CHECK_USE_OUTPUT_TEMPLATE:
    output_template_source_edit_.SetEnabled( use_output_template_check_.GetCheck() );
    remove_can_not_use_char_as_file_path_edit_.SetEnabled( use_output_template_check_.GetCheck() );
    return 0;
  }
  return this->TtForm::WMCommandReceived( id, code, child );
}


// -- AfterProcessPage ---------------------------------------------------
ConfigPropertySheet::AfterProcessPage::AfterProcessPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "後処理" ),
parameter_set_( parameter_set )
{
}

bool
ConfigPropertySheet::AfterProcessPage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    execute_after_process_check_.Create( parameter.AlterId( ID_CHECK_EXECUTE_AFTER_PROCESS ) );
    execute_file_label_.Create( parameter );;
    execute_file_edit_.Create( parameter );
    execute_file_ref_button_.Create( parameter.AlterId( ID_BUTTON_EXECUTE_FILE_REF ) );
    execute_arguments_label_.Create( parameter );;
    execute_arguments_edit_.Create( parameter );
    execute_arguments_help_.Create( parameter );
    start_on_background_check_.Create( parameter );
    wait_for_process_exit_check_.Create( parameter );
    delete_wav_file_check_.Create( parameter );
    change_play_file_check_.Create( parameter.AlterId( ID_CHECK_CHANGE_PLAY_FILE_ ) );
    play_file_path_edit_.Create( parameter );
    play_file_path_help1_.Create( parameter );
    play_file_path_help2_.Create( parameter );
  }

  execute_after_process_check_.SetPositionSize(   8,  16, 384,  16 );
  execute_file_label_.SetPositionSize(           16,  40, 384,  16 );
  execute_file_edit_.SetPositionSize(            16,  60, 300,  20 );
  execute_file_ref_button_.SetPositionSize(     320,  60,  52,  20 );
  execute_arguments_label_.SetPositionSize(      16,  88, 320,  20 );
  execute_arguments_edit_.SetPositionSize(       16, 108, 360,  20 );
  execute_arguments_help_.SetPositionSize(       24, 136, 384,  16 );
  start_on_background_check_.SetPositionSize(    16, 160, 384,  16 );
  wait_for_process_exit_check_.SetPositionSize(  16, 180, 384,  16 );
  delete_wav_file_check_.SetPositionSize(        16, 200, 384,  16 );
  change_play_file_check_.SetPositionSize(       16, 228, 384,  16 );
  play_file_path_edit_.SetPositionSize(          24, 248, 360,  20 );
  play_file_path_help1_.SetPositionSize(         32, 276, 180,  32 );
  play_file_path_help2_.SetPositionSize(        212, 276, 180,  32 );

  execute_after_process_check_.SetText( "WAV 作成後にプログラムを起動する(&E)" );
  execute_file_label_.SetText( "実行ファイル(&F)" );
  execute_file_ref_button_.SetText( "参照..." );
  execute_arguments_label_.SetText( "実行時引数(&A)" );
  execute_arguments_help_.SetText( "※ 出力 WAV ファイルは %O で指定できます" );
  start_on_background_check_.SetText( "プログラムをバックグラウンドで起動してみる(&B)" );
  wait_for_process_exit_check_.SetText( "プログラムが終了するまで待機する(&W)" );
  delete_wav_file_check_.SetText( "プログラム起動後・待機後に WAV ファイルを削除する(&D)" );
  change_play_file_check_.SetText( "「出力ファイルを開く」ボタンで開くファイルを以下に変更する(&C)" );
  play_file_path_help1_.SetText( "%O : 出力ファイルパス\r\n%Q : 出力ファイルディレクトリパス" );
  play_file_path_help2_.SetText( "%P : 出力ファイル名（拡張子なし）\r\n%R : 出力ファイルディレクトリ名" );

  execute_after_process_check_.Show();
  execute_file_label_.Show();
  execute_file_edit_.Show();
  execute_file_ref_button_.Show();
  execute_arguments_label_.Show();
  execute_arguments_edit_.Show();
  execute_arguments_help_.Show();
  start_on_background_check_.Show();
  wait_for_process_exit_check_.Show();
  delete_wav_file_check_.Show();
  change_play_file_check_.Show();
  play_file_path_edit_.Show();
  play_file_path_help1_.Show();
  play_file_path_help2_.Show();

  execute_after_process_check_.SetCheck( parameter_set_.execute_after_process_ );
  execute_file_edit_.SetText( parameter_set_.after_process_execute_file_ );
  execute_arguments_edit_.SetText( parameter_set_.after_process_execute_arguments_ );
  start_on_background_check_.SetCheck( parameter_set_.after_process_start_on_background_ );
  wait_for_process_exit_check_.SetCheck( parameter_set_.after_process_wait_for_process_exit_ );
  delete_wav_file_check_.SetCheck( parameter_set_.after_process_delete_wav_file_ );
  change_play_file_check_.SetCheck( parameter_set_.change_play_file_ );
  play_file_path_edit_.SetText( parameter_set_.play_file_path_ );

  this->WMCommandReceived( ID_CHECK_EXECUTE_AFTER_PROCESS, 0, 0 );

  return true;
}

bool
ConfigPropertySheet::AfterProcessPage::AtApply( void )
{
  parameter_set_.execute_after_process_ = execute_after_process_check_.GetCheck();
  parameter_set_.after_process_execute_file_          = execute_file_edit_.GetText();
  parameter_set_.after_process_execute_arguments_     = execute_arguments_edit_.GetText();
  parameter_set_.after_process_start_on_background_   = start_on_background_check_.GetCheck();
  parameter_set_.after_process_wait_for_process_exit_ = wait_for_process_exit_check_.GetCheck();
  parameter_set_.after_process_delete_wav_file_       = delete_wav_file_check_.GetCheck();
  parameter_set_.change_play_file_                    = change_play_file_check_.GetCheck();
  parameter_set_.play_file_path_                      = play_file_path_edit_.GetText();

  return true;
}

LRESULT
ConfigPropertySheet::AfterProcessPage::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_CHECK_EXECUTE_AFTER_PROCESS:
    execute_file_label_.SetEnabled( execute_after_process_check_.GetCheck() );
    execute_file_edit_.SetEnabled( execute_after_process_check_.GetCheck() );
    execute_file_ref_button_.SetEnabled( execute_after_process_check_.GetCheck() );
    execute_arguments_label_.SetEnabled( execute_after_process_check_.GetCheck() );
    execute_arguments_edit_.SetEnabled( execute_after_process_check_.GetCheck() );
    execute_arguments_help_.SetEnabled( execute_after_process_check_.GetCheck() );
    start_on_background_check_.SetEnabled( execute_after_process_check_.GetCheck() );
    wait_for_process_exit_check_.SetEnabled( execute_after_process_check_.GetCheck() );
    delete_wav_file_check_.SetEnabled( execute_after_process_check_.GetCheck() );
    change_play_file_check_.SetEnabled( execute_after_process_check_.GetCheck() );
    // walk through
  case ID_CHECK_CHANGE_PLAY_FILE_:
    play_file_path_edit_.SetEnabled( execute_after_process_check_.GetCheck() &&
                                     change_play_file_check_.GetCheck() );
    return 0;

  case ID_BUTTON_EXECUTE_FILE_REF: {
    TtOpenFileDialog dx;
    dx.SetFileName( execute_file_edit_.GetText() );
    dx.GetFilters().push_back( TtFileDialogFilter( "実行ファイル(*.exe)", "exe" ) );
    dx.GetFilters().push_back( TtFileDialogFilter( "全てのファイル(*.*)", "*" ) );
    if ( dx.ShowDialog( *this ) ) {
      execute_file_edit_.SetText( dx.GetFileName() );
    }
    return 0;
  }
  }
  return this->TtForm::WMCommandReceived( id, code, child );
}


// -- WavPage ------------------------------------------------------------
ConfigPropertySheet::WavPage::WavPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "WAV" ),
parameter_set_( parameter_set )
{
}

bool
ConfigPropertySheet::WavPage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    not_use_low_pass_filter_check_.Create( parameter );
    wav_file_not_found_reaction_label_.Create( parameter );
    wav_file_not_found_reaction_combo_.Create( parameter );
    wav_format_error_reaction_label_.Create( parameter );
    wav_format_error_reaction_combo_.Create( parameter );
    wav_file_error_reaction_label_.Create( parameter );
    wav_file_error_reaction_combo_.Create( parameter );
  }

  not_use_low_pass_filter_check_.SetPositionSize(       8,  16, 432,  16 );
  wav_file_not_found_reaction_label_.SetPositionSize(   8,  46, 432,  16 );
  wav_file_not_found_reaction_combo_.SetPositionSize(  16,  66, 400,  80 );
  wav_format_error_reaction_label_.SetPositionSize(     8,  96, 432,  16 );
  wav_format_error_reaction_combo_.SetPositionSize(    16, 116, 400,  60 );
  wav_file_error_reaction_label_.SetPositionSize(       8, 146, 432,  16 );
  wav_file_error_reaction_combo_.SetPositionSize(      16, 166, 400,  60 );

  not_use_low_pass_filter_check_.SetText( "#WAV?? のファイル読み込み時にローパスフィルタ(ノイズ除去)を使わない(&F)" );
  {
    using Parameter::WavFileNotFoundReaction;
    wav_file_not_found_reaction_label_.SetText( "#WAV?? に指定したファイルが存在しなかった場合の動作(&E)" );
    ComboBoxForEnum<WavFileNotFoundReaction>& tmp = wav_file_not_found_reaction_combo_;
    tmp.SetAlias( WavFileNotFoundReaction::DEFAULT, WavFileNotFoundReaction::MUST_ASK );
    tmp.PushWithData( "常にエラーとみなし、変換を中止する",                                   WavFileNotFoundReaction::ERROR );
    tmp.PushWithData( "使用されていない場合無視、それ以外は無視して続行するかユーザに尋ねる", WavFileNotFoundReaction::ASK_ONLY_ERROR );
    tmp.PushWithData( "無視して続行するかユーザに尋ねる",                                     WavFileNotFoundReaction::MUST_ASK );
    tmp.PushWithData( "常に無視して続行する",                                                 WavFileNotFoundReaction::IGNORE );
  }
  {
    using Parameter::WavFormatErrorReaction;
    wav_format_error_reaction_label_.SetText( "WAV ファイルの些細なフォーマットエラーの場合の動作(&H)" );
    ComboBoxForEnum<WavFormatErrorReaction>& tmp = wav_format_error_reaction_combo_;
    tmp.SetAlias( WavFormatErrorReaction::DEFAULT, WavFormatErrorReaction::ASK );
    tmp.PushWithData( "常にそのファイルを WAV ファイルエラーとみなす",                      WavFormatErrorReaction::ERROR );
    tmp.PushWithData( "無視して読み込んでみるか、WAV ファイルエラーにするかユーザに尋ねる", WavFormatErrorReaction::ASK );
    tmp.PushWithData( "常に些細なエラーを無視して読み込んでみる",                           WavFormatErrorReaction::IGNORE );
  }
  {
    using Parameter::WavFileErrorReaction;
    wav_file_error_reaction_label_.SetText( "WAV ファイルエラーの場合の動作(&N)" );
    ComboBoxForEnum<WavFileErrorReaction>& tmp = wav_file_error_reaction_combo_;
    tmp.SetAlias( WavFileErrorReaction::DEFAULT, WavFileErrorReaction::ASK );
    tmp.PushWithData( "常にそのファイルをエラーとみなして、変換を中止する", WavFileErrorReaction::ERROR );
    tmp.PushWithData( "無視して続行するか、変換を中止するかユーザに尋ねる", WavFileErrorReaction::ASK );
    tmp.PushWithData( "常に無視して続行する",                               WavFileErrorReaction::IGNORE );
  }

  not_use_low_pass_filter_check_.Show();
  wav_file_not_found_reaction_label_.Show();
  wav_file_not_found_reaction_combo_.Show();
  wav_format_error_reaction_label_.Show();
  wav_format_error_reaction_combo_.Show();
  wav_file_error_reaction_label_.Show();
  wav_file_error_reaction_combo_.Show();

  not_use_low_pass_filter_check_.SetCheck( parameter_set_.not_use_low_pass_filter_ );
  wav_file_not_found_reaction_combo_.SetSelectByItemData( parameter_set_.wav_file_not_found_reaction_ );
  wav_format_error_reaction_combo_.SetSelectByItemData( parameter_set_.wav_format_error_reaction_ );
  wav_file_error_reaction_combo_.SetSelectByItemData( parameter_set_.wav_file_error_reaction_ );

  return true;
}

bool
ConfigPropertySheet::WavPage::AtApply( void )
{
  parameter_set_.not_use_low_pass_filter_ = not_use_low_pass_filter_check_.GetCheck();
  parameter_set_.wav_file_not_found_reaction_ = wav_file_not_found_reaction_combo_.GetSelectedItemData();
  parameter_set_.wav_format_error_reaction_ = wav_format_error_reaction_combo_.GetSelectedItemData();
  parameter_set_.wav_file_error_reaction_ = wav_file_error_reaction_combo_.GetSelectedItemData();

  return true;
}


// -- CheckPage ----------------------------------------------------------
ConfigPropertySheet::CheckPage::CheckPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "検証" ),
parameter_set_( parameter_set )
{
}

bool
ConfigPropertySheet::CheckPage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    check_wav_file_exists_check_.Create( parameter );
    check_bmp_file_exists_check_.Create( parameter );
    check_stage_file_exists_check_.Create( parameter );
    check_file_exists_group_.Create( parameter );
    check_wav_used_check_.Create( parameter );
    check_bmp_used_check_.Create( parameter );
    bmp_check_attention_label_.Create( parameter );
    check_bpm_used_check_.Create( parameter );
    check_stop_used_check_.Create( parameter );
    check_used_group_.Create( parameter );
  }

  check_wav_file_exists_check_.SetPositionSize(    16,  28, 384,  16 );
  check_bmp_file_exists_check_.SetPositionSize(    16,  48, 384,  16 );
  check_stage_file_exists_check_.SetPositionSize(  16,  68, 384,  16 );
  check_file_exists_group_.SetPositionSize(         8,   8, 400,  88 );
  check_wav_used_check_.SetPositionSize(           16, 140, 384,  16 );
  check_bmp_used_check_.SetPositionSize(           16, 160, 384,  16 );
  bmp_check_attention_label_.SetPositionSize(      40, 180, 384,  16 );
  check_bpm_used_check_.SetPositionSize(           16, 200, 384,  16 );
  check_stop_used_check_.SetPositionSize(          16, 220, 384,  16 );
  check_used_group_.SetPositionSize(                8, 120, 400, 128 );

  check_wav_file_exists_check_.SetText( "#WAV00 ～ #WAVZZ (&W)" );
  check_bmp_file_exists_check_.SetText( "#BMP00 ～ #BMPZZ (&B)" );
  check_stage_file_exists_check_.SetText( "#STAGEFILE (&S)" );
  check_file_exists_group_.SetText( "定義で指定されたファイルが存在するかチェックする" );
  check_wav_used_check_.SetText( "#WAV01 ～ #WAVZZ (&V)" );
  check_bmp_used_check_.SetText( "#BMP01 ～ #BMPZZ (&M)" );
  bmp_check_attention_label_.SetText( "※注意 : BMS タブの BGA チャンネルを無視する設定を見直すこと" );
  check_bpm_used_check_.SetText( "#BPM01 ～ #BPMZZ (&P)" );
  check_stop_used_check_.SetText( "#STOP01 ～ #STOPZZ (&T)" );
  check_used_group_.SetText( "定義されたオブジェが BMS 内で使用しているかチェックする" );

  check_wav_file_exists_check_.Show();
  check_bmp_file_exists_check_.Show();
  check_stage_file_exists_check_.Show();
  check_file_exists_group_.Show();
  check_wav_used_check_.Show();
  check_bmp_used_check_.Show();
  bmp_check_attention_label_.Show();
  check_bpm_used_check_.Show();
  check_stop_used_check_.Show();
  check_used_group_.Show();

  check_wav_file_exists_check_.SetCheck( parameter_set_.check_wav_file_exists_ );
  check_bmp_file_exists_check_.SetCheck( parameter_set_.check_bmp_file_exists_ );
  check_stage_file_exists_check_.SetCheck( parameter_set_.check_stage_file_exists_ );
  check_wav_used_check_.SetCheck( parameter_set_.check_wav_used_ );
  check_bmp_used_check_.SetCheck( parameter_set_.check_bmp_used_ );
  check_bpm_used_check_.SetCheck( parameter_set_.check_bpm_used_ );
  check_stop_used_check_.SetCheck( parameter_set_.check_stop_used_ );

  return true;
}

bool
ConfigPropertySheet::CheckPage::AtApply( void )
{
  parameter_set_.check_wav_file_exists_   = check_wav_file_exists_check_.GetCheck();
  parameter_set_.check_bmp_file_exists_   = check_bmp_file_exists_check_.GetCheck();
  parameter_set_.check_stage_file_exists_ = check_stage_file_exists_check_.GetCheck();
  parameter_set_.check_wav_used_          = check_wav_used_check_.GetCheck();
  parameter_set_.check_bmp_used_          = check_bmp_used_check_.GetCheck();
  parameter_set_.check_bpm_used_          = check_bpm_used_check_.GetCheck();
  parameter_set_.check_stop_used_         = check_stop_used_check_.GetCheck();

  return true;
}

// -- CompressorPage -----------------------------------------------------
ConfigPropertySheet::CompressorPage::CompressorPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set ) :
TtPropertySheetPage( sheet, "コンプレッサ" ),
parameter_set_( parameter_set )
{
}

bool
ConfigPropertySheet::CompressorPage::AtPageCreate( void )
{
  return parameter_set_.show_compressor_;
}

bool
ConfigPropertySheet::CompressorPage::Created( void )
{
  {
    TtWindow::CreateParameter parameter( this );

    use_compressor_check_.Create( parameter.AlterId( ID_CHECK_USE_COMPRESSOR ) );
    threshold_label_.Create( parameter );
    threshold_edit_.Create( parameter );
    threshold_unit_label_.Create( parameter );
    ratio_label_.Create( parameter );
    ratio_edit_.Create( parameter );
    ratio_unit_label_.Create( parameter );
    attack_time_label_.Create( parameter );
    attack_time_edit_.Create( parameter );
    attack_time_unit_label_.Create( parameter );
    release_time_label_.Create( parameter );
    release_time_edit_.Create( parameter );
    release_time_unit_label_.Create( parameter );
    look_ahead_label_.Create( parameter );
    look_ahead_edit_.Create( parameter );
    look_ahead_unit_label_.Create( parameter );
  }

  use_compressor_check_.SetPositionSize(      8,  16,  384, 16 );
  threshold_label_.SetPositionSize(           8,  48,  110, 16 );
  threshold_edit_.SetPositionSize(          120,  44,  120, 20 );
  threshold_unit_label_.SetPositionSize(    248,  48,  110, 16 );
  ratio_label_.SetPositionSize(               8,  72,  110, 16 );
  ratio_edit_.SetPositionSize(              120,  68,  120, 20 );
  ratio_unit_label_.SetPositionSize(        248,  72,  110, 16 );
  attack_time_label_.SetPositionSize(         8,  96,  110, 16 );
  attack_time_edit_.SetPositionSize(        120,  92,  120, 20 );
  attack_time_unit_label_.SetPositionSize(  248,  96,  110, 16 );
  release_time_label_.SetPositionSize(        8, 120,  110, 16 );
  release_time_edit_.SetPositionSize(       120, 116,  120, 20 );
  release_time_unit_label_.SetPositionSize( 248, 120,  110, 16 );
  look_ahead_label_.SetPositionSize(          8, 144,  110, 16 );
  look_ahead_edit_.SetPositionSize(         120, 140,  120, 20 );
  look_ahead_unit_label_.SetPositionSize(   248, 144,  110, 16 );

  use_compressor_check_.SetText( "コンプレッサを使用する" );
  threshold_label_.SetText( "スレッショルド(実数) :" );
  threshold_unit_label_.SetText( "倍" );
  ratio_label_.SetText( "レシオ(実数) :" );
  ratio_unit_label_.SetText( "倍" );
  attack_time_label_.SetText( "アタック(整数) :" );
  attack_time_unit_label_.SetText( "/ 44100 秒" );
  release_time_label_.SetText( "リリース(整数) :" );
  release_time_unit_label_.SetText( "/ 44100 秒" );
  look_ahead_label_.SetText( "先読み(整数) :" );
  look_ahead_unit_label_.SetText( "/ 44100 秒" );

  use_compressor_check_.Show();
  threshold_label_.Show();
  threshold_edit_.Show();
  threshold_unit_label_.Show();
  ratio_label_.Show();
  ratio_edit_.Show();
  ratio_unit_label_.Show();
  attack_time_label_.Show();
  attack_time_edit_.Show();
  attack_time_unit_label_.Show();
  release_time_label_.Show();
  release_time_edit_.Show();
  release_time_unit_label_.Show();
  look_ahead_label_.Show();
  look_ahead_edit_.Show();
  look_ahead_unit_label_.Show();

  use_compressor_check_.SetCheck( parameter_set_.use_compressor_ );
  threshold_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.compress_threshold_ ) );
  ratio_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.compress_ratio_ ) );
  attack_time_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.compress_attack_time_ ) );
  release_time_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.compress_release_time_ ) );
  look_ahead_edit_.SetText( TtUtil::ToStringFrom( parameter_set_.compress_look_ahead_ ) );

  this->SetControlsEnabledByUseCompressorCheckBox();
  return true;
}

bool
ConfigPropertySheet::CompressorPage::AtKillActive( void )
{
  TtMessageBoxOk box;
  box.SetCaption( "エラー" );
  box.SetIcon( TtMessageBox::Icon::ERROR );
  int    tmp_int;
  double tmp_double;
  if ( NOT( TtUtil::StringToDouble( threshold_edit_.GetText(), &tmp_double ) ) ) {
    box.SetMessage( "スレッショルドの値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  if ( NOT( TtUtil::StringToDouble( ratio_edit_.GetText(), &tmp_double ) ) ) {
    box.SetMessage( "レシオの値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  if ( NOT( TtUtil::StringToInteger( attack_time_edit_.GetText(), &tmp_int, 10 ) ) ) {
    box.SetMessage( "アタックの値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  if ( NOT( TtUtil::StringToInteger( release_time_edit_.GetText(), &tmp_int, 10 ) ) ) {
    box.SetMessage( "リリースの値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  if ( NOT( TtUtil::StringToInteger( look_ahead_edit_.GetText(), &tmp_int, 10 ) ) ) {
    box.SetMessage( "先読み時間の値を正しく入力して下さい。" );
    box.ShowDialog( *this );
    return false;
  };
  return true;
}

bool
ConfigPropertySheet::CompressorPage::AtApply( void )
{
  if ( NOT( this->AtKillActive() ) ) {
    return false;
  }
  parameter_set_.use_compressor_ = use_compressor_check_.GetCheck();
  TtUtil::StringToDouble(  threshold_edit_.GetText(),    &parameter_set_.compress_threshold_ );
  TtUtil::StringToDouble(  ratio_edit_.GetText(),        &parameter_set_.compress_ratio_ );
  TtUtil::StringToInteger( attack_time_edit_.GetText(),  &static_cast<int>( parameter_set_.compress_attack_time_ ), 10 );
  TtUtil::StringToInteger( release_time_edit_.GetText(), &static_cast<int>( parameter_set_.compress_release_time_ ), 10 );
  TtUtil::StringToInteger( look_ahead_edit_.GetText(),   &parameter_set_.compress_look_ahead_, 10 );

  return true;
}

LRESULT
ConfigPropertySheet::CompressorPage::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_CHECK_USE_COMPRESSOR:
    this->SetControlsEnabledByUseCompressorCheckBox();
    return 0;
  }
  return this->TtForm::WMCommandReceived( id, code, child );
}

void
ConfigPropertySheet::CompressorPage::SetControlsEnabledByUseCompressorCheckBox( void )
{
  threshold_label_.SetEnabled( use_compressor_check_.GetCheck() );
  threshold_edit_.SetEnabled( use_compressor_check_.GetCheck() );
  threshold_unit_label_.SetEnabled( use_compressor_check_.GetCheck() );
  ratio_label_.SetEnabled( use_compressor_check_.GetCheck() );
  ratio_edit_.SetEnabled( use_compressor_check_.GetCheck() );
  ratio_unit_label_.SetEnabled( use_compressor_check_.GetCheck() );
  attack_time_label_.SetEnabled( use_compressor_check_.GetCheck() );
  attack_time_edit_.SetEnabled( use_compressor_check_.GetCheck() );
  attack_time_unit_label_.SetEnabled( use_compressor_check_.GetCheck() );
  release_time_label_.SetEnabled( use_compressor_check_.GetCheck() );
  release_time_edit_.SetEnabled( use_compressor_check_.GetCheck() );
  release_time_unit_label_.SetEnabled( use_compressor_check_.GetCheck() );
  look_ahead_label_.SetEnabled( use_compressor_check_.GetCheck() );
  look_ahead_edit_.SetEnabled( use_compressor_check_.GetCheck() );
  look_ahead_unit_label_.SetEnabled( use_compressor_check_.GetCheck() );
}


// -- ConfigPropertySheet ------------------------------------------------
ConfigPropertySheet::ConfigPropertySheet( ParameterSet& parameter_set ) :
TtPropertySheet( false ),
bms_page_( *this, parameter_set ),
convert_page_( *this, parameter_set ),
volume_page_( *this, parameter_set ),
random_page_( *this, parameter_set ),
output_template_page_( *this, parameter_set ),
after_process_page_( *this, parameter_set ),
wav_page_( *this, parameter_set ),
check_page_( *this, parameter_set ),
compressor_page_( *this, parameter_set )
{
}


bool
ConfigPropertySheet::Created( void )
{
  this->SetText( "詳細設定" );
  this->SetIcon( ::LoadIcon( ::GetModuleHandle( NULL ), "MAIN_ICON" ), true );

  return true;
}
