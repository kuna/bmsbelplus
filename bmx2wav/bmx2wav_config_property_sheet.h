#ifndef BMX2WAV_CONFIG_PROPERTY_SHEET_H_
#define BMX2WAV_CONFIG_PROPERTY_SHEET_H_

#include "tt_window_controls.h"
#include "tt_combo_box.h"
#include "tt_property_sheet.h"
#include "tt_enum.h"

#include "bmx2wav_converter_parameter.h"
#include "bmx2wav_converter.h"

namespace Bmx2Wav {
  template <class EnumBase>
  class ComboBoxForEnum : public TtComboBoxTableDropDownNotEdit< TtEnum<EnumBase> > {};

  class ConfigPropertySheet : public TtPropertySheet {
  private:
    // -- BmsPage --------------------------------------------------------
    class BmsPage : public TtPropertySheetPage {
    public:
      explicit BmsPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool Created( void );
      virtual bool AtApply( void );

    private:
      ParameterSet& parameter_set_;

      TtCheckBox ignore_bga_channel_check_;
      TtCheckBox different_long_note_enclosed_object_is_error_check_;

      TtStatic parse_duplicate_header_reaction_label_;
      ComboBoxForEnum<Parameter::ParseDuplicateHeaderReaction>
        parse_duplicate_header_reaction_combo_;

      TtStatic parse_duplicate_bar_change_reaction_label_;
      ComboBoxForEnum<Parameter::ParseDuplicateBarChangeReaction>
        parse_duplicate_bar_change_reaction_combo_;

      TtStatic parse_error_reaction_label_;
      ComboBoxForEnum<Parameter::ParseErrorReaction>
        parse_error_reaction_combo_;
    };

    // -- ConvertPage ----------------------------------------------------
    class ConvertPage : public TtPropertySheetPage {
    private:
      enum {
        ID_CHECK_DO_TRIMING = 10001,
      };

    public:
      explicit ConvertPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool Created( void );
      virtual bool AtKillActive( void );
      virtual bool AtApply( void );
      virtual LRESULT WMCommandReceived( int id, int code, HWND child );

    private:
      ParameterSet& parameter_set_;

      TtCheckBox   cancel_at_resounding_check_;
      TtCheckBox   not_sound_object_specified_by_lnobj_check_;
      TtCheckBox   error_in_not_entried_wav_was_used_check_;
      TtStatic     start_label_;
      TtStatic     end_label_;
      TtStatic     convert_start_end_bar_number_label_;
      TtEditNumber convert_start_bar_number_edit_;
      TtEditNumber convert_end_bar_number_edit_;
      TtCheckBox   do_triming_check_;
      TtEditNumber triming_start_bar_number_edit_;
      TtEditNumber triming_end_bar_number_edit_;

      TtStatic scroll_object_define_error_reaction_label_;
      ComboBoxForEnum<Parameter::ScrollObjectDefineErrorReaction>
        scroll_object_define_error_reaction_combo_;

      TtStatic scroll_object_entry_is_empty_reaction_label_;
      ComboBoxForEnum<Parameter::ScrollObjectEntryIsEmptyReaction>
        scroll_object_entry_is_empty_reaction_combo_;
    };

    // -- VolumePage -----------------------------------------------------
    class VolumePage : public TtPropertySheetPage {
    private:
      enum {
        ID_RADIO_PEAK = 10001,
        ID_RADIO_AVERAGE,
        ID_RADIO_NONE,
        ID_RADIO_OVER,
      };

    public:
      explicit VolumePage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool Created( void );
      virtual bool AtKillActive( void );
      virtual bool AtApply( void );
      virtual LRESULT WMCommandReceived( int id, int code, HWND child );

    private:
      ParameterSet& parameter_set_;

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

    // -- RandomPage -----------------------------------------------------
    class RandomPage : public TtPropertySheetPage {
    private:
      enum {
        ID_RADIO_EVERY_SELECT = 10001,
        ID_RADIO_CONSTANT,
        ID_RADIO_REAL_RANDOM,
      };

    public:
      explicit RandomPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool Created( void );
      virtual bool AtKillActive( void );
      virtual bool AtApply( void );
      virtual LRESULT WMCommandReceived( int id, int code, HWND child );

    private:
      ParameterSet& parameter_set_;

      TtRadioButton random_kind_every_select_radio_;
      TtRadioButton random_kind_constant_radio_;
      TtRadioButton random_kind_real_random_radio_;
      TtGroup       random_kind_group_;
      TtStatic      random_constant_number_label_;
      TtEditNumber  random_constant_number_edit_;
    };

    // -- OutputTemplatePage ---------------------------------------------
    class OutputTemplatePage : public TtPropertySheetPage {
    private:
      enum {
        ID_CHECK_USE_OUTPUT_TEMPLATE = 10001,
      };

    public:
      explicit OutputTemplatePage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool Created( void );
      virtual bool AtApply( void );
      virtual LRESULT WMCommandReceived( int id, int code, HWND child );

    private:
      ParameterSet& parameter_set_;

      TtCheckBox use_output_template_check_;
      TtEdit     output_template_source_edit_;
      TtCheckBox remove_can_not_use_char_as_file_path_edit_;
      TtStatic   output_template_help1_;
      TtStatic   output_template_help2_;
      TtStatic   output_template_help3_;
    };

    // -- AfterProcessPage -----------------------------------------------
    class AfterProcessPage : public TtPropertySheetPage {
    private:
      enum {
        ID_CHECK_EXECUTE_AFTER_PROCESS = 10001,
        ID_BUTTON_EXECUTE_FILE_REF,
        ID_CHECK_CHANGE_PLAY_FILE_,
      };

    public:
      explicit AfterProcessPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool Created( void );
      virtual bool AtApply( void );
      virtual LRESULT WMCommandReceived( int id, int code, HWND child );

    private:
      ParameterSet& parameter_set_;

      TtCheckBox execute_after_process_check_;
      TtStatic   execute_file_label_;
      TtEdit     execute_file_edit_;
      TtButton   execute_file_ref_button_;
      TtStatic   execute_arguments_label_;
      TtEdit     execute_arguments_edit_;
      TtStatic   execute_arguments_help_;
      TtCheckBox start_on_background_check_;
      TtCheckBox wait_for_process_exit_check_;
      TtCheckBox delete_wav_file_check_;
      TtCheckBox change_play_file_check_;
      TtEdit     play_file_path_edit_;
      TtStatic   play_file_path_help1_;
      TtStatic   play_file_path_help2_;
    };

    // -- WavPage --------------------------------------------------------
    class WavPage : public TtPropertySheetPage {
    public:
      explicit WavPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool Created( void );
      virtual bool AtApply( void );

    private:
      ParameterSet& parameter_set_;

      TtCheckBox    not_use_low_pass_filter_check_;

      TtStatic wav_file_not_found_reaction_label_;
      ComboBoxForEnum<Parameter::WavFileNotFoundReaction>
        wav_file_not_found_reaction_combo_;

      TtStatic wav_format_error_reaction_label_;
      ComboBoxForEnum<Parameter::WavFormatErrorReaction>
        wav_format_error_reaction_combo_;

      TtStatic wav_file_error_reaction_label_;
      ComboBoxForEnum<Parameter::WavFileErrorReaction>
        wav_file_error_reaction_combo_;
    };

    // -- CheckPage ------------------------------------------------------
    class CheckPage : public TtPropertySheetPage {
    public:
      explicit CheckPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool Created( void );
      virtual bool AtApply( void );

    private:
      ParameterSet& parameter_set_;

      TtCheckBox check_wav_file_exists_check_;
      TtCheckBox check_bmp_file_exists_check_;
      TtCheckBox check_stage_file_exists_check_;
      TtGroup    check_file_exists_group_;
      TtCheckBox check_wav_used_check_;
      TtCheckBox check_bmp_used_check_;
      TtStatic   bmp_check_attention_label_;
      TtCheckBox check_bpm_used_check_;
      TtCheckBox check_stop_used_check_;
      TtGroup    check_used_group_;
    };

    // -- CompressorPage -------------------------------------------------
    class CompressorPage : public TtPropertySheetPage {
    private:
      enum {
        ID_CHECK_USE_COMPRESSOR = 10001,
      };

    public:
      explicit CompressorPage( ConfigPropertySheet& sheet, ParameterSet& parameter_set );

      virtual bool AtPageCreate( void );
      virtual bool Created( void );
      virtual bool AtKillActive( void );
      virtual bool AtApply( void );
      virtual LRESULT WMCommandReceived( int id, int code, HWND child );
      void SetControlsEnabledByUseCompressorCheckBox( void );

    private:
      ParameterSet& parameter_set_;

      TtCheckBox   use_compressor_check_;
      TtStatic     threshold_label_;
      TtEdit       threshold_edit_;
      TtStatic     threshold_unit_label_;
      TtStatic     ratio_label_;
      TtEdit       ratio_edit_;
      TtStatic     ratio_unit_label_;
      TtStatic     attack_time_label_;
      TtEditNumber attack_time_edit_;
      TtStatic     attack_time_unit_label_;
      TtStatic     release_time_label_;
      TtEditNumber release_time_edit_;
      TtStatic     release_time_unit_label_;
      TtStatic     look_ahead_label_;
      TtEdit       look_ahead_edit_;
      TtStatic     look_ahead_unit_label_;
    };

    // -- ConfigPropertySheet --------------------------------------------
  public:
    explicit ConfigPropertySheet( ParameterSet& parameter_set );

    virtual bool Created( void );

  private:
    BmsPage            bms_page_;
    ConvertPage        convert_page_;
    VolumePage         volume_page_;
    RandomPage         random_page_;
    OutputTemplatePage output_template_page_;
    AfterProcessPage   after_process_page_;
    WavPage            wav_page_;
    CheckPage          check_page_;
    CompressorPage     compressor_page_;
  };
}

#endif // BMX2WAV_CONFIG_PROPERTY_SHEET_H_
