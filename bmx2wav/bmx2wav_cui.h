#ifndef BMX2WAV_CUI_H_
#define BMX2WAV_CUI_H_

#include "tt_time.h"
#include "tt_message_box.h"

#include "bmx2wav_converter.h"

namespace Bmx2Wav {
  namespace Console {
    int Main( int argc, char* argv[] );
    void OutputVersion( void );
    void OutputUsage( void );

    // -- CountProgressCalculator ----------------------------------------
    class CountProgressCalculator {
    public:
      void Initialize( unsigned int start, unsigned int end, unsigned int width );
      unsigned int Add( unsigned int x );
      unsigned int Calc( unsigned int a );

      unsigned int start_;
      unsigned int end_;
      unsigned int width_;
      unsigned int current_;
    };

    // -- RangeProgressCalculator ----------------------------------------
    class RangeProgressCalculator {
    public:
      void Initialize( unsigned int start, unsigned int end, unsigned int width );
      unsigned int DoneAt( unsigned int x );
      unsigned int Calc( unsigned int a );

      unsigned int start_;
      unsigned int end_;
      unsigned int width_;
      unsigned int current_;
    };

    // -- Outputer -------------------------------------------------------
    class Outputer : public Converter::ProgressReceiver {
    protected:
      explicit Outputer( Converter& converter, FILE* stream );

    protected:
      Converter& converter_;

      // -- StreamOut ---------------------------------------------
      class StreamOut {
      public:
        explicit StreamOut( FILE* stream );

        StreamOut& operator <<( const std::string& str );
        StreamOut& operator <<( int value );
        StreamOut& operator <<( unsigned int value );
        StreamOut& operator <<( double value );
        StreamOut& operator <<( BmsWord& word );
        enum Period {
          PERIOD
        };
        void operator <<( Period period );

      private:
        FILE* stream_;
      } stream_out_;

      TtTime all_time_;
      TtTime partial_time_;
    };

    // -- NormalOutputer -----------------------------------------------
    class NormalOutputer : public Outputer {
    public:
      explicit NormalOutputer( Converter& converter );

      virtual void AtConvertStart( void );
      virtual void AtBmsParseStart( const std::string& filename );
      virtual void AtBmsParseEnd( const std::string& filename, BmsBms& bms );
      virtual void AtWavFileReadStart( unsigned int file_count );
      virtual void AtOneWavFileReadStart( BmsWord word, const std::string& filename );
      virtual void AtOneWavFileReadEnd( BmsWord word, const std::string& filename );
      virtual bool AtWavFileFormatError( BmsWord word, InvalidWavFileFormatException& e );
      virtual void AtWavFileReadEnd( unsigned int file_count );
      virtual void AtMixStart( unsigned int start_position, unsigned int end_position );
      virtual void AtMixProgress( unsigned int current_position );
      virtual void AtMixEnd( unsigned int end_position );
      virtual void AtNormalizeStart( void );
      virtual void AtNormalizeEnd( double change_ratio );
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

    private:
      CountProgressCalculator wav_read_progress_calculator_;
      RangeProgressCalculator mix_progress_calculator_;
    };

    // -- DetailOutputer -------------------------------------------------
    class DetailOutputer : public Outputer, public BmsParser::Reactor {
    public:
      explicit DetailOutputer( Converter& converter );

      virtual void AtConvertStart( void );
      virtual BmsParser::Reactor& GetBmsParserReactor( void );

    private:
      void AtRandomValueGenerated( BmsParser::Parser& parser, unsigned int max, unsigned int random_value );

    public:
      virtual void AtBmsParseEnd( const std::string& filename, BmsBms& bms );
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
    };
  }
}

#endif // BMX2WAV_CUI_H_
