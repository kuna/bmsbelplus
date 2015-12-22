#ifndef BMX2WAV_SIMPLE_CONFIG_DIALOG_H_
#define BMX2WAV_SIMPLE_CONFIG_DIALOG_H_

#include "tt_window_controls.h"
#include "tt_dialog.h"

#include "bmx2wav_converter.h"

namespace Bmx2Wav {
  class SimpleConfigDialog : public TtDialog {
  public:
    enum {
      ID_BUTTON_OK = 10001,
      ID_BUTTON_CANCEL,

      ID_RADIO_PEAK,
      ID_RADIO_AVERAGE,
      ID_RADIO_NONE,
      ID_RADIO_OVER,
    };
    explicit SimpleConfigDialog( ParameterSet& parameter_set );

    virtual bool Created( void );
    virtual LRESULT WMCommandReceived( int id, int code, HWND child );

  private:
    ParameterSet& parameter_set_;

    TtButton      ok_button_;
    TtButton      cancel_button_;
    TtCheckBox    cancel_at_resounding_check_;
    TtRadioButton normalize_kind_peak_radio_;
    TtRadioButton normalize_kind_average_radio_;
    TtRadioButton normalize_kind_none_radio_;
    TtRadioButton normalize_kind_over_radio_;
    TtGroup       normalize_kind_group_;
    TtStatic      normalize_over_ppm_label_;
    TtEditNumber  normalize_over_ppm_edit_;
    TtStatic      volume_label_;
    TtEditNumber  volume_edit_;
  };
}

#endif // BMX2WAV_SIMPLE_CONFIG_DIALOG_H_
