#include <sstream>

#include "bmsbel/bms_define.h"
#include "bmsbel/bms_exception.h"


// -- BmsException -------------------------------------------------------
BmsException::BmsException( void )
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


std::string
BmsInternalException::Message( void ) const
{
  std::stringstream os;
  os << "An error occured while processing command. " << file_ << "(" << line_ << ")";
  return os.str();
}

// -- BmsOutOfRangeAccessException ---------------------------------------
BmsOutOfRangeAccessException::BmsOutOfRangeAccessException( const std::type_info& klass ) :
BmsWithCauseClassException( klass )
{
}

std::string
BmsOutOfRangeAccessException::Message( void ) const
{
  std::stringstream os;
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

std::string
BmsInvalidWordValueUsedException::Message( void ) const
{
  std::stringstream os;
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

std::string
BmsInvalidCharUsedAsWordException::GetAsString( void ) const
{
  std::string tmp;
  tmp.append( msb_, 1 );
  tmp.append( lsb_, 1 );
  return tmp;
}

std::string
BmsInvalidCharUsedAsWordException::Message( void ) const
{
  std::stringstream os;
  os << "0〜9, A〜Z 以外の文字をオブジェクトにしようとしました。文字列 : " << this->GetAsString();
  return os.str();
}

// -- BmsInvalidStringConvertedAsBufferException -------------------------
BmsInvalidStringConvertedAsBufferException::BmsInvalidStringConvertedAsBufferException( const std::string& str ) :
str_( str )
{
}

const std::string&
BmsInvalidStringConvertedAsBufferException::GetString( void ) const
{
  return str_;
}

std::string
BmsInvalidStringConvertedAsBufferException::Message( void ) const
{
  std::stringstream os;
  os << "オブジェクト配列として不正な文字列をオブジェクト配列に変換しようとしました。文字列 : " << str_;
  return os.str();
}

// -- BmsTooLongStringConvertedAsBufferException --------------------------
BmsTooLongStringConvertedAsBufferException::BmsTooLongStringConvertedAsBufferException( const std::string& str ) :
str_( str )
{
}

const std::string&
BmsTooLongStringConvertedAsBufferException::GetString( void ) const
{
  return str_;
}

std::string
BmsTooLongStringConvertedAsBufferException::Message( void ) const
{
  std::stringstream os;
  os << "分解能を超えるオブジェクトを持つ文字列をオブジェクト配列に変換しようとしました。文字列 : " << str_;
  return os.str();
}


// -- BmsFileOpenException -----------------------------------------------
BmsFileOpenException::BmsFileOpenException( const std::string& filename, unsigned int error_number ) :
filename_( filename ),
error_number_( error_number )
{
}

const std::string&
BmsFileOpenException::GetFileName( void ) const
{
  return filename_;
}

unsigned int
BmsFileOpenException::GetErrorNumber( void ) const
{
  return error_number_;
}

std::string
BmsFileOpenException::Message( void ) const
{
  std::stringstream os;
  os << "BMS File open failed. " << filename_ << "(" << error_number_ << ")";
  return os.str();
}

// -- BmsFileNotSupportedEncoding -----------------------------------------------
BmsFileNotSupportedEncoding::BmsFileNotSupportedEncoding(const std::string& filename, unsigned int error_number) :
filename_(filename),
error_number_(error_number)
{
}

const std::string&
BmsFileNotSupportedEncoding::GetFileName(void) const
{
	return filename_;
}

unsigned int
BmsFileNotSupportedEncoding::GetErrorNumber(void) const
{
	return error_number_;
}

std::string
BmsFileNotSupportedEncoding::Message(void) const
{
	std::stringstream os;
	os << "BMS File loader - unknown encoding. " << filename_ << "(" << error_number_ << ")";
	return os.str();
}

// -- BmsFileAccessException -----------------------------------------------
BmsFileAccessException::BmsFileAccessException( const std::string& filename, unsigned int error_number ) :
filename_( filename ),
error_number_( error_number )
{
}

const std::string&
BmsFileAccessException::GetFileName( void ) const
{
  return filename_;
}

unsigned int
BmsFileAccessException::GetErrorNumber( void ) const
{
  return error_number_;
}

std::string
BmsFileAccessException::Message( void ) const
{
  std::stringstream os;
  os << "BMS File access failed. " << filename_ << "(" << error_number_ << ")";
  return os.str();
}


// -- BmsDuplicateHeaderException ----------------------------------------
BmsDuplicateHeaderException::BmsDuplicateHeaderException( const std::string& header ) :
header_( header )
{
}

const std::string&
BmsDuplicateHeaderException::GetHeader( void ) const
{
  return header_;
}

std::string
BmsDuplicateHeaderException::Message( void ) const
{
  std::stringstream os;
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

std::string
BmsDuplicateBarChangeException::Message( void ) const
{
  std::stringstream os;
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
BmsParseInvalidBarChangeValueException::BmsParseInvalidBarChangeValueException( unsigned int line, const std::string& str ) :
BmsParseException( line ),
str_( str )
{
}

const std::string&
BmsParseInvalidBarChangeValueException::GetString( void ) const
{
  return str_;
}

std::string
BmsParseInvalidBarChangeValueException::Message( void ) const
{
  std::stringstream os;
  os << "小節長変更の値が不正です。不正な文字列 : " << str_;
  return os.str();
}


// -- BmsParseInvalidEndAsObjectArrayException ---------------------------
BmsParseInvalidEndAsObjectArrayException::BmsParseInvalidEndAsObjectArrayException( unsigned int line ) :
BmsParseException( line )
{
}

std::string
BmsParseInvalidEndAsObjectArrayException::Message( void ) const
{
  std::stringstream os;
  os << "BMS Parse Failed (wrong object/note)";
  return os.str();
}

// -- class BmsParseNoObjectArrayException -------------------------------
BmsParseNoObjectArrayException::BmsParseNoObjectArrayException( unsigned int line ) :
BmsParseException( line )
{
}

std::string
BmsParseNoObjectArrayException::Message( void ) const
{
  std::stringstream os;
  os << "BMS Parse Failed (no object/note exists)";
  return os.str();
}

// -- class BmsParseTooManyObjectException -------------------------------
BmsParseTooManyObjectException::BmsParseTooManyObjectException( unsigned int line ) :
BmsParseException( line )
{
}

std::string
BmsParseTooManyObjectException::Message( void ) const
{
  std::stringstream os;
  os << "Too many object/note exists.";
  return os.str();
}

// -- BmsParseDuplicateHeaderException -----------------------------------
BmsParseDuplicateHeaderException::BmsParseDuplicateHeaderException( unsigned int line, const std::string& header ) :
BmsParseException( line ),
header_( header )
{
}

const std::string&
BmsParseDuplicateHeaderException::GetHeader( void ) const
{
  return header_;
}

std::string
BmsParseDuplicateHeaderException::Message( void ) const
{
  std::stringstream os;
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

std::string
BmsParseDuplicateBarChangeException::Message( void ) const
{
  std::stringstream os;
  os << "小節長変更が複数回指定されました。小節番号 : " << bar_;
  return os.str();
}

// -- BmsParseInvalidCharException ---------------------------------------
BmsParseInvalidCharException::BmsParseInvalidCharException( unsigned int line, const std::string& str ) :
BmsParseException( line ),
str_( str )
{
}

const std::string&
BmsParseInvalidCharException::GetString( void ) const
{
  return str_;
}

// -- BmsParseInvalidCharAsHeaderOrBarException --------------------------
BmsParseInvalidCharAsHeaderOrBarException::BmsParseInvalidCharAsHeaderOrBarException( unsigned int line, const std::string& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::string
BmsParseInvalidCharAsHeaderOrBarException::Message( void ) const
{
  std::stringstream os;
  os << "ヘッダ名または小節番号として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsHeaderKeyException ----------------------------
BmsParseInvalidCharAsHeaderKeyException::BmsParseInvalidCharAsHeaderKeyException( unsigned int line, const std::string& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::string
BmsParseInvalidCharAsHeaderKeyException::Message( void ) const
{
  std::stringstream os;
  os << "ヘッダ名として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsBarException ----------------------------------
BmsParseInvalidCharAsBarException::BmsParseInvalidCharAsBarException( unsigned int line, const std::string& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::string
BmsParseInvalidCharAsBarException::Message( void ) const
{
  std::stringstream os;
  os << "小節番号として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsChannelException ------------------------------
BmsParseInvalidCharAsChannelException::BmsParseInvalidCharAsChannelException( unsigned int line, const std::string& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::string
BmsParseInvalidCharAsChannelException::Message( void ) const
{
  std::stringstream os;
  os << "チャネル番号として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsColonException --------------------------------
BmsParseInvalidCharAsColonException::BmsParseInvalidCharAsColonException( unsigned int line, const std::string& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::string
BmsParseInvalidCharAsColonException::Message( void ) const
{
  std::stringstream os;
  os << "チャネル番号とオブジェクト配列を区切るコロンとして不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidCharAsObjectArrayException --------------------------
BmsParseInvalidCharAsObjectArrayException::BmsParseInvalidCharAsObjectArrayException( unsigned int line, const std::string& str ) :
BmsParseInvalidCharException( line, str )
{
}

std::string
BmsParseInvalidCharAsObjectArrayException::Message( void ) const
{
  std::stringstream os;
  os << "オブジェクト配列として不正な文字が指定されました。文字列 : " << this->GetString();
  return os.str();
}

// -- BmsParseInvalidRandomValueException --------------------------------
BmsParseInvalidRandomValueException::BmsParseInvalidRandomValueException( unsigned int line,
                                                                          const std::string& key,
                                                                          const std::string& value ) :
BmsParseException( line ),
key_( key ),
value_( value )
{
}

const std::string&
BmsParseInvalidRandomValueException::GetKey( void ) const
{
  return key_;
}

const std::string&
BmsParseInvalidRandomValueException::GetValue( void ) const
{
  return value_;
}

std::string
BmsParseInvalidRandomValueException::Message( void ) const
{
  std::stringstream os;
  os << "ランダム構文 #" << key_ << " で指定された値が不正です。値 : " << value_;
  return os.str();
}

// -- BmsParseUnexceptedEndIfException -----------------------------------
BmsParseUnexceptedEndIfException::BmsParseUnexceptedEndIfException( unsigned int line ) :
BmsParseException( line )
{
}

std::string
BmsParseUnexceptedEndIfException::Message( void ) const
{
  std::stringstream os;
  os << "Wrong #IF ~ #ENDIF clause.";
  return os.str();
}

// -- BmsParseNotFoundEndIfException -------------------------------------
BmsParseNotFoundEndIfException::BmsParseNotFoundEndIfException( unsigned int line )
{
}

std::string
BmsParseNotFoundEndIfException::Message( void ) const
{
  std::stringstream os;
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

std::string
BmsLongNoteObjectInvalidEncloseException::Message(void) const
{
	std::stringstream ss;
	ss << "Longnote has been closed unexpectedly." << "\r\n" <<
		"Start : " << start_.ToString() << "\r\n" <<
		"End : " << end_.ToString() << "\r\n" <<
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

std::string
BmsLongNoteObjectNotEnclosedException::Message(void) const
{
	return "Longnote had not been enclosed properly.";
}

// -- InvalidFormatAsBpmChangeValueException ---------------------------------
InvalidFormatAsBpmChangeValueException::InvalidFormatAsBpmChangeValueException(const BmsWord& word)
	: word(word)
{
}

std::string
InvalidFormatAsBpmChangeValueException::Message(void) const
{
	std::stringstream ss;
	ss << "Wrong BPM Value found on channel #03: " << word.ToString();
	return ss.str();
}

// -- InvalidFormatAsExtendedBpmChangeValueException ---------------------------------
InvalidFormatAsExtendedBpmChangeValueException::InvalidFormatAsExtendedBpmChangeValueException(const BmsWord& word, const std::string& value)
	: word(word)
{
}

std::string
InvalidFormatAsExtendedBpmChangeValueException::Message(void) const
{
	std::stringstream ss;
	ss << "Wrong Extended BPM Value found: " << word.ToString() << " = " << value;
	return ss.str();
}

// -- InvalidFormatAsBpmChangeValueException ---------------------------------
InvalidFormatAsStopSequenceException::InvalidFormatAsStopSequenceException(const BmsWord& word, const std::string& value)
	: word(word)
{
}

std::string
InvalidFormatAsStopSequenceException::Message(void) const
{
	std::stringstream ss;
	ss << "Wrong STOP Value found: " << word.ToString() << " = " << value;
	return ss.str();
}

// -- ExtendedBpmChangeEntryNotExistException ---------------------------------
ExtendedBpmChangeEntryNotExistException::ExtendedBpmChangeEntryNotExistException(const BmsWord& word)
	: word(word)
{
}

std::string
ExtendedBpmChangeEntryNotExistException::Message(void) const
{
	std::stringstream ss;
	ss << "Cannot find Extended BPM on channel #08: " << word.ToString();
	return ss.str();
}

// -- StopSequenceEntryNotExistException ---------------------------------
StopSequenceEntryNotExistException::StopSequenceEntryNotExistException(const BmsWord& word)
	: word(word)
{
}

std::string
StopSequenceEntryNotExistException::Message(void) const
{
	std::stringstream ss;
	ss << "Cannot find STOP sequence on channel #09: " << word.ToString();
	return ss.str();
}

// -- InvalidFormatAsBpmHeaderException ---------------------------------
InvalidFormatAsBpmHeaderException::InvalidFormatAsBpmHeaderException()
{
}

std::string
InvalidFormatAsBpmHeaderException::Message(void) const
{
	return "Base BPM is invalid value. cannot be converted to float.";
}

// -- InvalidLongNoteType ---------------------------------
InvalidLongNoteType::InvalidLongNoteType(const std::string& value)
	: value(value)
{
}

std::string
InvalidLongNoteType::Message(void) const
{
	return "#LNTYPE is invalid: " + value;
}

// -- UnsupportedLongNoteType ---------------------------------
UnsupportedLongNoteType::UnsupportedLongNoteType(const std::string& value)
	: value(value)
{
}

std::string
UnsupportedLongNoteType::Message(void) const
{
	return "Unsupported #LNTYPE value: " + value;
}

// -- UnknownChannel ---------------------------------
UnknownChannelException::UnknownChannelException(const BmsWord& word)
	: word(word)
{
}

std::string
UnknownChannelException::Message(void) const
{
	return "Unknown channel: " + word.ToString();
}

// -- BmsTimeWrongException ---------------------------------
BmsTimeWrongException::BmsTimeWrongException()
{
}

std::string
BmsTimeWrongException::Message(void) const
{
	return "Less time then previous one cannot be inserted into BmsTimeManager.";
}
