#include "bmsbel\bms_exception.h"

#include "bmsbel\bms_parser_frame.h"
#include "bmsbel\bms_parser.h"		// MUST include to provide proper size of BmsParser

// -- BmsParser::BgmChannelCounter --------------------------------------------------
BmsParser::BgmChannelCounter::BgmChannelCounter( void ) :
std::vector<unsigned int>( BmsConst::BAR_MAX_COUNT )
{
}

BmsParser::BgmChannelCounter::BgmChannelCounter( const BgmChannelCounter& other ) :
std::vector<unsigned int>( BmsConst::BAR_MAX_COUNT )
{
  this->std::vector<unsigned int>::operator =( other );
}

void
BmsParser::BgmChannelCounter::ZeroClear( void )
{
  for ( BgmChannelCounter::Iterator it = this->Begin(); it != this->End(); ++it ) {
    *it = 0;
  }
}

BmsParser::BgmChannelCounter::Iterator
BmsParser::BgmChannelCounter::Begin( void )
{
  return this->begin();
}

BmsParser::BgmChannelCounter::Iterator
BmsParser::BgmChannelCounter::End( void )
{
  return this->end();
}

BmsParser::BgmChannelCounter::ConstIterator
BmsParser::BgmChannelCounter::Begin( void ) const
{
  return this->begin();
}

BmsParser::BgmChannelCounter::ConstIterator
BmsParser::BgmChannelCounter::End( void ) const
{
  return this->end();
}


// -- BmsParser::FrameData -----------------------------------------------
BmsParser::FrameData::FrameData( BmsBms& bms ) :
bms_( bms ),
bgm_channel_counter_( *new BgmChannelCounter() )
{
}

BmsParser::FrameData::~FrameData()
{
  delete &bgm_channel_counter_;
}

// -- BmsParser::FrameStack ----------------------------------------------
BmsParser::FrameStack::FrameStack( void ) :
depth_( 1 ),
random_value_( 0 )
{
}

BmsParser::FrameStack::~FrameStack()
{
}

void
BmsParser::FrameStack::PushIfStatement( unsigned int prerequisite )
{
  NOT_USE_VAR( prerequisite );
  ++depth_;
}

void
BmsParser::FrameStack::PopIfStatement( void )
{
  --depth_;
}

void
BmsParser::FrameStack::MoveChildIfStatement( unsigned int prerequisite )
{
  NOT_USE_VAR( prerequisite );
  ++depth_;
}

void
BmsParser::FrameStack::MoveParentIfStatement( void )
{
  --depth_;
}


unsigned
BmsParser::FrameStack::GetDepth( void ) const
{
  return depth_;
}

void
BmsParser::FrameStack::SetRandomValue( unsigned int value )
{
  random_value_ = value;
}

unsigned int
BmsParser::FrameStack::GetRandomValue( void ) const
{
  return random_value_;
}

// -- BmsParser::FrameStackMasterOnly ------------------------------------
BmsParser::FrameStackMasterOnly::FrameStackMasterOnly( BmsBms& master_data ) :
FrameStack(),
if_condition_stack_(),
master_frame_data_( master_data )
{
  if_condition_stack_.push( true );
}

BmsParser::FrameStackMasterOnly::~FrameStackMasterOnly()
{
}

void
BmsParser::FrameStackMasterOnly::PushGenerateRandom( unsigned int max )
{
  NOT_USE_VAR( max );
}

void
BmsParser::FrameStackMasterOnly::PushIfStatement( unsigned int prerequisite )
{
  if_condition_stack_.push( prerequisite == random_value_ );

  this->FrameStack::PushIfStatement( prerequisite );
}

void
BmsParser::FrameStackMasterOnly::PopIfStatement( void )
{
  if_condition_stack_.pop();

  this->FrameStack::PopIfStatement();
}

void
BmsParser::FrameStackMasterOnly::MoveChildIfStatement( unsigned int prerequisite )
{
  if_condition_stack_.push( prerequisite == random_value_ );

  this->FrameStack::MoveChildIfStatement( prerequisite );
}

void
BmsParser::FrameStackMasterOnly::MoveParentIfStatement( void )
{
  if_condition_stack_.pop();

  this->FrameStack::MoveParentIfStatement();
}

void
BmsParser::FrameStackMasterOnly::CallWriteDownFunction( Parser& parser, WriteDownFunction function )
{
  if ( if_condition_stack_.top() ) {
    (parser.*function)( master_frame_data_ );
  }
}


// -- BmsParser::FrameStackMakeSyntaxTree --------------------------------
BmsParser::FrameStackMakeSyntaxTree::FrameStackMakeSyntaxTree( BmsBms& master_data, BmsRandom::IfStatement& root_statement ) :
FrameStackMasterOnly( master_data ),
main_stack_()
{
  BmsRandom::Block& new_block = root_statement.MakeNewBlockWithCopyArrayName( master_data.GetRegistArraySet() );

  main_stack_.push( new MainFrame( root_statement, new FrameData( new_block.GetData() ) ) );
}

BmsParser::FrameStackMakeSyntaxTree::~FrameStackMakeSyntaxTree()
{
  while ( NOT( main_stack_.empty() ) ) {
    delete main_stack_.top()->frame_data_;
    delete main_stack_.top();
    main_stack_.pop();
  }
}


void
BmsParser::FrameStackMakeSyntaxTree::PushGenerateRandom( unsigned int max )
{
  main_stack_.top()->statement_.GetLastBlock().MakeNewGenerateRandom( max );

  this->FrameStackMasterOnly::PushGenerateRandom( max );
}

void
BmsParser::FrameStackMakeSyntaxTree::PushIfStatement( unsigned int prerequisite )
{
  BmsRandom::IfStatement& new_statement = main_stack_.top()->statement_.GetLastBlock().MakeNewChild( prerequisite );
  BmsRandom::Block& new_block = new_statement.MakeNewBlockWithCopyArrayName(
    main_stack_.top()->frame_data_->bms_.GetRegistArraySet() );

  main_stack_.push( new MainFrame( new_statement, new FrameData( new_block.GetData() ) ) );

  this->FrameStackMasterOnly::PushIfStatement( prerequisite );
}

void
BmsParser::FrameStackMakeSyntaxTree::PopIfStatement( void )
{
  delete main_stack_.top()->frame_data_;
  delete main_stack_.top();
  main_stack_.pop();

  delete main_stack_.top()->frame_data_;
  main_stack_.top()->frame_data_ = new FrameData(
    main_stack_.top()->statement_.MakeNewBlockWithCopyArrayName(
      main_stack_.top()->statement_.GetLastBlock().GetData().GetRegistArraySet() ).GetData() );

  this->FrameStackMasterOnly::PopIfStatement();
}

void
BmsParser::FrameStackMakeSyntaxTree::MoveChildIfStatement( unsigned int prerequisite )
{
  BmsRandom::IfStatement* child_statement = main_stack_.top()->statement_.GetBlock(
    main_stack_.top()->current_block_position_ ).GetChild();
  if ( child_statement == NULL ) {
    throw BMS_INTERNAL_EXCEPTION;
  }
  if ( child_statement->GetBlockArraySize() <= 0 ) {
    throw BMS_INTERNAL_EXCEPTION;
  }
  if ( child_statement->GetPrequisite() != prerequisite ) {
    throw BMS_INTERNAL_EXCEPTION;
  }

  main_stack_.push( new MainFrame( *child_statement, new FrameData( child_statement->GetFirstBlock().GetData() ) ) );

  this->FrameStackMasterOnly::MoveChildIfStatement( prerequisite );
}

void
BmsParser::FrameStackMakeSyntaxTree::MoveParentIfStatement( void )
{
  delete main_stack_.top()->frame_data_;
  delete main_stack_.top();
  main_stack_.pop();

  main_stack_.top()->current_block_position_ += 1;

  if ( main_stack_.top()->statement_.GetBlockArraySize() <= main_stack_.top()->current_block_position_ ) {
    throw BMS_INTERNAL_EXCEPTION;
  }

  delete main_stack_.top()->frame_data_;
  main_stack_.top()->frame_data_ = new FrameData(
    main_stack_.top()->statement_.GetBlock( main_stack_.top()->current_block_position_ ).GetData() );

  this->FrameStackMasterOnly::MoveParentIfStatement();
}

void
BmsParser::FrameStackMakeSyntaxTree::CallWriteDownFunction( Parser& parser, WriteDownFunction function )
{
  if ( main_stack_.top()->frame_data_ != NULL ) {
    (parser.*function)( *main_stack_.top()->frame_data_ );
  }

  this->FrameStackMasterOnly::CallWriteDownFunction( parser, function );
}
