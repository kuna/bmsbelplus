#include "bmsbel\bms_buffer.h"

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_util.h"
#include "bmsbel\bms_exception.h"
#include "bmsbel\bms_word.h"


BmsBuffer::BmsBuffer( unsigned int length ) :
array_( length, BmsWord::MIN )
{
}

BmsBuffer::BmsBuffer( unsigned int length, const std::wstring& input ) :
array_( length, BmsWord::MIN )
{
  if ( input.size() == 0 || input.size() % 2 == 1 ) {
    throw BmsInvalidStringConvertedAsBufferException( input );
  }
  if ( input.size() / 2 > length ) {
    throw BmsTooLongStringConvertedAsBufferException( input );
  }

  const wchar_t* p = input.c_str();
  for ( unsigned int i = 0; i < length; i += length / ( input.size() / 2 ) ) {
    array_[i] = BmsWord( p );
    p += 2;
  }
}


BmsBuffer::~BmsBuffer()
{
}


unsigned int
BmsBuffer::GetLength( void ) const
{
  return array_.size();
}

unsigned int
BmsBuffer::GetCount( void ) const
{
  unsigned int count = 0;
  for ( BmsBuffer::ConstIterator it = this->Begin(); it != this->End(); ++it ) {
    if ( *it != BmsWord::MIN ) {
      ++count;
    }
  }
  return count;
}


BmsWord
BmsBuffer::At( unsigned int pos ) const
{
  this->IfPositionOverLengthError( pos );
  return array_[pos];
}

BmsWord&
BmsBuffer::operator []( unsigned int pos )
{
  this->IfPositionOverLengthError( pos );
  return array_[pos];
}


void
BmsBuffer::DeleteAt( unsigned int pos )
{
  this->IfPositionOverLengthError( pos );
  array_[pos] = BmsWord::MIN;
}


void
BmsBuffer::Clear( void )
{
  array_.clear();
}


void
BmsBuffer::Merge( const BmsBuffer& buffer )
{
  this->Merge( 0, buffer );
}

void
BmsBuffer::Merge( unsigned int start, const BmsBuffer& buffer )
{
  if ( start + buffer.GetLength() > this->GetLength() ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );
  }
  for ( unsigned int i = 0; ; ++i ) {
    if ( i > start + array_.size() || i > buffer.GetLength() ) {
      break;
    }
    if ( buffer.At( i ) != BmsWord::MIN ) {
      array_[start + i] = buffer.At( i );
    }
  }
}


BmsBuffer
BmsBuffer::SubBuffer( unsigned int from, unsigned int to ) const
{
  if ( to < from ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );
  }
  return this->SubBufferLength( from, to - from + 1 );
}

BmsBuffer
BmsBuffer::SubBufferLength( unsigned int from, unsigned int length ) const
{
  if ( from + length > this->GetLength() ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );
  }

  BmsBuffer buf( length );
  for ( unsigned int i = 0; i < length; ++i ) {
    buf.array_[i] = array_[from + i];
  }
  return buf;
}

bool
BmsBuffer::Contains( const BmsWord &word ) const
{
  for ( BmsBuffer::ConstIterator it = this->Begin(); it != this->End(); ++it ) {
    if ( *it == word ) {
      return true;
    }
  }
  return false;
}

void
BmsBuffer::ExtendTo( unsigned int size )
{
  if ( size > this->GetLength() ) {
    array_.resize( size, BmsWord::MIN );
  }
}

void
BmsBuffer::MagnifyBy( unsigned int multiplier )
{
  int old_length = static_cast<int>( this->GetLength() );
  this->ExtendTo( this->GetLength() * multiplier );
  for ( int i = old_length - 1; i > 0; --i ) {
    array_[i * multiplier] = array_[i];
    array_[i] = BmsWord::MIN;
  }
}


std::string
BmsBuffer::ToString( void ) const
{
  if ( this->GetLength() == 0 ) {
    return "";
  }

  // 飛ばす間隔を計算
  unsigned int step = this->GetLength();
  for ( unsigned int i = 1; i < this->GetLength(); ++i ) {
    if ( array_[i] != BmsWord::MIN ) {
      step = BmsUtil::GCD( step, i );
    }
  }

  std::string tmp;
  tmp.reserve( this->GetLength() / step * 2 );
  for ( unsigned int i = 0; i < this->GetLength(); i += step ) {
    tmp.append( array_[i].ToCharPtr() );
  }
  return tmp;
}


BmsBuffer::Iterator
BmsBuffer::Begin( void )
{
  return array_.begin();
}

BmsBuffer::Iterator
BmsBuffer::End( void )
{
  return array_.end();
}

BmsBuffer::ConstIterator
BmsBuffer::Begin( void ) const
{
  return array_.begin();
}

BmsBuffer::ConstIterator
BmsBuffer::End( void ) const
{
  return array_.end();
}


void
BmsBuffer::IfPositionOverLengthError( unsigned int pos ) const
{
  if ( pos >= array_.size() ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );
  }
}
