#include <stdio.h>
#include <time.h>

#include "bmsbel\bms_exception.h"

#include "bmsbel\bms_random.h"


// -- BmsRandom::GenerateRandom ------------------------------------------
BmsRandom::GenerateRandom::GenerateRandom( unsigned int max ) :
max_( max )
{
}

unsigned int
BmsRandom::GenerateRandom::GetMax( void ) const
{
  return max_;
}

// -- BmsRandom::Block ------------------------------------------------------------
BmsRandom::Block::Block( void ) :
data_(),
generate_random_array_(),
child_( NULL )
{
}

BmsRandom::Block::~Block()
{
  for ( std::vector<GenerateRandom*>::iterator it = generate_random_array_.begin(); it != generate_random_array_.end(); ++it ) {
    delete *it;
  }
  if ( child_ != NULL ) {
    delete child_;
  }
}

BmsBms&
BmsRandom::Block::GetData( void )
{
  return data_;
}

std::vector<BmsRandom::GenerateRandom*>
BmsRandom::Block::GetGenerateRandomArray( void )
{
  return generate_random_array_;
}

BmsRandom::IfStatement*
BmsRandom::Block::GetChild( void )
{
  return child_;
}

BmsRandom::IfStatement&
BmsRandom::Block::MakeNewChild( unsigned int prerequisite )
{
  if ( child_ != NULL ) {
    delete child_;
    child_ = NULL;
  }
  child_ = new IfStatement( prerequisite );
  return *child_;
}

BmsRandom::GenerateRandom&
BmsRandom::Block::MakeNewGenerateRandom( unsigned int max )
{
  GenerateRandom* tmp = new GenerateRandom( max );
  generate_random_array_.push_back( tmp );
  return *tmp;
}

// -- BmsRandom::IfStatement ------------------------------------------------------
BmsRandom::IfStatement::IfStatement( unsigned int prerequisite ) :
prerequisite_( prerequisite ),
block_array_()
{
}

BmsRandom::IfStatement::~IfStatement()
{
  for ( std::vector<Block*>::iterator it = block_array_.begin(); it != block_array_.end(); ++it ) {
    delete *it;
  }
}

unsigned int
BmsRandom::IfStatement::GetPrequisite( void ) const
{
  return prerequisite_;
}

unsigned int
BmsRandom::IfStatement::GetBlockArraySize( void ) const
{
  return block_array_.size();
}

BmsRandom::Block&
BmsRandom::IfStatement::GetBlock( unsigned int index )
{
  if ( index >= block_array_.size() ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );;
  }
  return *block_array_[index];
}

BmsRandom::Block&
BmsRandom::IfStatement::GetLastBlock( void )
{
  if ( block_array_.empty() ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );;
  }
  return *block_array_[block_array_.size() - 1];
}

BmsRandom::Block&
BmsRandom::IfStatement::GetFirstBlock( void )
{
  if ( block_array_.empty() ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );;
  }
  return *block_array_[0];
}


BmsRandom::Block&
BmsRandom::IfStatement::MakeNewBlock( void )
{
  Block* tmp = new Block();
  block_array_.push_back( tmp );
  return *tmp;
}

BmsRandom::Block&
BmsRandom::IfStatement::MakeNewBlockWithCopyArrayName( const BmsRegistArraySet& array_set )
{
  Block& tmp = this->MakeNewBlock();
  for ( BmsRegistArraySet::ConstIterator it = array_set.Begin(); it != array_set.End(); ++it ) {
    if ( NOT( tmp.GetData().GetRegistArraySet().Exists( it->first ) ) ) {
      tmp.GetData().GetRegistArraySet().MakeNewArray( it->first );
    }
  }
  return tmp;
}


// -- BmsRandom::RootStatement -------------------------------------------
BmsRandom::RootStatement::RootStatement( void ) :
IfStatement( 0 )
{
}
