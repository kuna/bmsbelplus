#ifndef BMSBEL_PARSER_FRAME_H_
#define BMSBEL_PARSER_FRAME_H_

#include <stack>

#include "bms_define.h"
#include "bms_bms.h"
#include "bms_random.h"

namespace BmsParser {

  class Parser;
  struct FrameData;
  typedef void (Parser::*WriteDownFunction)( const FrameData& );

  // -- BgmChannelCounter ------------------------------------------------
  class BgmChannelCounter : public std::vector<unsigned int> {
  public:
    explicit BgmChannelCounter( void );
    explicit BgmChannelCounter( const BgmChannelCounter& other );

    void ZeroClear( void );

    typedef std::vector<unsigned int>::iterator Iterator;
    typedef std::vector<unsigned int>::const_iterator ConstIterator;

    Iterator Begin( void );
    Iterator End( void );
    ConstIterator Begin( void ) const;
    ConstIterator End( void ) const;
  };

  // -- FrameData --------------------------------------------------------
  struct FrameData {
  public:
    explicit FrameData( BmsBms& bms );
    ~FrameData();

  public:
    BmsBms&            bms_;
    BgmChannelCounter& bgm_channel_counter_;
  };

  // -- FrameStack -------------------------------------------------------
  class FrameStack {
  public:
    explicit FrameStack( void );

    virtual ~FrameStack();

  public:
    virtual void PushGenerateRandom( unsigned int max ) = 0;
    virtual void PushIfStatement( unsigned int prerequisite ) = 0;
    virtual void PopIfStatement( void ) = 0;

    virtual void MoveChildIfStatement( unsigned int prerequisite ) = 0;
    virtual void MoveParentIfStatement( void ) = 0;

    unsigned int GetDepth( void ) const;

    void SetRandomValue( unsigned int value );
    unsigned int GetRandomValue( void ) const;

    virtual void CallWriteDownFunction( Parser& parser, WriteDownFunction function ) = 0;

  protected:
    unsigned int depth_;
    unsigned int random_value_;
  };

  // -- FrameStackMasterOnly ---------------------------------------------
  class FrameStackMasterOnly : public FrameStack {
  public:
    explicit FrameStackMasterOnly( BmsBms& master_data );

    virtual ~FrameStackMasterOnly();

    virtual void PushGenerateRandom( unsigned int max );
    virtual void PushIfStatement( unsigned int prerequisite );
    virtual void PopIfStatement( void );

    virtual void MoveChildIfStatement( unsigned int prerequisite );
    virtual void MoveParentIfStatement( void );

    virtual void CallWriteDownFunction( Parser& parser, WriteDownFunction function );

  private:
    std::stack<bool>  if_condition_stack_;
    FrameData         master_frame_data_;
  };

  // -- FrameStackMakeSyntaxTree -----------------------------------------
  class FrameStackMakeSyntaxTree : public FrameStackMasterOnly {
  public:
    explicit FrameStackMakeSyntaxTree( BmsBms& master_data, BmsRandom::IfStatement& root_statement );

    virtual ~FrameStackMakeSyntaxTree();

    virtual void PushGenerateRandom( unsigned int max );
    virtual void PushIfStatement( unsigned int prerequisite );
    virtual void PopIfStatement( void );

    virtual void MoveChildIfStatement( unsigned int prerequisite );
    virtual void MoveParentIfStatement( void );

    virtual void CallWriteDownFunction( Parser& parser, WriteDownFunction function );

  private:
    struct MainFrame {
    public:
      explicit MainFrame( BmsRandom::IfStatement& statement, FrameData* frame_data ) :
      statement_( statement ), frame_data_( frame_data ), current_block_position_( 0 ) {}

    public:
      BmsRandom::IfStatement& statement_;
      FrameData*              frame_data_;
      unsigned int            current_block_position_;
    };
    std::stack<MainFrame*> main_stack_;
  };
}

#endif // BMSBEL_PARSER_FRAME_H_
