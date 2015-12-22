#ifndef BMSBEL_RANDOM_MANAGER_H_
#define BMSBEL_RANDOM_MANAGER_H_

#include <vector>

#include "bms_bms.h"

namespace BmsRandom {
  // -- GenerateRandom ---------------------------------------------------
  class GenerateRandom {
  public:
    explicit GenerateRandom( unsigned int max );
    unsigned int GetMax( void ) const;

  private:
    unsigned int max_;
  };

  class IfStatement;

  // -- Block ------------------------------------------------------------
  class Block {
  public:
    explicit Block( void );
    ~Block();

    BmsBms&                      GetData( void );
    std::vector<GenerateRandom*> GetGenerateRandomArray( void );
    IfStatement*                 GetChild( void );

    IfStatement&    MakeNewChild( unsigned int prerequisite );
    GenerateRandom& MakeNewGenerateRandom( unsigned int max );

    private:
    BmsBms                       data_;
    std::vector<GenerateRandom*> generate_random_array_;
    IfStatement*                 child_;
  };

  // -- IfStatement ------------------------------------------------------
  class IfStatement {
  public:
    explicit IfStatement( unsigned int prerequisite );
    virtual ~IfStatement();

    unsigned int GetPrequisite( void ) const;
    unsigned int GetBlockArraySize( void ) const;
    Block& GetBlock( unsigned int index );
    Block& GetFirstBlock( void );
    Block& GetLastBlock( void );

    Block& MakeNewBlock( void );
    Block& MakeNewBlockWithCopyArrayName( const BmsRegistArraySet& array_set );

  private:
    unsigned int        prerequisite_;
    std::vector<Block*> block_array_;
  };

  // -- RootStatement ----------------------------------------------------
  class RootStatement : public IfStatement {
  public:
    explicit RootStatement( void );
  };
}

#endif // BMSBEL_RANDOM_MANAGER_H_
