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

  bool IsChannel() const;
  bool IsWavChannel( void ) const;
  bool IsBmpChannel() const;
  bool IsShouldPlayWavChannel(void) const;
  bool IsFirstPlayerNoteChannel(void) const;
  bool IsSecondPlayerNoteChannel(void) const;
  bool IsFirstPlayerChannel(void) const;
  bool IsSecondPlayerChannel(void) const;	// includes invisible/mine note
  bool Is5KeyChannel(void) const;			// DEP
  bool Is7KeyChannel(void) const;			// DEPRECIATED
  bool Is9KeyChannel(void) const;			// DEP
  bool IsLongNoteChannel(void) const;
  bool IsMineNoteChannel(void) const;
  int GetChannelType() const;

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

namespace BmsChannelType {
	enum BMSCHANNEL {
		BGM = 1,
		MEASURELENGTH = 2,
		BPM = 3,
		BGA = 4,
		EXTOBJ = 5,
		BGAPOOR = 6,
		BGALAYER = 7,
		EXTBPM = 8,
		STOP = 9,
		BGALAYER2 = 10,
		FIRSTPLAYER = 11,
		SECONDPLAYER = 21,
		FIRSTPLAYERHIDDEN = 31,
		SECONDPLAYERHIDDEN = 41,
		FIRSTPLAYERLN = 51,
		SECONDPLAYERLN = 61,
		FIRSTPLAYERMINE = 131,
		SECONDPLAYERMINE = 141,
		BGAKEYBOUND = 1001,		// 365
		BGAOPAC = 1002,			// 11
		BGALAYEROPAC = 1003,	// 12
		BGALAYER2OPAC = 1004,	// 13
		BGAPOOROPAC = 1005,		// 14
		BGAARGB = 1006,			// 361
		BGALAYERARGB = 1007,	// 362
		BGALAYER2ARGB = 1008,	// 363
		BGAPOORARGB = 1009,		// 364
	};
}