#ifndef BMX2WAV_CONVERTER_H_
#define BMX2WAV_CONVERTER_H_

#include <map>

#include "tt_form.h"
#include "tt_process.h"

#include "bms_bms.h"
#include "bms_parser.h"

#include "bmx2wav_wav.h"
#include "bmx2wav_wav_maker.h"
#include "bmx2wav_converter_parameter.h"
#include "bmx2wav_exception.h"


namespace Bmx2Wav {
  class Converter {
  public:
    // -- ProgressReceiver -----------------------------------------------
    class ProgressReceiver {
    public:
      static ProgressReceiver DefaultObject;

    public:
      explicit ProgressReceiver( void );
      virtual ~ProgressReceiver();

      virtual void AtConvertStart( void );
      virtual BmsParser::Reactor& GetBmsParserReactor( void );
      virtual void AtBmsParseStart( const std::string& filename );
      virtual void AtBmsParseEnd( const std::string& filename, BmsBms& bms );
      virtual WavMaker::Reactor& GetWavMakerReactor( void );
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
    };

    // -- Converter ------------------------------------------------------
  public:
    ParameterSet& GetParameterSet( void );

    void SetProgressReceiver( ProgressReceiver* receiver );

    void Convert( void );

  private:
    ParameterSet      parameter_set_;
    ProgressReceiver* receiver_;
  };

}

#endif // BMX2WAV_CONVERTER_H_
