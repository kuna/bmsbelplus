#ifndef BMX2WAV_CONVERTING_DIALOG_H_
#define BMX2WAV_CONVERTING_DIALOG_H_

#include "tt_thread.h"
#include "tt_time.h"
#include "tt_dialog.h"
#include "tt_window_controls.h"
#include "tt_progress.h"
#include "tt_message_queue.h"

#include "bmx2wav_converter.h"

namespace Bmx2Wav {
  class ConvertingDialog : public TtDialog,
                           public BmsParser::Reactor,
                           public WavMaker::Reactor,
                           public Converter::ProgressReceiver {
  public:
    // -- Thread ---------------------------------------------------------
    class Thread : public TtThread {
    public:
      explicit Thread( ConvertingDialog& dialog, Converter& converter );

      virtual unsigned int Run( void );

    private:
      ConvertingDialog&            dialog_;
      Converter&                   converter_;
    };

    // -- ConvertingDialog -----------------------------------------------
  private:
    enum {
      ID_BUTTON_SHOW_DETAIL_LOG = 10001,
      ID_BUTTON_PLAY,
      ID_BUTTON_ABORT,
    };

  public:
    explicit ConvertingDialog( Converter& converter );
    ~ConvertingDialog();

    void SetAutoClose( bool flag );
    bool GetAutoClose( void ) const;

    std::string GetDetailLogText( void );

    TtThread& GetRunThread( void );

    virtual bool Created( void );

    virtual LRESULT WMCloseReceived( void );
    virtual LRESULT WMCommandReceived( int id, int code, HWND child );

  protected:
    void AbortIfAbortMessageExistsInQueue( void );

    // -- DetailLogEdit --------------------------------------------------
    class DetailLogEdit : public TtWindowWithStyle<TtEditMultiline, WS_VSCROLL> {
    public:
      explicit DetailLogEdit( void );

      DetailLogEdit& operator <<( const std::string& str );
      DetailLogEdit& operator <<( int value );
      DetailLogEdit& operator <<( unsigned int value );
      DetailLogEdit& operator <<( double value );
      DetailLogEdit& operator <<( BmsWord& word );
      enum Period {
        PERIOD
      };
      void operator <<( Period period );
    };

  public:
    // bmx2wav_converting_dialog_reaction.cpp
    // -- Converter::ProgressReceiver ------------------------------------
    virtual void AtConvertStart( void );
    virtual BmsParser::Reactor& GetBmsParserReactor( void );
    virtual void AtBmsParseStart( const std::string& filename );

    // -- BmsParser::Reactor ---------------------------------------------
    virtual std::string AtDuplicateHeader( BmsParser::Parser& parser,
                                           const std::string header,
                                           const std::string before,
                                           const std::string present );
    virtual double AtDuplicateBarChange( BmsParser::Parser& parser,
                                         unsigned int bar,
                                         double       before,
                                         double       present );
    virtual bool AtParseError( BmsParser::Parser& parser, BmsParseException& e, const std::string& line );
    virtual unsigned int AtGenerateRandom( BmsParser::Parser& parser, unsigned int max );

    virtual void AtLineDataRead( BmsParser::Parser& parser );
    virtual void AtLineEnd( BmsParser::Parser& parser );

  private:
    void AtParseErrorIgnored( BmsParseException& e, const std::string& line );
    void AtRandomValueGenerated( BmsParser::Parser& parser, unsigned int max, unsigned int random_value );

    // -- Converter::ProgressReceiver ------------------------------------
  public:
    virtual void AtBmsParseEnd( const std::string& filename, BmsBms& bms );
    virtual WavMaker::Reactor& GetWavMakerReactor( void );

    // -- WavMaker::Reactor ----------------------------------------------
    virtual void AtOneTickRead( void );

    // -- Converter::ProgressReceiver ------------------------------------
    virtual void AtWavFileReadStart( unsigned int file_count );
    virtual void AtOneWavFileReadStart( BmsWord word, const std::string& filename );
    virtual void AtOneWavFileReadEnd( BmsWord word, const std::string& filename );
    virtual bool AtWavFileNotExists( BmsWord word, const std::string& filename, bool wav_was_used );
    virtual bool AtWavFileFormatError( BmsWord word, InvalidWavFileFormatException& e );
    virtual bool AtWavFileError( BmsWord word, WavFileError& e );
    virtual void AtWavFileReadEnd( unsigned int file_count );
    virtual bool AtScrollObjectDefineError( BmsWord word, const std::string& name, const std::string& value );
    virtual void AtMixStart( unsigned int start_position, unsigned int end_position );
    virtual bool AtScrollObjectEntryIsEmpty( BmsWord word, const std::string& name );
    virtual void AtMixProgress( unsigned int current_position );
    virtual void AtMixEnd( unsigned int end_position );
    virtual void AtNormalizeStart( void );
    virtual void AtNormalizeEnd( double change_ratio );
    virtual void AtCompressStart( double threshold, double ratio, unsigned int attack_time, unsigned int release_time, int look_ahead );
    virtual void AtCompressEnd( double threshold, double ratio, unsigned int attack_time, unsigned int release_time, int look_ahead );
    virtual void AtVolumeChangeStart( int percentage );
    virtual void AtVolumeChangeEnd( int percentage );
    virtual void AtWriteToFileStart( const std::string& filename );
    virtual void AtWriteToFileEnd( const std::string& filename );
    virtual void AtAfterProcessStart( TtProcess::CreateInfo& info );
    virtual void AtCreatedProcess( TtProcess& process, TtProcess::CreateInfo& info );
    virtual void AtWaitForProcessStart( TtProcess& process );
    virtual void AtWaitForProcessEnd( TtProcess& process );
    virtual void AtDeleteFileStart( const std::string& filename );
    virtual void AtDeleteFileEnd( const std::string& filename, bool result );
    virtual void AtAfterProcessEnd( TtProcess& process, TtProcess::CreateInfo& info, const std::string& outputed_filename, BmsBms& bms );
    virtual void AtFailStartProcess( TtProcess::CreateInfo& info );
    virtual void AtConvertEnd( void );
    virtual void AtConvertError( TtMessageBoxOk& box );
    virtual void AtConvertAbort( TtMessageBoxOk& box );
    virtual void AtThreadEnd( void );

  protected:
    TtStatic         bms_file_label_;
    TtStatic         bms_title_artist_label_;
    TtStatic         explanation_label_;
    TtStatic         wav_read_label_;
    TtProgressSmooth wav_read_progress_;
    TtStatic         wav_mix_label_;
    TtProgressSmooth wav_mix_progress_;
    TtButton         play_button_;
    TtButton         abort_button_;
    TtButton         show_detail_log_button_;
    DetailLogEdit    detail_log_edit_;

    Converter&          converter_;
    Thread              thread_;
    TtMessageQueue<int> abort_message_queue_;

    TtTime           all_time_;
    TtTime           partial_time_;
    unsigned int     mix_end_position_;

    bool is_converting_;
    bool is_detail_log_showing_;
    bool is_success_;
    bool auto_close_;

    std::string outputed_filename_;
    std::string detail_log_text_;
  };
}

#endif // BMX2WAV_CONVERTING_DIALOG_H_
