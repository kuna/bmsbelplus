#ifndef BMSBEL_PARSER_H_
#define BMSBEL_PARSER_H_

#include <vector>
#include <stack>
#include <queue>
#include <set>

#include "bms_define.h"
#include "bms_bms.h"
#include "bms_random.h"
#include "bms_exception.h"
#include "bms_parser_frame.h"
#include "bms_text_file_reader.h"

namespace BmsParser {
  class Parser;
  class Reactor;

  // -- StartInfo --------------------------------------------------------
  struct StartInfo {
  public:
    explicit StartInfo( BmsBms&                   bms,
                        BmsRandom::RootStatement& root_statement,
                        Reactor&                  reactor );
  public:
    BmsBms&                   bms_;
    BmsRandom::RootStatement& root_statement_;
    Reactor&                  reactor_;
    bool                      make_syntax_tree_;
    std::set<BmsWord>         ignore_channel_set_;
  };

  // -- ParsingInfo ------------------------------------------------------
  struct ParsingInfo {
  public:
    explicit ParsingInfo( StartInfo&  start_info,
                          FrameStack& frame_stack );

  public:
    BmsBms&                   bms_;
    BmsRandom::RootStatement& root_statement_;
    Reactor&                  reactor_;
    bool                      make_syntax_tree_;
    std::set<BmsWord>         ignore_channel_set_;
    std::vector<std::string>  line_data_;
    FrameStack&               frame_stack_;
    std::queue<unsigned int>  random_value_queue_;
    bool                      is_first_parse_;
    unsigned int              line_number_;
    unsigned int              bar_;
    BmsWord                   channel_number_;
    std::string               key_;
    std::string               value_;
    double                    bar_change_ratio_;
    std::vector<BmsWord>      word_array_;
  };

  // -- Reactor ----------------------------------------------------------
  class Reactor {
  public:
    static Reactor DefaultObject;

  public:
    explicit Reactor( void );
    virtual ~Reactor();

    virtual std::string AtDuplicateHeader( Parser& parser,
                                           const std::string header,
                                           const std::string before,
                                           const std::string present );
    virtual double AtDuplicateBarChange( Parser& parser,
                                         unsigned int bar,
                                         double       before,
                                         double       present );
    virtual bool AtParseError( Parser& parser, BmsParseException& e, const std::string& line );
    virtual unsigned int AtGenerateRandom( Parser& parser, unsigned int max );

    virtual void AtLineDataRead( Parser& parser );
    virtual void AtParseStart( Parser& parser );
    virtual void AtLineEnd( Parser& parser );
    virtual void AtParseEnd( Parser& parser );

  private:
    unsigned int random_value_;
  };

  // -- Parser -----------------------------------------------------------
  void Parse( const std::string& filename, StartInfo& start_info );
  void Parse( BmsTextFileReader& reader, StartInfo& start_info );

  class Parser {
    friend void Parse( BmsTextFileReader& reader, StartInfo& start_info );

  public:
    ParsingInfo& GetInfo( void );

  private:
    explicit Parser( StartInfo& start_info, FrameStack& frame_stack );
    void Start( void );

    void ParseDefault( const char* str );
    void ParseComment( const char* str );
    void ParseHeaderOrBar( const char* str );
    void ParseHeaderKey( const char* str );
    void ParseHeaderSeparator( const char* str );
    void ParseHeaderValue( const char* str );
    void WriteDownHeader( const FrameData& data );
    void WriteDownRegistArray( const FrameData& data );
    void ParseBar( const char* str );
    void ParseChannel( const char* str );
    void ParseColon( const char* str );
    void ParseBarChangeValue( const char* str );
    void WriteDownBarChangeValue( const FrameData& data );
    void ParseObjectArray( const char* str );
    void WriteDownObjectArray( const FrameData& data );
    void ParseRandomStatement( void );

    unsigned int DecideBarDivisionCountFromBarChange( unsigned int current, double bar_change_ratio );
    unsigned int DecideBarDivisionCountFromObjectArray( unsigned int current, double bar_change_ratio );

  private:
    ParsingInfo info_;
  };
}

#endif // BMSBEL_PARSER_H_
