#include <sstream>

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_exception.h"


// -- BmsException -------------------------------------------------------
BmsException::BmsException( void ) :
std::domain_error( "BmsException" )
{
}

BmsException::~BmsException()
{
}


// -- BmsInternalException -----------------------------------------------
BmsInternalException::BmsInternalException( const char* file, unsigned int line ) :
file_( file ),
line_( line )
{
}

const char*
BmsInternalException::GetFile( void ) const
{
  return file_;
}

unsigned int
BmsInternalException::GetLine( void ) const
{
  return line_;
}


std::wstring
BmsInternalException::Message( void ) const
{
  std::wstringstream os;
  os << "An error occured while processing command. " << file_ << "(" << line_ << ")";
  return os.str();
}

// -- BmsOutOfRangeAccessException ---------------------------------------
BmsOutOfRangeAccessException::BmsOutOfRangeAccessException( const std::type_info& klass ) :
BmsWithCauseClassException( klass )
{
}

std::wstring
BmsOutOfRangeAccessException::Message( void ) const
{
  std::wstringstream os;
  os << this->GetCauseClass().name() << "  - Tried to access over supporting range of BMS file format.";
  return os.str();
}

// -- BmsInvalidWordValueUsedException -----------------------------------
BmsInvalidWordValueUsedException::BmsInvalidWordValueUsedException( int value ) :
value_( value )
{
}

int
BmsInvalidWordValueUsedException::GetValue( void ) const
{
  return value_;
}

std::wstring
BmsInvalidWordValueUsedException::Message( void ) const
{
  std::wstringstream os;
  os << "オブジェクトを 00 〜 ZZ の範囲外の値にしてから文字列に変換しようとしました : " << value_;
  return os.str();
}

// -- BmsInvalidCharUsedAsWordException ----------------------------------
BmsInvalidCharUsedAsWordException::BmsInvalidCharUsedAsWordException( char msb, char lsb ) :
msb_( msb ),
lsb_( lsb )
{
}

char
BmsInvalidCharUsedAsWordException::GetMsb( void ) const
{
  return msb_;
}

char
BmsInvalidCharUsedAsWordException::GetLsb( void ) const
{
  return lsb_;
}

std::wstring
BmsInvalidCharUsedAsWordException::GetAsString( void ) const
{
  std::wstring tmp;
  tmp.append( msb_, 1 );
  tmp.append( lsb_, 1 );
  return tmp;
}

std::wstring
BmsInvalidCharUsedAsWordException::Message( void ) const
{
  std::wstringstream os;
  os << "0〜9, A〜Z 以外の文字をオブジェクトにしようとしました。文字列 : " << this->GetAsString();
  return os.str();
}

// -- BmsInvalidStringConvertedAsBufferException -------------------------
BmsInvalidStringConvertedAsBufferException::BmsInvalidStringConvertedAsBufferException( const std::wstring& str ) :
str_( str )
{
}

const std::wstring&
BmsInvalidStringConvertedAsBufferException::GetString( void ) const
{
  return str_;
}

std::wstring
BmsInvalidStringConvertedAsBufferException::Message( void ) const
{
  std::wstringstream os;
  os << "オブジェクト配列として不正な文字列をオブジェクト配列に変換しようとしました。文字列 : " << str_;
  return os.str();
}

// -- BmsTooLongStringConvertedAsBufferException --------------------------
BmsTooLongStringConvertedAsBufferException::BmsTooLongStringConvertedAsBufferException( const std::wstring& str ) :
str_( str )
{
}

const std::wstring&
BmsTooLongStringConvertedAsBufferException::GetString( void ) const
{
  return str_;
}

std::wstring
BmsTooLongStringConvertedAsBufferException::Message( void ) const
{
  std::wstringstream os;
  os << "分解能を超えるオブジェクトを持つ文字列をオブジェクト配列に変換しようとしました。文字列 : " << str_;
  return os.str();
}


// -- BmsFileOpenException -----------------------------------------------
BmsFileOpenException::BmsFileOpenException( const std::wstring& filename, unsigned int error_number ) :
filename_( filename ),
error_number_( error_number )
{
}

const std::wstring&
BmsFileOpenException::GetFileName( void ) const
{
  return filename_;
}

unsigned int
BmsFileOpenException::GetErrorNumber( void ) const
{
  return error_number_;
}

std::wstring
BmsFileOpenException::Message( void ) const
{
  std::wstringstream os;
  os << "BMS File open failed. " << filename_ << "(" << error_number_ << ")";
  return os.str();
}

// -- BmsFileNotSupportedEncoding -----------------------------------------------
BmsFileNotSupportedEncoding::BmsFileNotSupportedEncoding(const std::wstring& filename, unsigned int error_number) :
filename_(filename),
error_number_(error_number)
{
}

const std::wstring&
BmsFileNotSupportedEncoding::GetFileName(void) const
{
	return filename_;
}

unsigned int
BmsFileNotSupportedEncoding::GetErrorNumber(void) const
{
	return error_number_;
}

std::wstring
BmsFileNotSupportedEncoding::Message(void) const
{
	std::wstringstream os;
	os << "BMS File loader - unknown encoding. " << filename_ << "(" << error_number_ << ")";
	return os.str();
}

// -- BmsFileAccessException -----------------------------------------------
BmsFileAccessException::BmsFileAccessException( const std::wstring& filename, unsigned int error_number ) :
filename_( filename ),
error_number_( error_number )
{
}

const std::wstring&
BmsFileAccessException::GetFileName( void ) const
{
  return filename_;
}

unsigned int
BmsFileAccessException::GetErrorNumber( void ) const
{
  return error_number_;
}

std::wstring
BmsFileAccessException::Message( void ) const
{
  std::wstringstream os;
  os << "BMS File access failed. " << filename_ << "(" << error_number_ << ")";
  return os.str();
}


// -- BmsDuplicateHeaderException ----------------------------------------
BmsDuplicateHeaderException::BmsDuplicateHeaderException( const std::wstring& header ) :
header_( header )
{
}

const std::wstring&
BmsDuplicateHeaderException::GetHeader( void ) const
{
  return header_;
}

std::wstring
BmsDuplicateHeaderException::Message( void ) const
{
  std::wstringstream os;
  os << "ヘッダが複数回指定されました。ヘッダ : " << header_;
  return os.str();
}


// -- BmsDuplicateBarChangeException ----------------------------------------
BmsDuplicateBarChangeException::BmsDuplicateBarChangeException( unsigned int bar ) :
bar_( bar )
{
}

unsigned int
BmsDuplicateBarChangeException::GetBar( void ) const
{
  return bar_;
}

std::wstring
BmsDuplicateBarChangeException::Message( void ) const
{
  std::wstringstream os;
  os << "小節長変更が複数回指定されました。小節番号 : " << bar_;
  return os.str();
}


// -- BmsParseException --------------------------------------------------
BmsParseException::BmsParseException( unsigned int line ) :
line_( line )
{
}

unsigned int
BmsParseException::GetLine( void ) const
{
  return line_;
}


// -- BmsParseInvalidBarChangeValueException -----------------------------
BmsParseInvalidBarChangeValueException::BmsParseInvalidBarChangeValueException( unsigned int line, const std::wstring& str ) :
BmsParseException( line ),
str_( str )
{
}

const std::wstring&
BmsParseInvalidBarChangeValueException::GetString( void ) const
{
  return str_;
}

std::wstring
BmsParseInvalidBarChangeValueException::Message( void ) const
{
  std::wstringstream os;
  os << "小節長変更の値が不正です。不正な文字列 : " << str_;
  return os.str();
}


// -- BmsParseInvalidEndAsObjectArrayException ---------------------------
BmsParseInvalidEndAsObjectArrayException::BmsParseInvalidEndAsObjectArrayException( unsigned int line ) :
BmsParseException( line )
{
}

std::wstring
BmsParseInvalidEndAsObjectArrayException::Message( void ) const
{
  std::wstringstream os;
  os << "BMS Parse Failed (wrong object/note)";
  return os.str();
}

// -- class BmsParseNoObjectArrayException -------------------------------
BmsParseNoObjectArrayException::BmsParseNoObjectArrayException( unsigned int line ) :
BmsParseException( line )
{
}

std::wstring
BmsParseNoObjectArrayException::Message( void ) const
{
  std::wstringstream os;
  os << "BMS Parse Failed (no object/note exists)";
  return os.str();
}

// -- class BmsParseTooManyObjectException -------------------------------
BmsParseTooManyObjectException::BmsParseTooManyObjectException( unsigned int line ) :
BmsParseException( line )
{
}

std::wstring
BmsParseTooManyObjectException::Message( void ) const
{
  std::wstringstream os;
  os << "Too many object/note exists.";
  return os.str();
}

// -- BmsParseDuplicateHeaderException -----------------------------------
BmsParseDuplicateHeaderException::BmsParseDuplicateHeaderException( unsigned int line, const std::wstring& header ) :
BmsParseException( line ),
header_( header )
{
}

const std::wstring&
BmsParseDuplicateHeaderException::GetHeader( void ) const
{
  return header_;
}

std::wstring
BmsParseDuplicateHeaderException::Message( void ) const
{
  std::wstringstream os;
  os << "ヘッダが複数回指定されました。ヘッダ : " << header_;
  return os.str();
}

// -- BmsParseDuplicateBarChangeException --------------------------------
BmsParseDuplicateBarChangeException::BmsParseDuplicateBarChangeException( unsigned int line, unsigned int bar ) :
BmsParseException( line ),
bar_( bar )
{
}

unsigned int
BmsParseDuplicateBarChangeException::GetBar( void ) const
{
  return bar_;
}

std::wstring
BmsParseDuplicateBarChangeException::Message( void ) const
{
  std::wstringstream os;
  os << "小節長変更が複数回指定されました。小節番号 : " << bar_;
  return os.str();
}

// -- BmsParseInvalidCharException ---------------------------------------
BmsParseInvalidCharException::BmsParseInvalidCharException( unsigned int line, const std::wstring& str ) :
BmsParseException( line ),
str_( str )
{
}

const std::wstring&
BmsParseInvalidCharException::GetString( void ) const
{
  return str_;
}

// -- BmsParseInvalidCharAsHeaderOrBarException --------------------------
BmsParseInvalidCharAsHeaderOrBarException::BmsParseInvalidCharAsHeaderOrBarException( unsigned int line, const std::wstring& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::wstring
BmsParseInvalidCharAsHeaderOrBarException::Message( void ) const
{
  std::wstringstream os;
  os << "ヘッダ名または小節番号として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsHeaderKeyException ----------------------------
BmsParseInvalidCharAsHeaderKeyException::BmsParseInvalidCharAsHeaderKeyException( unsigned int line, const std::wstring& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::wstring
BmsParseInvalidCharAsHeaderKeyException::Message( void ) const
{
  std::wstringstream os;
  os << "ヘッダ名として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsBarException ----------------------------------
BmsParseInvalidCharAsBarException::BmsParseInvalidCharAsBarException( unsigned int line, const std::wstring& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::wstring
BmsParseInvalidCharAsBarException::Message( void ) const
{
  std::wstringstream os;
  os << "小節番号として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsChannelException ------------------------------
BmsParseInvalidCharAsChannelException::BmsParseInvalidCharAsChannelException( unsigned int line, const std::wstring& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::wstring
BmsParseInvalidCharAsChannelException::Message( void ) const
{
  std::wstringstream os;
  os << "チャネル番号として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsColonException --------------------------------
BmsParseInvalidCharAsColonException::BmsParseInvalidCharAsColonException( unsigned int line, const std::wstring& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::wstring
BmsParseInvalidCharAsColonException::Message( void ) const
{
  std::wstringstream os;
  os << "チャネル番号とオブジェクト配列を区切るコロンとして不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsObjectArrayException --------------------------
BmsParseInvalidCharAsObjectArrayException::BmsParseInvalidCharAsObjectArrayException( unsigned int line, const std::wstring& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::wstring
BmsParseInvalidCharAsObjectArrayException::Message( void ) const
{
  std::wstringstream os;
  os << "オブジェクト配列として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidRandomValueException --------------------------------
BmsParseInvalidRandomValueException::BmsParseInvalidRandomValueException( unsigned int line,
                                                                          const std::wstring& key,
                                                                          const std::wstring& value ) :
BmsParseException( line ),
key_( key ),
value_( value )
{
}

const std::wstring&
BmsParseInvalidRandomValueException::GetKey( void ) const
{
  return key_;
}

const std::wstring&
BmsParseInvalidRandomValueException::GetValue( void ) const
{
  return value_;
}

std::wstring
BmsParseInvalidRandomValueException::Message( void ) const
{
  std::wstringstream os;
  os << "ランダム構文 #" << key_ << " で指定された値が不正です。値 : " << value_;
  return os.str();
}

// -- BmsParseUnexceptedEndIfException -----------------------------------
BmsParseUnexceptedEndIfException::BmsParseUnexceptedEndIfException( unsigned int line ) :
BmsParseException( line )
{
}

std::wstring
BmsParseUnexceptedEndIfException::Message( void ) const
{
  std::wstringstream os;
  os << "Wrong #IF ~ #ENDIF clause.";
  return os.str();
}

// -- BmsParseNotFoundEndIfException -------------------------------------
BmsParseNotFoundEndIfException::BmsParseNotFoundEndIfException( unsigned int line )
{
}

std::wstring
BmsParseNotFoundEndIfException::Message( void ) const
{
  std::wstringstream os;
  os << "Cannot find #ENDIF";
  return os.str();
}


// -- LongNoteObjectInvalidEncloseException ------------------------------
BmsLongNoteObjectInvalidEncloseException::BmsLongNoteObjectInvalidEncloseException(const BmsWord& start, const BmsWord& end, unsigned int bar) :
start_(start),
end_(end),
bar_(bar)
{
}

std::wstring
BmsLongNoteObjectInvalidEncloseException::Message(void) const
{
	std::wstringstream ss;
	ss << "Longnote has been closed unexpectedly." << "\r\n" <<
		"Start : " << start_.ToWString() << "\r\n" <<
		"End : " << end_.ToWString() << "\r\n" <<
		"End Measure : " << bar_;
	return ss.str();
}

const BmsWord&
BmsLongNoteObjectInvalidEncloseException::GetStartWord(void) const
{
	return start_;
}

const BmsWord&
BmsLongNoteObjectInvalidEncloseException::GetEndWord(void) const
{
	return end_;
}

unsigned int
BmsLongNoteObjectInvalidEncloseException::GetBar(void) const
{
	return bar_;
}

// -- LongNoteObjectNotEnclosedException ---------------------------------
BmsLongNoteObjectNotEnclosedException::BmsLongNoteObjectNotEnclosedException(void)
{
}

std::wstring
BmsLongNoteObjectNotEnclosedException::Message(void) const
{
	return L"Longnote had not been enclosed properly.";
}

// -- InvalidFormatAsBpmChangeValueException ---------------------------------
InvalidFormatAsBpmChangeValueException::InvalidFormatAsBpmChangeValueException(const BmsWord& word)
	: word(word)
{
}

std::wstring
InvalidFormatAsBpmChangeValueException::Message(void) const
{
	std::wstringstream ss;
	ss << "Wrong BPM Value found on channel #03: " << word.ToWString();
	return ss.str();
}

// -- InvalidFormatAsExtendedBpmChangeValueException ---------------------------------
InvalidFormatAsExtendedBpmChangeValueException::InvalidFormatAsExtendedBpmChangeValueException(const BmsWord& word, const std::wstring& value)
	: word(word)
{
}

std::wstring
InvalidFormatAsExtendedBpmChangeValueException::Message(void) const
{
	std::wstringstream ss;
	ss << "Wrong Extended BPM Value found: " << word.ToWString() << " = " << value;
	return ss.str();
}

// -- InvalidFormatAsBpmChangeValueException ---------------------------------
InvalidFormatAsStopSequenceException::InvalidFormatAsStopSequenceException(const BmsWord& word, const std::wstring& value)
	: word(word)
{
}

std::wstring
InvalidFormatAsStopSequenceException::Message(void) const
{
	std::wstringstream ss;
	ss << "Wrong STOP Value found: " << word.ToWString() << " = " << value;
	return ss.str();
}

// -- ExtendedBpmChangeEntryNotExistException ---------------------------------
ExtendedBpmChangeEntryNotExistException::ExtendedBpmChangeEntryNotExistException(const BmsWord& word)
	: word(word)
{
}

std::wstring
ExtendedBpmChangeEntryNotExistException::Message(void) const
{
	std::wstringstream ss;
	ss << "Cannot find Extended BPM on channel #08: " << word.ToWString();
	return ss.str();
}

// -- StopSequenceEntryNotExistException ---------------------------------
StopSequenceEntryNotExistException::StopSequenceEntryNotExistException(const BmsWord& word)
	: word(word)
{
}

std::wstring
StopSequenceEntryNotExistException::Message(void) const
{
	std::wstringstream ss;
	ss << "Cannot find STOP sequence on channel #09: " << word.ToWString();
	return ss.str();
}

// -- InvalidFormatAsBpmHeaderException ---------------------------------
InvalidFormatAsBpmHeaderException::InvalidFormatAsBpmHeaderException()
{
}

std::wstring
InvalidFormatAsBpmHeaderException::Message(void) const
{
	return L"Base BPM is invalid value. cannot be converted to float.";
}

// -- InvalidLongNoteType ---------------------------------
InvalidLongNoteType::InvalidLongNoteType(const std::wstring& value)
	: value(value)
{
}

std::wstring
InvalidLongNoteType::Message(void) const
{
	return L"#LNTYPE is invalid: " + value;
}

// -- UnsupportedLongNoteType ---------------------------------
UnsupportedLongNoteType::UnsupportedLongNoteType(const std::wstring& value)
	: value(value)
{
}

std::wstring
UnsupportedLongNoteType::Message(void) const
{
	return L"Unsupported #LNTYPE value: " + value;
}

// -- UnknownChannel ---------------------------------
UnknownChannelException::UnknownChannelException(const BmsWord& word)
	: word(word)
{
}

std::wstring
UnknownChannelException::Message(void) const
{
	return L"Unknown channel: " + word.ToWString();
}

// -- BmsTimeWrongException ---------------------------------
BmsTimeWrongException::BmsTimeWrongException()
{
}

std::wstring
BmsTimeWrongException::Message(void) const
{
	return L"Less time then previous one cannot be inserted into BmsTimeManager.";
}