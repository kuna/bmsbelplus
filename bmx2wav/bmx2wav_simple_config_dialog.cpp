#include "ttl_define.h"
#include "tt_message_box.h"
#include "tt_window_util.h"
#include "tt_util.h"

#include "bmx2wav_simple_config_dialog.h"


using namespace Bmx2Wav;


SimpleConfigDialog::SimpleConfigDialog( ParameterSet& parameter_set ) :
parameter_set_( parameter_set )
{
}


bool
SimpleConfigDialog::Created( void )
{
  this->SetIcon( ::LoadIcon( ::GetModuleHandle( NULL ), "MAIN_ICON" ), true );

  {
    TtWindow::CreateParameter parameter( this );
    ok_button_.Create( parameter.AlterId( ID_BUTTON_OK ) );
    cancel_button_.Create( parameter.AlterId( ID_BUTTON_CANCEL ) );
    cancel_at_resounding_check_.Create( parameter );
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

  this->SetText( "簡易設定" );

  this->SetSize( 412, 230 );
  ok_button_.SetPositionSize(     this->GetWidth() - 200, this->GetHeight() - 52, 88, 20 );
  cancel_button_.SetPositionSize( this->GetWidth() - 100, this->GetHeight() - 52, 88, 20 );
  cancel_at_resounding_check_.SetPositionSize(     8,  16, 384,  16 );
  normalize_kind_peak_radio_.SetPositionSize(     16,  64, 100,  16 );
  normalize_kind_average_radio_.SetPositionSize( 136,  64, 100,  16 );
  normalize_kind_none_radio_.SetPositionSize(    256,  64, 100,  16 );
  normalize_kind_over_radio_.SetPositionSize(     16,  90, 108,  16 );
  normalize_kind_group_.SetPositionSize(           8,  44, 384,  80 );
  normalize_over_ppm_label_.SetPositionSize(     128,  91, 114,  16 );
  normalize_over_ppm_edit_.SetPositionSize(      244,  87,  60,  20 );
  volume_label_.SetPositionSize(                   8, 138,  70,  16 );
  volume_edit_.SetPositionSize(                   72, 134,  40,  20 );

  ok_button_.SetText( "OK" );
  cancel_button_.SetText( "キャンセル" );
  cancel_at_resounding_check_.SetText( "WAV が鳴り終わる前に同じ WAV が鳴った場合、前の WAV を消す(&D)" );
  normalize_kind_peak_radio_.SetText( "ピーク(&P)" );
  normalize_kind_average_radio_.SetText( "平均(&A)");
  normalize_kind_none_radio_.SetText( "やんない(&Y)" );
  normalize_kind_over_radio_.SetText( "微少標本廃棄(&O)" );
  normalize_kind_group_.SetText( "ノーマライズ" );
  normalize_over_ppm_label_.SetText( "廃棄標本率(ppm)(&N):" );
  volume_label_.SetText( "音量(%)(&V) :" );

  ok_button_.Show();
  cancel_button_.Show();
  cancel_at_resounding_check_.Show();
  normalize_kind_peak_radio_.Show();
  normalize_kind_average_radio_.Show();
  normalize_kind_none_radio_.Show();
  normalize_kind_over_radio_.Show();
  normalize_kind_group_.Show();
  normalize_over_ppm_edit_.Show();
  normalize_over_ppm_label_.Show();
  volume_label_.Show();
  volume_edit_.Show();

  cancel_at_resounding_check_.SetCheck( parameter_set_.cancel_at_resounding_ );
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

  this->SetCenterRelativeToParent();

  return true;
}

LRESULT
SimpleConfigDialog::WMCommandReceived( int id, int code, HWND child )
{
  switch ( id ) {
  case ID_BUTTON_OK: {
    int tmp;
    if ( NOT( TtUtil::StringToInteger( volume_edit_.GetText(), &tmp, 10 ) ) ) {
      TtMessageBoxOk box;
      box.SetMessage( "音量の値を正しく入力して下さい。" );
      box.SetCaption( "エラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
      return 0;
    }
    if ( NOT( TtUtil::StringToInteger( normalize_over_ppm_edit_.GetText(), &tmp, 10 ) ) ) {
      TtMessageBoxOk box;
      box.SetMessage( "自動超過ノーマライズの数値を正しく入力して下さい。" );
      box.SetCaption( "エラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
      return 0;
    };
    parameter_set_.cancel_at_resounding_ = cancel_at_resounding_check_.GetCheck();
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
    this->Close();
    return 0;
  }

  case ID_BUTTON_CANCEL:
    this->Close();
    return 0;

  case ID_RADIO_OVER:
  case ID_RADIO_PEAK:
  case ID_RADIO_AVERAGE:
  case ID_RADIO_NONE:
    normalize_over_ppm_label_.SetEnabled( normalize_kind_over_radio_.GetCheck() );
    normalize_over_ppm_edit_.SetEnabled( normalize_kind_over_radio_.GetCheck() );
    return 0;
  };
  return this->TtDialog::WMCommandReceived( id, code, child );
}
