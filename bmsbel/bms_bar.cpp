#include "bmsbel\bms_bar.h"

#include "bmsbel\bms_exception.h"


// -- BmsBar -------------------------------------------------------------
BmsBar::BmsBar( BmsBarManager* manager ) :
manager_( manager ),
length_( BmsConst::DEFAULT_BAR_DIVISION_COUNT )
{
}

BmsBar::BmsBar( BmsBarManager* manager, unsigned int length ) :
manager_( manager ),
length_( length )
{
}

BmsBar::~BmsBar()
{
}


unsigned int
BmsBar::GetLength( void ) const
{
  return length_;
}

double
BmsBar::GetRatio( void ) const
{
  return static_cast<double>( length_ ) / static_cast<double>( manager_->GetBarDivisionCount() );
}

void
BmsBar::SetLength( unsigned int length )
{
  length_ = length;
}

void
BmsBar::SetRatio( double ratio )
{
  length_ = static_cast<unsigned int>( static_cast<double>( manager_->GetBarDivisionCount() ) * ratio );
}


// -- BmsBarManager ------------------------------------------------------
BmsBarManager::BmsBarManager( void ) :
bar_division_count_( BmsConst::DEFAULT_BAR_DIVISION_COUNT ),
array_( BmsConst::BAR_MAX_COUNT, BmsBar( this ) )
{
}

BmsBar
BmsBarManager::At( unsigned int pos ) const
{
  this->IfPositionOverMaxError( pos );
  return array_[pos];
}

BmsBar&
BmsBarManager::operator []( unsigned int pos )
{
  this->IfPositionOverMaxError( pos );
  return array_[pos];
}

const BmsBar&
BmsBarManager::operator []( unsigned int pos ) const
{
  this->IfPositionOverMaxError( pos );
  return array_[pos];
}

void
BmsBarManager::InsertAt( unsigned int pos, BmsBar bar )
{
  this->IfPositionOverMaxError( pos );
  array_.insert( array_.begin() + pos, bar );
  array_.pop_back();
}

void
BmsBarManager::DeleteAt( unsigned int pos )
{
  this->IfPositionOverMaxError( pos );
  array_.erase( array_.begin() + pos );
  array_.push_back( BmsBar( this, bar_division_count_ ) );
}

void
BmsBarManager::Clear( void )
{
  for ( std::deque<BmsBar>::iterator it = array_.begin(); it != array_.end(); ++it ) {
    it->SetLength( bar_division_count_ );
  }
}


unsigned int
BmsBarManager::GetChannelPositionByBarNumber( unsigned int pos ) const
{
  this->IfPositionOverMaxError( pos );
  unsigned int current = 0;
  for ( unsigned int i = 0; i < pos; ++i ) {
    current += array_[i].GetLength();
  }
  return current;
}

unsigned int
BmsBarManager::GetBarNumberByChannelPosition( unsigned int pos ) const
{
  unsigned int bar = 0;
  for ( unsigned int current = 0; bar <= BmsConst::BAR_MAX_VALUE; ++bar ) {
    if ( current > pos ) {
      break;
    }
    current += array_[bar].GetLength();
  }
  return bar - 1;
}


unsigned int
BmsBarManager::GetBarDivisionCount( void ) const
{
  return bar_division_count_;
}

void
BmsBarManager::MultiplyBarDivisionCount( unsigned int multiplier )
{
  bar_division_count_ *= multiplier;
  for ( std::deque<BmsBar>::iterator it = array_.begin(); it != array_.end(); ++it ) {
    it->SetLength( it->GetLength() * multiplier );
  }
}

void
BmsBarManager::IfPositionOverMaxError( unsigned int pos ) const
{
  if ( pos > BmsConst::BAR_MAX_VALUE ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );
  }
}
