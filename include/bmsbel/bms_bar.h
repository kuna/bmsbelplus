#ifndef BMSBEL_BAR_H_
#define BMSBEL_BAR_H_

#include <deque>

#include "bms_define.h"

class BmsBarManager;

// -- BmsBar -------------------------------------------------------------
class BmsBar {
public:
  explicit BmsBar( BmsBarManager* manager );
  explicit BmsBar( BmsBarManager* manager, unsigned int length );

  ~BmsBar();

  unsigned int GetLength( void ) const;
  double GetRatio( void ) const;

  void SetLength( unsigned int length );
  void SetRatio( double ratio );

private:
  BmsBarManager* manager_;
  unsigned int   length_;
};

// -- BmsBarManager ------------------------------------------------------
class BmsBarManager {
public:
  explicit BmsBarManager( void );

  BmsBar At( unsigned int pos ) const;
  BmsBar& operator []( unsigned int pos );
  const BmsBar& operator []( unsigned int pos ) const;

  void InsertAt( unsigned int pos, BmsBar bar );
  void DeleteAt( unsigned int pos );

  void Clear( void );

  unsigned int GetChannelPositionByBarNumber( unsigned int bar ) const;
  unsigned int GetBarNumberByChannelPosition( unsigned int pos ) const;

  unsigned int GetBarDivisionCount( void ) const;
  void MultiplyBarDivisionCount( unsigned int multiplier );
private:
  void IfPositionOverMaxError( unsigned int pos ) const;

private:
  unsigned int       bar_division_count_;
  std::deque<BmsBar> array_;
};

#endif // BMSBEL_BAR_H_
