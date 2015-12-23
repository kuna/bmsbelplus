#ifndef BMSBEL_EXCEPTION_H_
#define BMSBEL_EXCEPTION_H_

#include <stdexcept>
#include <typeinfo>
#include "bms_word.h"

// 多重継承用
// 原因クラス保持用
class BmsWithCauseClassException
{
protected:
  explicit BmsWithCauseClassException( const std::type_info& klass ) : klass_( klass ) {}
  const std::type_info& GetCauseClass( void ) const { return klass_; }
private:
  const std::type_info& klass_;
};


// -- BmsException -------------------------------------------------------
class BmsException : public std::domain_error {
public:
  explicit BmsException( void );
  virtual ~BmsException();

  virtual std::wstring Message( void ) const = 0;
};


// -- BmsInternalException -----------------------------------------------
class BmsInternalException : public BmsException {
public:
  explicit BmsInternalException( const char* file, unsigned int line );

  const char*  GetFile( void ) const;
  unsigned int GetLine( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const char*  file_;
  unsigned int line_;
};

#define BMS_INTERNAL_EXCEPTION BmsInternalException( __FILE__, __LINE__ )


// -- BmsOutOfRangeAccessException ---------------------------------------
class BmsOutOfRangeAccessException : public BmsException,
                                     public BmsWithCauseClassException {
public:
  explicit BmsOutOfRangeAccessException( const std::type_info& klass );

  virtual std::wstring Message( void ) const;
};

// -- BmsInvalidWordValueUsedException -----------------------------------
class BmsInvalidWordValueUsedException : public BmsException {
public:
  explicit BmsInvalidWordValueUsedException( int value );

  int GetValue( void ) const;

  virtual std::wstring Message( void ) const;

private:
  int value_;
};

// -- BmsInvalidCharUsedAsWordException ----------------------------------
class BmsInvalidCharUsedAsWordException : public BmsException {
public:
  explicit BmsInvalidCharUsedAsWordException( char msb, char lsb );

  char GetMsb( void ) const;
  char GetLsb( void ) const;
  std::wstring GetAsString( void ) const;

  virtual std::wstring Message( void ) const;

private:
  char msb_;
  char lsb_;
};

// -- BmsInvalidStringConvertedAsBufferException -------------------------
class BmsInvalidStringConvertedAsBufferException : public BmsException {
public:
  explicit BmsInvalidStringConvertedAsBufferException( const std::wstring& str );

  const std::wstring& GetString( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const std::wstring str_;
};

// -- BmsTooLongStringConvertedAsBufferException --------------------------
class BmsTooLongStringConvertedAsBufferException : public BmsException {
public:
  explicit BmsTooLongStringConvertedAsBufferException( const std::wstring& str );

  const std::wstring& GetString( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const std::wstring str_;
};

// -- BmsFileOpenException -----------------------------------------------
class BmsFileOpenException : public BmsException {
public:
  explicit BmsFileOpenException( const std::wstring& filename, unsigned int error_number );

  const std::wstring& GetFileName( void ) const;
  unsigned int       GetErrorNumber( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const std::wstring filename_;
  unsigned int      error_number_;
};

// -- BmsFileNotSupportedEncoding -----------------------------------------------
class BmsFileNotSupportedEncoding : public BmsException {
public:
	BmsFileNotSupportedEncoding(const std::wstring& filename, unsigned int error_number);

	const std::wstring& GetFileName(void) const;
	unsigned int       GetErrorNumber(void) const;

	virtual std::wstring Message(void) const;

private:
	const std::wstring filename_;
	unsigned int      error_number_;
};

// -- BmsFileAccessException -----------------------------------------------
class BmsFileAccessException : public BmsException {
public:
  explicit BmsFileAccessException( const std::wstring& filename, unsigned int error_number );

  const std::wstring& GetFileName( void ) const;
  unsigned int       GetErrorNumber( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const std::wstring filename_;
  unsigned int      error_number_;
};


// -- BmsDuplicateHeaderException ----------------------------------------
class BmsDuplicateHeaderException : public BmsException {
public:
  explicit BmsDuplicateHeaderException( const std::wstring& header );

  const std::wstring& GetHeader( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const std::wstring header_;
};

// -- BmsDuplicateBarChangeException ----------------------------------------
class BmsDuplicateBarChangeException : public BmsException {
public:
  explicit BmsDuplicateBarChangeException( unsigned int bar );

  unsigned int GetBar( void ) const;

  virtual std::wstring Message( void ) const;

private:
  unsigned int bar_;
};


// -- BmsParseException --------------------------------------------------
class BmsParseException : public BmsException {
public:
  explicit BmsParseException( unsigned int line );

  unsigned int GetLine( void ) const;

private:
  unsigned int line_;
};


// -- BmsParseInvalidBarChangeValueException -----------------------------
class BmsParseInvalidBarChangeValueException : public BmsParseException {
public:
  explicit BmsParseInvalidBarChangeValueException( unsigned int line, const std::wstring& str );

  const std::wstring& GetString( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const std::wstring str_;
};


// -- BmsParseInvalidEndAsObjectArrayException ---------------------------
class BmsParseInvalidEndAsObjectArrayException : public BmsParseException {
public:
  explicit BmsParseInvalidEndAsObjectArrayException( unsigned int line );

  virtual std::wstring Message( void ) const;
};


// -- class BmsParseNoObjectArrayException -------------------------------
class BmsParseNoObjectArrayException : public BmsParseException {
public:
  explicit BmsParseNoObjectArrayException( unsigned int line );

  virtual std::wstring Message( void ) const;
};


// -- class BmsParseTooManyObjectException -------------------------------
class BmsParseTooManyObjectException : public BmsParseException {
public:
  explicit BmsParseTooManyObjectException( unsigned int line );

  virtual std::wstring Message( void ) const;
};


// -- BmsParseDuplicateHeaderException -----------------------------------
class BmsParseDuplicateHeaderException : public BmsParseException {
public:
  explicit BmsParseDuplicateHeaderException( unsigned int line, const std::wstring& header );

  const std::wstring& GetHeader( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const std::wstring header_;
};

// -- BmsParseDuplicateBarChangeException --------------------------------
class BmsParseDuplicateBarChangeException : public BmsParseException {
public:
  explicit BmsParseDuplicateBarChangeException( unsigned int line, unsigned int bar );

  unsigned int GetBar( void ) const;

  virtual std::wstring Message( void ) const;

private:
  unsigned int bar_;
};


// -- BmsParseInvalidCharException ---------------------------------------
class BmsParseInvalidCharException : public BmsParseException {
public:
  explicit BmsParseInvalidCharException( unsigned int line, const std::wstring& str );

  const std::wstring& GetString( void ) const;

private:
  const std::wstring str_;
};


// -- BmsParseInvalidCharAsHeaderOrBarException --------------------------
class BmsParseInvalidCharAsHeaderOrBarException : public BmsParseInvalidCharException {
public:
  explicit BmsParseInvalidCharAsHeaderOrBarException( unsigned int line, const std::wstring& str );

  virtual std::wstring Message( void ) const;
};

// -- BmsParseInvalidCharAsHeaderKeyException ----------------------------
class BmsParseInvalidCharAsHeaderKeyException : public BmsParseInvalidCharException {
public:
  explicit BmsParseInvalidCharAsHeaderKeyException( unsigned int line, const std::wstring& str );

  virtual std::wstring Message( void ) const;
};

// -- BmsParseInvalidCharAsBarException ----------------------------------
class BmsParseInvalidCharAsBarException : public BmsParseInvalidCharException {
public:
  explicit BmsParseInvalidCharAsBarException( unsigned int line, const std::wstring& str );

  virtual std::wstring Message( void ) const;
};

// -- BmsParseInvalidCharAsChannelException ------------------------------
class BmsParseInvalidCharAsChannelException : public BmsParseInvalidCharException {
public:
  explicit BmsParseInvalidCharAsChannelException( unsigned int line, const std::wstring& str );

  virtual std::wstring Message( void ) const;
};

// -- BmsParseInvalidCharAsColonException --------------------------------
class BmsParseInvalidCharAsColonException : public BmsParseInvalidCharException {
public:
  explicit BmsParseInvalidCharAsColonException( unsigned int line, const std::wstring& str );

  virtual std::wstring Message( void ) const;
};

// -- BmsParseInvalidCharAsObjectArrayException --------------------------
class BmsParseInvalidCharAsObjectArrayException : public BmsParseInvalidCharException {
public:
  explicit BmsParseInvalidCharAsObjectArrayException( unsigned int line, const std::wstring& str );

  virtual std::wstring Message( void ) const;
};

// -- BmsParseInvalidRandomValueException --------------------------------
class BmsParseInvalidRandomValueException : public BmsParseException {
public:
  explicit BmsParseInvalidRandomValueException( unsigned int line,
                                                const std::wstring& key,
                                                const std::wstring& value );

  const std::wstring& GetKey( void ) const;
  const std::wstring& GetValue( void ) const;

  virtual std::wstring Message( void ) const;

private:
  const std::wstring key_;
  const std::wstring value_;
};


// -- BmsParseUnexceptedEndIfException -----------------------------------
class BmsParseUnexceptedEndIfException : public BmsParseException {
public:
  explicit BmsParseUnexceptedEndIfException( unsigned int line );

  virtual std::wstring Message( void ) const;
};

//
// BMSEXCEPTION
//
// -- BmsParseNotFoundEndIfException -------------------------------------
class BmsParseNotFoundEndIfException : public BmsException {
public:
  explicit BmsParseNotFoundEndIfException( unsigned int line );

  virtual std::wstring Message( void ) const;
};

// -- LongNoteObjectInvalidEncloseException ----------------------------
class BmsLongNoteObjectInvalidEncloseException : public BmsException {
public:
	explicit BmsLongNoteObjectInvalidEncloseException(const BmsWord& start, const BmsWord& end, unsigned int bar);
	virtual std::wstring Message(void) const;

	const BmsWord& GetStartWord(void) const;
	const BmsWord& GetEndWord(void) const;
	unsigned int GetBar(void) const;

private:
	BmsWord start_;
	BmsWord end_;
	unsigned int bar_;
};

// -- LongNoteObjectNotEnclosedException -------------------------------
class BmsLongNoteObjectNotEnclosedException : public BmsException {
public:
	explicit BmsLongNoteObjectNotEnclosedException(void);
	virtual std::wstring Message(void) const;
};

// -- InvalidFormatAsBpmChangeValueException -------------------------------
class InvalidFormatAsBpmChangeValueException : public BmsException {
	BmsWord word;
public:
	InvalidFormatAsBpmChangeValueException(const BmsWord& word);
	virtual std::wstring Message(void) const;
};

// -- InvalidFormatAsExtendedBpmChangeValueException -------------------------------
class InvalidFormatAsExtendedBpmChangeValueException : public BmsException {
	BmsWord word;
	std::wstring value;
public:
	InvalidFormatAsExtendedBpmChangeValueException(const BmsWord& word, const std::wstring& value);
	virtual std::wstring Message(void) const;
};

// -- InvalidFormatAsStopSequenceException -------------------------------
class InvalidFormatAsStopSequenceException : public BmsException {
	BmsWord word;
	std::wstring value;
public:
	InvalidFormatAsStopSequenceException(const BmsWord& word, const std::wstring& value);
	virtual std::wstring Message(void) const;
};

// -- ExtendedBpmChangeEntryNotExistException -------------------------------
class ExtendedBpmChangeEntryNotExistException : public BmsException {
private:
	BmsWord word;
public:
	ExtendedBpmChangeEntryNotExistException(const BmsWord& word);
	virtual std::wstring Message(void) const;
};

// -- StopSequenceEntryNotExistException -------------------------------
class StopSequenceEntryNotExistException : public BmsException {
private:
	BmsWord word;
public:
	StopSequenceEntryNotExistException(const BmsWord& word);
	virtual std::wstring Message(void) const;
};

// -- InvalidFormatAsBpmHeaderException -------------------------------
class InvalidFormatAsBpmHeaderException : public BmsException {
public:
	InvalidFormatAsBpmHeaderException();
	virtual std::wstring Message(void) const;
};

// -- InvalidLongNoteType -------------------------------
class InvalidLongNoteType : public BmsException {
private:
	std::wstring value;
public:
	InvalidLongNoteType(const std::wstring& value);
	virtual std::wstring Message(void) const;
};

// -- UnsupportedLongNoteType -------------------------------
class UnsupportedLongNoteType : public BmsException {
private:
	std::wstring value;
public:
	UnsupportedLongNoteType(const std::wstring& value);
	virtual std::wstring Message(void) const;
};

// -- UnknownChannel -------------------------------
class UnknownChannelException : public BmsException {
private:
	BmsWord word;
public:
	UnknownChannelException(const BmsWord& word);
	virtual std::wstring Message(void) const;
};

// -- BmsTimeWrongException -------------------------------
class BmsTimeWrongException : public BmsException {
public:
	BmsTimeWrongException();
	virtual std::wstring Message(void) const;
};

#endif // BMSBEL_EXCEPTION_H_
