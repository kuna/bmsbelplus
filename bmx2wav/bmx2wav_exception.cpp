#include "tt_util.h"

#include "bmx2wav_exception.h"

using namespace Bmx2Wav;


// -- Bmx2WavException ---------------------------------------------------
Bmx2WavException::Bmx2WavException( void )
{
}

// -- Bmx2WavInternalException -------------------------------------------
Bmx2WavInternalException::Bmx2WavInternalException( const char* file, unsigned int line ) :
file_( file ),
line_( line )
{
}

const char*
Bmx2WavInternalException::GetFile( void ) const
{
  return file_;
}

unsigned int
Bmx2WavInternalException::GetLine( void ) const
{
  return line_;
}

std::string
Bmx2WavInternalException::BasicMessage( void )
{
  return "内部エラーが発生しました。";
}

std::string
Bmx2WavInternalException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ファイル : " << file_ << "\r\n" <<
    "行番号 : " << line_ );
  return tmp;
}

// -- Bmx2WavConvertException --------------------------------------------
Bmx2WavConvertException::Bmx2WavConvertException( void )
{
}


// -- WavFileError -------------------------------------------------------
WavFileError::WavFileError( const std::string& filename ) :
WithFileNameException( filename )
{
}


// -- EntriedWavFileNotFoundException ------------------------------------
EntriedWavFileNotFoundException::EntriedWavFileNotFoundException( BmsWord number, const std::string& filename ) :
WavFileError( filename ),
WithWordException( number )
{
}

std::string
EntriedWavFileNotFoundException::BasicMessage( void )
{
  return "指定された WAV ファイルが存在しません。";
}

std::string
EntriedWavFileNotFoundException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ヘッダ : WAV" << this->GetNumber() << "\r\n" <<
    "ファイル名 : " << this->GetFileName() );
  return tmp;
}

BmsWord
EntriedWavFileNotFoundException::GetNumber( void ) const
{
  return this->GetWord();
}


// -- InvalidWavFileFormatException --------------------------------------
InvalidWavFileFormatException::InvalidWavFileFormatException( const std::string& filename, const std::string& reason ) :
WavFileError( filename ),
reason_( reason )
{
}

std::string
InvalidWavFileFormatException::BasicMessage( void )
{
  return "WAV ファイルの形式が不正です。";
}

std::string
InvalidWavFileFormatException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ファイル : " << this->GetFileName() << "\r\n" <<
    "理由 : " << reason_ );
  return tmp;
}

const std::string&
InvalidWavFileFormatException::GetReason( void ) const
{
  return reason_;
}

// -- UnsupportedWavFileFormatException ----------------------------------
UnsupportedWavFileFormatException::UnsupportedWavFileFormatException( const std::string& filename, const std::string& reason ) :
WavFileError( filename ),
reason_( reason )
{
}

std::string
UnsupportedWavFileFormatException::BasicMessage( void )
{
  return "サポートしていない WAV ファイルの形式です。";
}

std::string
UnsupportedWavFileFormatException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ファイル : " << this->GetFileName() << "\r\n" <<
    "理由 : " << reason_ );
  return tmp;
}

const std::string&
UnsupportedWavFileFormatException::GetReason( void ) const
{
  return reason_;
}

// -- WavFileReadException -----------------------------------------------
WavFileReadException::WavFileReadException( const std::string& filename ) :
WavFileError( filename )
{
}

std::string
WavFileReadException::BasicMessage( void )
{
  return "WAV ファイルの読み込みに失敗しました。";
}

std::string
WavFileReadException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ファイル : " << this->GetFileName() );
  return tmp;
}

// -- WavFileWriteException ----------------------------------------------
WavFileWriteException::WavFileWriteException( const std::string& filename ) :
WavFileError( filename )
{
}

std::string
WavFileWriteException::BasicMessage( void )
{
  return "WAV ファイルの書き込みに失敗しました。";
}

std::string
WavFileWriteException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ファイル : " << this->GetFileName() );
  return tmp;
}

// -- OggFileOpenException -----------------------------------------------
OggFileOpenException::OggFileOpenException( const std::string& filename ) :
WavFileError( filename )
{
}

std::string
OggFileOpenException::BasicMessage( void )
{
  return "ogg ファイルを開くのに失敗しました。";
}

std::string
OggFileOpenException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ファイル : " << this->GetFileName() );
  return tmp;
}

// -- OggFileReadException -----------------------------------------------
OggFileReadException::OggFileReadException( const std::string& filename ) :
WavFileError( filename )
{
}

std::string
OggFileReadException::BasicMessage( void )
{
  return "ogg ファイルの読み込みに失敗しました。";
}

std::string
OggFileReadException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ファイル : " << this->GetFileName() );
  return tmp;
}


// -- EntriedBmpFileNotFoundException ------------------------------------
EntriedBmpFileNotFoundException::EntriedBmpFileNotFoundException( BmsWord number, const std::string& filename ) :
WithFileNameException( filename ),
WithWordException( number )
{
}

std::string
EntriedBmpFileNotFoundException::BasicMessage( void )
{
  return "指定された BMP ファイルが存在しません。";
}

std::string
EntriedBmpFileNotFoundException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ヘッダ : BMP" << this->GetNumber() << "\r\n" <<
    "ファイル名 : " << this->GetFileName() );
  return tmp;
}

BmsWord
EntriedBmpFileNotFoundException::GetNumber( void ) const
{
  return this->GetWord();
}


// -- EntriedRegistArrayValueNotUsedException --------------------------
EntriedRegistArrayValueNotUsedException::EntriedRegistArrayValueNotUsedException( const std::string& name, BmsWord number ) :
WithWordException( number ),
name_( name )
{
}

std::string
EntriedRegistArrayValueNotUsedException::BasicMessage( void )
{
  return "ヘッダで指定された定義が譜面中に使用されていません。";
}

std::string
EntriedRegistArrayValueNotUsedException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ヘッダ : " << this->GetName() << this->GetNumber() );
  return tmp;
}

const std::string&
EntriedRegistArrayValueNotUsedException::GetName( void ) const
{
  return name_;
}

BmsWord
EntriedRegistArrayValueNotUsedException::GetNumber( void ) const
{
  return this->GetWord();
}

// -- LongNoteObjectInvalidEncloseException ------------------------------
LongNoteObjectInvalidEncloseException::LongNoteObjectInvalidEncloseException( BmsWord start, BmsWord end, unsigned int bar ) :
start_( start ),
end_( end ),
bar_( bar )
{
}

std::string
LongNoteObjectInvalidEncloseException::BasicMessage( void )
{
  return "ロングノートのオブジェクトの終端が不正です。";
}
std::string
LongNoteObjectInvalidEncloseException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "開始 : " << start_ << "\r\n" <<
    "終端 : " << end_ << "\r\n" <<
    "終端の小節番号 : " << bar_ );
  return tmp;
}

BmsWord
LongNoteObjectInvalidEncloseException::GetStartWord( void ) const
{
  return start_;
}

BmsWord
LongNoteObjectInvalidEncloseException::GetEndWord( void ) const
{
  return end_;
}

unsigned int
LongNoteObjectInvalidEncloseException::GetBar( void ) const
{
  return bar_;
}

// -- LongNoteObjectNotEnclosedException ---------------------------------
LongNoteObjectNotEnclosedException::LongNoteObjectNotEnclosedException( void )
{
}

std::string
LongNoteObjectNotEnclosedException::BasicMessage( void )
{
  return "ロングノートの終端が存在しません。";
}

std::string
LongNoteObjectNotEnclosedException::Message( void )
{
  return this->BasicMessage();
}

// -- InvalidFormatAsBpmHeaderException ----------------------------------
InvalidFormatAsBpmHeaderException::InvalidFormatAsBpmHeaderException( void )
{
}

std::string
InvalidFormatAsBpmHeaderException::BasicMessage( void )
{
  return "指定された BPM の表記が不正です。";
}

std::string
InvalidFormatAsBpmHeaderException::Message( void )
{
  return this->BasicMessage();
}

// -- NotEntriedWavWasUsedException --------------------------------------
NotEntriedWavWasUsedException::NotEntriedWavWasUsedException( BmsWord number ) :
WithWordException( number )
{
}

std::string
NotEntriedWavWasUsedException::BasicMessage( void )
{
  return "WAV ファイルとして指定されていないオブジェクトが使用されました。";
}

std::string
NotEntriedWavWasUsedException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "オブジェクト : " << this->GetNumber() );
  return tmp;
}

BmsWord
NotEntriedWavWasUsedException::GetNumber( void ) const
{
  return this->GetWord();
}

// -- InvalidFormatAsBpmChangeValueException -----------------------------
InvalidFormatAsBpmChangeValueException::InvalidFormatAsBpmChangeValueException( BmsWord word ) :
WithWordException( word )
{
}

std::string
InvalidFormatAsBpmChangeValueException::BasicMessage( void )
{
  return "通常の BPM 変更に使用されたオブジェクトが不正です。";
}

std::string
InvalidFormatAsBpmChangeValueException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "オブジェクト : " << this->GetWord() );
  return tmp;
}

// -- ExtendedBpmChangeEntryNotExistException ----------------------------
ExtendedBpmChangeEntryNotExistException::ExtendedBpmChangeEntryNotExistException( BmsWord number ) :
WithWordException( number )
{
}

std::string
ExtendedBpmChangeEntryNotExistException::BasicMessage( void )
{
  return "拡張 BPM 変更で未指定のヘッダが使用されました。";
}

std::string
ExtendedBpmChangeEntryNotExistException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "オブジェクト : " << this->GetNumber() );
  return tmp;
}

BmsWord
ExtendedBpmChangeEntryNotExistException::GetNumber( void ) const
{
  return this->GetWord();
}

// -- InvalidFormatAsExtendedBpmChangeValueException ---------------------
InvalidFormatAsExtendedBpmChangeValueException::InvalidFormatAsExtendedBpmChangeValueException(
  BmsWord number, const std::string& value ) :
WithWordException( number ),
value_( value )
{
}

std::string
InvalidFormatAsExtendedBpmChangeValueException::BasicMessage( void )
{
  return "拡張 BPM 変更で指定した値が不正です。";
}

std::string
InvalidFormatAsExtendedBpmChangeValueException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ヘッダ : BPM" << this->GetWord() << "\r\n" <<
    "値 : " << value_ );
  return tmp;
}

BmsWord
InvalidFormatAsExtendedBpmChangeValueException::GetNumber( void ) const
{
  return this->GetWord();
}

const std::string&
InvalidFormatAsExtendedBpmChangeValueException::GetValue( void ) const
{
  return value_;
}

// -- StopSequenceEntryNotExistException ---------------------------------
StopSequenceEntryNotExistException::StopSequenceEntryNotExistException( BmsWord number ) :
WithWordException( number )
{
}

std::string
StopSequenceEntryNotExistException::BasicMessage( void )
{
  return "ストップシーケンスで未指定のヘッダが使用されました。";
}
std::string
StopSequenceEntryNotExistException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "オブジェクト : " << this->GetNumber() );
  return tmp;
}

BmsWord
StopSequenceEntryNotExistException::GetNumber( void ) const
{
  return this->GetWord();
}


// -- InvalidFormatAsStopSequenceException -------------------------------
InvalidFormatAsStopSequenceException::InvalidFormatAsStopSequenceException( BmsWord number, const std::string& value ) :
WithWordException( number ),
value_( value )
{
}

std::string
InvalidFormatAsStopSequenceException::BasicMessage( void )
{
  return "ストップシーケンスで指定した値が不正です。";
}

std::string
InvalidFormatAsStopSequenceException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ヘッダ : STOP" << this->GetWord() << "\r\n" <<
    "値 : " << value_ );
  return tmp;
}

const std::string&
InvalidFormatAsStopSequenceException::GetValue( void ) const
{
  return value_;
}


// -- OutputFileIsInputWavFileException ----------------------------------
OutputFileIsInputWavFileException::OutputFileIsInputWavFileException( BmsWord number, const std::string& filename ) :
WithWordException( number ),
WithFileNameException( filename )
{
}

std::string
OutputFileIsInputWavFileException::BasicMessage( void )
{
  return "出力ファイルが BMS ファイルの WAV で指定されているファイルです。";
}

std::string
OutputFileIsInputWavFileException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ヘッダ : WAV" << this->GetNumber() << "\r\n" <<
    "ファイル名 : " << this->GetFileName() );
  return tmp;
}

BmsWord
OutputFileIsInputWavFileException::GetNumber( void ) const
{
  return this->GetWord();
}

// -- ConvertAbortException --------------------------------------------
ConvertAbortException::ConvertAbortException( void )
{
}

std::string
ConvertAbortException::BasicMessage( void )
{
  return "変換は中止されました。";
}

std::string
ConvertAbortException::Message( void )
{
  return this->BasicMessage();
}

// -- ShoudPlayObjectNotExistsException ----------------------------------
ShoudPlayObjectNotExistsException::ShoudPlayObjectNotExistsException( void )
{
}

std::string
ShoudPlayObjectNotExistsException::BasicMessage( void )
{
  return "この BMS には音を鳴らすべきオブジェクトが存在していません。";
}

std::string
ShoudPlayObjectNotExistsException::Message( void )
{
  return this->BasicMessage();
}

// -- EntriedStageFileNotFoundException ----------------------------------
EntriedStageFileNotFoundException::EntriedStageFileNotFoundException( const std::string& filename ) :
WithFileNameException( filename )
{
}

std::string
EntriedStageFileNotFoundException::BasicMessage( void )
{
  return "指定された STAGEFILE ファイルが存在しません。";
}

std::string
EntriedStageFileNotFoundException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "ファイル名 : " << this->GetFileName() );
  return tmp;
}

// -- StartProcessException ----------------------------------------------
StartProcessException::StartProcessException( const std::string& filename,
                                              const std::string& arguments,
                                              const std::string& system_message ) :
WithFileNameException( filename ),
arguments_( arguments ),
system_message_( system_message )
{
}

const std::string&
StartProcessException::GetExecuteFileName( void ) const
{
  return this->GetFileName();
}

const std::string&
StartProcessException::GetArguments( void ) const
{
  return arguments_;
}

const std::string&
StartProcessException::GetSystemMessage( void ) const
{
  return system_message_;
}


std::string
StartProcessException::BasicMessage( void )
{
  return "プログラム実行に失敗しました。";
}

std::string
StartProcessException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "実行ファイル名 : " << this->GetExecuteFileName() << "\r\n" <<
    "引数 : " << arguments_ << "\r\n" <<
    "システムメッセージ : " << system_message_ );
  return tmp;
}

// -- InvalidConvertBarException -----------------------------------------
InvalidConvertBarException::InvalidConvertBarException( unsigned int start, unsigned int end ) :
start_( start ),
end_( end )
{
}

std::string
InvalidConvertBarException::BasicMessage( void )
{
  return "指定した演奏範囲が不正です。";
}


std::string 
InvalidConvertBarException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "演奏開始小節番号 : " << this->GetStartBar() << "\r\n"
    "演奏終了小節番号 : " << this->GetEndBar() );
  return tmp;
}

unsigned int
InvalidConvertBarException::GetStartBar( void ) const
{
  return start_;
}

unsigned int
InvalidConvertBarException::GetEndBar( void ) const
{
  return end_;
}

// -- InvalidTrimingBarException -----------------------------------------
InvalidTrimingBarException::InvalidTrimingBarException( unsigned int start, unsigned int end ) :
start_( start ),
end_( end )
{
}

std::string
InvalidTrimingBarException::BasicMessage( void )
{
  return "指定した切り出し範囲が不正です。";
}

std::string
InvalidTrimingBarException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtUtil::StringAppender( tmp ) << "\r\n" <<
    "切り出し開始小節番号 : " << this->GetStartBar() << "\r\n"
    "切り出し終了小節番号 : " << this->GetEndBar() );
  return tmp;
}

unsigned int
InvalidTrimingBarException::GetStartBar( void ) const
{
  return start_;
}

unsigned int
InvalidTrimingBarException::GetEndBar( void ) const
{
  return end_;
}
