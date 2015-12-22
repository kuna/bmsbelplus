#ifndef BMX2WAV_EXCEPTION_H_
#define BMX2WAV_EXCEPTION_H_

#include <string>

#include "tt_exception.h"

#include "bms_word.h"

namespace Bmx2Wav {

  // 多重継承用
  // BmsWord 保持用
  class WithWordException {
  protected:
    explicit WithWordException( BmsWord word ) : word_( word ) {}
    BmsWord GetWord( void ) const { return word_; }
  private:
    BmsWord word_;
  };

  // 多重継承用
  // FileName 保持用
  class WithFileNameException {
  protected:
    explicit WithFileNameException( const std::string& filename ) : filename_( filename ) {}
  public:
    const std::string& GetFileName( void ) const { return filename_; }
  private:
    const std::string filename_;
  };

  // -- Bmx2WavException -------------------------------------------------
  class Bmx2WavException : public TtException {
  public:
    explicit Bmx2WavException( void );

    virtual std::string BasicMessage( void ) = 0;
    virtual std::string Message( void ) = 0;
  };

  // -- Bmx2WavInternalException -----------------------------------------
  class Bmx2WavInternalException : public Bmx2WavException {
  public:
    explicit Bmx2WavInternalException( const char* file, unsigned int line );

    const char*  GetFile( void ) const;
    unsigned int GetLine( void ) const;

    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

  private:
    const char*  file_;
    unsigned int line_;
  };
#define BMX2WAV_INTERNAL_EXCEPTION Bmx2WavInternalException( __FILE__, __LINE__ )

  // -- Bmx2WavConvertException ------------------------------------------
  class Bmx2WavConvertException : public Bmx2WavException {
  public:
    explicit Bmx2WavConvertException( void );
  };

  // -- WavFileError -----------------------------------------------------
  class WavFileError : public Bmx2WavConvertException,
                       public WithFileNameException {
  public:
    explicit WavFileError( const std::string& filename );
  };


  // -- EntriedWavFileNotFoundException ----------------------------------
  class EntriedWavFileNotFoundException : public WavFileError,
                                          public WithWordException {
  public:
    explicit EntriedWavFileNotFoundException( BmsWord number, const std::string& filename );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord GetNumber( void ) const;
  };


  // -- InvalidWavFileFormatException ------------------------------------
  class InvalidWavFileFormatException : public WavFileError {
  public:
    explicit InvalidWavFileFormatException( const std::string& filename, const std::string& reason );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    const std::string& GetReason( void ) const;

  private:
    std::string reason_;
  };

  // -- UnsupportedWavFileFormatException ------------------------------------
  class UnsupportedWavFileFormatException : public WavFileError {
  public:
    explicit UnsupportedWavFileFormatException( const std::string& filename, const std::string& reason );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    const std::string& GetReason( void ) const;

  private:
    std::string reason_;
  };

  // -- WavFileReadException ---------------------------------------------
  class WavFileReadException : public WavFileError {
  public:
    explicit WavFileReadException( const std::string& filename );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- WavFileWriteException --------------------------------------------
  class WavFileWriteException : public WavFileError {
  public:
    explicit WavFileWriteException( const std::string& filename );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- OggFileOpenException ---------------------------------------------
  class OggFileOpenException : public WavFileError {
  public:
    explicit OggFileOpenException( const std::string& filename );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- OggFileReadException ---------------------------------------------
  class OggFileReadException : public WavFileError {
  public:
    explicit OggFileReadException( const std::string& filename );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };


  // -- EntriedBmpFileNotFoundException ----------------------------------
  class EntriedBmpFileNotFoundException : public Bmx2WavConvertException,
                                          public WithFileNameException,
                                          public WithWordException {
  public:
    explicit EntriedBmpFileNotFoundException( BmsWord number, const std::string& filename );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord GetNumber( void ) const;
  };

  // -- EntriedRegistArrayValueNotUsedException --------------------------
  class EntriedRegistArrayValueNotUsedException : public Bmx2WavConvertException,
                                                  public WithWordException {
  public:
    explicit EntriedRegistArrayValueNotUsedException( const std::string& name, BmsWord number );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    const std::string& GetName( void ) const;
    BmsWord GetNumber( void ) const;

  private:
    const std::string name_;
  };


  // -- LongNoteObjectInvalidEncloseException ----------------------------
  class LongNoteObjectInvalidEncloseException : public Bmx2WavConvertException {
  public:
    explicit LongNoteObjectInvalidEncloseException( BmsWord start, BmsWord end, unsigned int bar );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord      GetStartWord( void ) const;
    BmsWord      GetEndWord( void ) const;
    unsigned int GetBar( void ) const;

  private:
    BmsWord start_;
    BmsWord end_;
    unsigned int bar_;
  };

  // -- LongNoteObjectNotEnclosedException -------------------------------
  class LongNoteObjectNotEnclosedException : public Bmx2WavConvertException {
  public:
    explicit LongNoteObjectNotEnclosedException( void );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- InvalidFormatAsBpmHeaderException --------------------------------
  class InvalidFormatAsBpmHeaderException : public Bmx2WavConvertException {
  public:
    explicit InvalidFormatAsBpmHeaderException( void );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- NotEntriedWavWasUsedException ------------------------------------
  class NotEntriedWavWasUsedException : public Bmx2WavConvertException,
                                        public WithWordException {
  public:
    explicit NotEntriedWavWasUsedException( BmsWord number );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord GetNumber( void ) const;
  };

  // -- InvalidFormatAsBpmChangeValueException ---------------------------
  class InvalidFormatAsBpmChangeValueException : public Bmx2WavConvertException,
                                                 public WithWordException {
  public:
    explicit InvalidFormatAsBpmChangeValueException( BmsWord word );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- ExtendedBpmChangeEntryNotExistException --------------------------
  class ExtendedBpmChangeEntryNotExistException : public Bmx2WavConvertException,
                                                  public WithWordException {
  public:
    explicit ExtendedBpmChangeEntryNotExistException( BmsWord number );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord GetNumber( void ) const;
  };

  // -- InvalidFormatAsExtendedBpmChangeValueException -------------------
  class InvalidFormatAsExtendedBpmChangeValueException : public Bmx2WavConvertException,
                                                         public WithWordException {
  public:
    explicit InvalidFormatAsExtendedBpmChangeValueException(
      BmsWord number, const std::string& value );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord GetNumber( void ) const;
    const std::string& GetValue( void ) const;

  private:
    std::string value_;
  };

  // -- StopSequenceEntryNotExistException -------------------------------
  class StopSequenceEntryNotExistException : public Bmx2WavConvertException,
                                             public WithWordException {
  public:
    explicit StopSequenceEntryNotExistException( BmsWord number );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord GetNumber( void ) const;
  };

  // -- InvalidFormatAsStopSequenceException -----------------------------
  class InvalidFormatAsStopSequenceException : public Bmx2WavConvertException,
                                               public WithWordException {
  public:
    explicit InvalidFormatAsStopSequenceException( BmsWord number, const std::string& value );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord GetNumber( void ) const;
    const std::string& GetValue( void ) const;

  private:
    std::string value_;
  };

  // -- OutputFileIsInputWavFileException --------------------------------
  class OutputFileIsInputWavFileException : public Bmx2WavConvertException,
                                            public WithWordException,
                                            public WithFileNameException {
  public:
    explicit OutputFileIsInputWavFileException( BmsWord number, const std::string& filename );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    BmsWord GetNumber( void ) const;
  };

  // -- ConvertAbortException --------------------------------------------
  class ConvertAbortException : public Bmx2WavConvertException {
  public:
    explicit ConvertAbortException( void );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- ShoudPlayObjectNotExistsException --------------------------------
  class ShoudPlayObjectNotExistsException : public Bmx2WavConvertException {
  public:
    explicit ShoudPlayObjectNotExistsException( void );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- EntriedStageFileNotFoundException --------------------------------
  class EntriedStageFileNotFoundException : public Bmx2WavConvertException,
                                            public WithFileNameException {
  public:
    explicit EntriedStageFileNotFoundException( const std::string& filename );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );
  };

  // -- StartProcessException --------------------------------------------
  class StartProcessException : public Bmx2WavConvertException,
                                public WithFileNameException {
  public:
    explicit StartProcessException( const std::string& filename,
                                    const std::string& arguments,
                                    const std::string& system_message );
    const std::string& GetExecuteFileName( void ) const;
    const std::string& GetArguments( void ) const;
    const std::string& GetSystemMessage( void ) const;

    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

  private:
    const std::string arguments_;
    const std::string system_message_;
  };

  // -- InvalidConvertBarException ---------------------------------------
  class InvalidConvertBarException : public Bmx2WavConvertException {
  public:
    explicit InvalidConvertBarException( unsigned int start, unsigned int end );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    unsigned int GetStartBar( void ) const;
    unsigned int GetEndBar( void ) const;

  private:
    unsigned int start_;
    unsigned int end_;
  };
  
  // -- InvalidTrimingBarException ---------------------------------------
  class InvalidTrimingBarException : public Bmx2WavConvertException {
  public:
    explicit InvalidTrimingBarException( unsigned int start, unsigned int end );
    virtual std::string BasicMessage( void );
    virtual std::string Message( void );

    unsigned int GetStartBar( void ) const;
    unsigned int GetEndBar( void ) const;

  private:
    unsigned int start_;
    unsigned int end_;
  };

}

#endif // BMX2WAV_EXCEPTION_H_
