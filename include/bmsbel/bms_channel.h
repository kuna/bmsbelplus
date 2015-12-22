#ifndef BMSBEL_CHANNEL_H_
#define BMSBEL_CHANNEL_H_

#include <vector>
#include <map>

#include "bms_define.h"
#include "bms_word.h"
#include "bms_buffer.h"

class BmsChannel;

// -- BmsChannelBuffer ---------------------------------------------------
class BmsChannelBuffer : public BmsBuffer {
private:
  static const int DEFAULT_LENGTH = BmsConst::DEFAULT_BAR_DIVISION_COUNT * 10;

  friend class BmsChannel;
  explicit BmsChannelBuffer( void );

  void ExtendArrayOver( unsigned int pos );

public:
  // これらの関数は範囲を超えてアクセスしたら自動拡張される
  virtual BmsWord& operator []( unsigned int pos );

  virtual void Merge( const BmsBuffer& buffer );
  virtual void Merge( unsigned int start, const BmsBuffer& buffer );

  int GetObjectExistsMaxPosition( unsigned int start ) const;

  void MultiplyBarDivisionCount( unsigned int multiplier );
};


// -- BmsChannel ---------------------------------------------------------
class BmsChannel {
private:
  friend class BmsChannelManager;
  explicit BmsChannel( const BmsWord& channel_number );

public:
  ~BmsChannel();

  void SetChannelNumber( const BmsWord& channel_number );
  BmsWord& GetChannelNumber();

  unsigned int GetBufferCount( void ) const;

  BmsChannelBuffer& MakeNewBuffer( void );

  BmsChannelBuffer& GetBuffer( unsigned int pos );
  BmsChannelBuffer& operator []( unsigned int pos );

  bool Contains( const BmsWord &word ) const;

  int GetObjectExistsMaxPosition( unsigned int start ) const;

  void MultiplyBarDivisionCount( unsigned int multiplier );

  bool IsChannel( void ) const;
  bool IsWavChannel( void ) const;
  bool IsShouldPlayWavChannel(void) const;
  bool IsFirstPlayerNoteChannel(void) const;
  bool IsSecondPlayerNoteChannel(void) const;
  bool IsFirstPlayerChannel(void) const;
  bool IsSecondPlayerChannel(void) const;	// includes invisible/mine note
  bool IsBmpChannel( void ) const;
  bool IsBpmChangeChannel( void ) const;
  bool IsInvisibilityObjectChannel( void ) const;
  bool IsExtendedBpmChangeChannel( void ) const;
  bool IsLongNoteChannel( void ) const;
  bool IsStopSequenceChannel( void ) const;
  bool IsMineChannel(void) const;
  bool Is5KeyChannel(void) const;
  bool Is7KeyChannel(void) const;
  bool Is9KeyChannel(void) const;

  // -- Iterator ---------------------------------------------------------
  typedef std::vector<BmsChannelBuffer*>::iterator Iterator;
  typedef std::vector<BmsChannelBuffer*>::const_iterator ConstIterator;

  Iterator Begin( void );
  Iterator End( void );
  ConstIterator Begin( void ) const;
  ConstIterator End( void ) const;

private:
  BmsWord                        channel_number_;
  std::vector<BmsChannelBuffer*> buffers_;
};


// -- BmsChannelManager --------------------------------------------------
class BmsChannelManager {
public:
  typedef bool (BmsChannel::*BmsChannelConditionJudgeFunction)( void ) const;

  explicit BmsChannelManager( void );

  ~BmsChannelManager();

  unsigned int GetCount( void ) const;

  BmsChannel& GetChannel( const BmsWord &channel_number );
  BmsChannel& operator []( const BmsWord &channel_number );

  void DeleteChannel( const BmsWord &channel_number );

  void Clear( void );

  bool Contains( const BmsWord &word ) const;
  bool Contains( const BmsWord &word, BmsChannelConditionJudgeFunction func ) const;

  int GetObjectExistsMaxPosition( void ) const;
  int GetObjectExistsMaxPosition( BmsChannelConditionJudgeFunction func ) const;

  void MultiplyBarDivisionCount( unsigned int multiplier );

  // -- Iterator ---------------------------------------------------------
  typedef std::map<BmsWord, BmsChannel*>::iterator Iterator;
  typedef std::map<BmsWord, BmsChannel*>::const_iterator ConstIterator;

  Iterator Begin( void );
  Iterator End( void );
  ConstIterator Begin( void ) const;
  ConstIterator End( void ) const;

private:
  std::map<BmsWord, BmsChannel*> channels_;
};

#endif // BMSBEL_CHANNEL_H_
