#ifndef BMX2WAV_BATCH_DIALOG_H_
#define BMX2WAV_BATCH_DIALOG_H_

#include "tt_window_controls.h"
#include "tt_dialog.h"
#include "tt_listview.h"
#include "tt_logger.h"

#include "bmx2wav.h"
#include "bmx2wav_converter.h"
#include "bmx2wav_converting_dialog.h"

namespace Bmx2Wav {

  // -- BatchEntry -------------------------------------------------------
  struct BatchEntry {
  public:
    bool        is_separator_;
    std::string input_filename_;
    std::string output_filename_;
    std::string bms_title_;
    std::string bms_artist_;
    std::string bms_genre_;
    bool        is_done_;

    explicit BatchEntry( void );
  };

  // -- BatchBmsList -----------------------------------------------------
  class BatchDialog;
  class BatchBmsList : public TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS> {
  public:
    explicit BatchBmsList( BatchDialog& dialog );

    virtual LRESULT WindowProcedure( UINT msg, WPARAM w_param, LPARAM l_param );

  private:
    BatchDialog& dialog_;
  };

  // -- BatchLogDialog ---------------------------------------------------
  class BatchLogDialog : public TtWindowWithStyle<TtDialogModeless, WS_SIZEBOX> {
  public:
    explicit BatchLogDialog( void );

    template <class TYPE> 
    BatchLogDialog& operator <<( const TYPE& value ) {
      int tmp = log_edit_.GetTextLength();
      log_edit_.SendMessage( EM_SETSEL, tmp, tmp );
      log_edit_.SendMessage( EM_REPLACESEL, 0, reinterpret_cast<WPARAM>( TtUtil::ToStringFrom( value ).c_str() ) );
      return *this;
    }
    enum Period {
      PERIOD
    };
    void operator <<( Period period );

  protected:
    virtual DWORD GetExtendedStyle( void );
    virtual bool Created( void );

    virtual LRESULT WMCloseReceived( void );
    virtual LRESULT WMSizeReceived( int flag, int width, int height );

  private:
    TtWindowWithStyle<TtEditMultiline, WS_VSCROLL> log_edit_;
  };

  // -- BatchLogger ------------------------------------------------------
  class BatchLogger : public TtLoggerStandard {
  public:
    explicit BatchLogger( const std::string& filename );

    void AddTimeToBuffer( void );

    virtual void Put( const std::string& value );

  protected:
    virtual void Prefix( std::string& str );
  };

  // -- BatchDialog ------------------------------------------------------
  class BatchDialog : public TtWindowWithStyle<TtDialog, WS_SIZEBOX> {
  private:
    friend BatchBmsList;
    enum {
      ID_MENU_SIMPLE_CONFIG      = ID_RC_BATCH_MENU_SIMPLE_CONFIG,
      ID_MENU_CONFIG             = ID_RC_BATCH_MENU_CONFIG,
      ID_MENU_SHOW_LOG_DIALOG    = ID_RC_BATCH_MENU_SHOW_LOG_DIALOG,
      ID_MENU_OPEN_DIR           = ID_RC_BATCH_MENU_OPEN_DIR,
      ID_MENU_OPEN_DIR_RECURSIVE = ID_RC_BATCH_MENU_OPEN_DIR_RECURSIVE,
      ID_MENU_DELETE_DONE_MENU   = ID_RC_BATCH_MENU_DELETE_DONE_ITEM,
      ID_MENU_ADD_SEPARATOR      = ID_RC_BATCH_MENU_ADD_SEPARATOR,
      ID_LISTVIEW = 10001,
      ID_BUTTON_UP,
      ID_BUTTON_DOWN,
      ID_BUTTON_DELETE,
      ID_BUTTON_ADD,
      ID_CHECK_NO_MESSAGE_BOX,
      ID_CHECK_LOG_FILE,
      ID_BUTTON_LOG_FILE_REF,
      ID_BUTTON_EXECUTE,
    };

  public:
    explicit BatchDialog( Converter& converter );
    ~BatchDialog();

    virtual DWORD GetExtendedStyle( void );

    virtual bool Created( void );

    virtual void EndDialog( int result );

    virtual LRESULT WindowProcedure( UINT msg, WPARAM w_param, LPARAM l_param );

  protected:
    virtual LRESULT WMSizeReceived( int flag, int width, int height );
    virtual LRESULT WMCommandReceived( int id, int code, HWND child );
    virtual LRESULT WMDropFilesReceived( HDROP drop );

  private:
    void AddEntry( const BatchEntry& entry );
    void AddSeparator( void );
    void RepaintList( void );
    void ReadDirectory( bool is_recursive );
    void OpenDirectory( const std::string& path, bool is_recursive );
    BatchEntry MakeEntry( const std::string& filename );
    void ExecuteBatch( void );

  private:
    Converter&              converter_;
    std::vector<BatchEntry> entries_;

    TtCheckBox   no_message_box_check_;
    TtCheckBox   force_convert_check_;
    TtCheckBox   log_file_check_;
    TtEdit       log_file_path_edit_;
    TtButton     log_file_ref_button_;
    TtButton     execute_button_;
    BatchBmsList bms_list_;
    TtButton     up_button_;
    TtButton     down_button_;
    TtButton     delete_button_;
    TtButton     add_button_;

    BatchLogDialog log_dialog_;
  };
}

#endif // BMX2WAV_BATCH_DIALOG_H_
