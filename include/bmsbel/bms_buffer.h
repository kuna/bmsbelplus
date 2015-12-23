#ifndef BMSBEL_BUFFER_H_
#define BMSBEL_BUFFER_H_

#include <string>
#include <vector>

#include "bms_word.h"

class BmsBuffer {
public:
  explicit BmsBuffer( unsigned int length );
  explicit BmsBuffer( unsigned int length, const std::wstring& input );

  virtual ~BmsBuffer();

  unsigned int GetLength( void ) const;
  unsigned int GetCount( void ) const;

  BmsWord At( unsigned int pos ) const;
  virtual BmsWord& operator []( unsigned int pos );

  void DeleteAt( unsigned int pos );

  void Clear( void );
  
  virtual void Merge( const BmsBuffer& buffer );
  virtual void Merge( unsigned int start, const BmsBuffer& buffer );

  BmsBuffer SubBuffer( unsigned int from, unsigned int to ) const;
  BmsBuffer SubBufferLength( unsigned int from, unsigned int length ) const;

  bool Contains( const BmsWord &word ) const;

  void ExtendTo( unsigned int size );
  void MagnifyBy( unsigned int multiplier );

  std::string ToString( void ) const;

  // -- Iterator ---------------------------------------------------------
  typedef std::vector<BmsWord>::iterator Iterator;
  typedef std::vector<BmsWord>::const_iterator ConstIterator;

  Iterator Begin( void );
  Iterator End( void );
  ConstIterator Begin( void ) const;
  ConstIterator End( void ) const;

private:
  void IfPositionOverLengthError( unsigned int pos ) const;

private:
  std::vector<BmsWord> array_;
};

#endif // BMSBEL_BUFFER_H_
