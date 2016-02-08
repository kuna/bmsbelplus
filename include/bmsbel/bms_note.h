#pragma once

#include "bms_word.h"
#include "bms_define.h"
#include <map>
#include <set>
#include <vector>

/*
 * BmsNote
 * 
 * Compiled note data for play
 */
class BmsNote {
public:
	const static int NOTE_NONE = 0;
	const static int NOTE_NORMAL = 1;
	const static int NOTE_LNSTART = 2;
	const static int NOTE_LNEND = 3;
	const static int NOTE_MINE = 4;
	const static int NOTE_HIDDEN = 5;
	const static int NOTE_PRESS = 6;		// ok when just pressed
	bool operator==(BmsNote& note) { return note.type == type; }
	bool operator!=(BmsNote& note) { return note.type != type; }
public:
	BmsNote();		// NONEtype note
	BmsNote(int type, const BmsWord& value, unsigned int time, double pos);
	int type;		// available note type - refered on top

	// under these are metadatas.
	BmsWord value;
	unsigned int time;
	double pos;		// it's basically calculated, but you may can recalculate (just for CONSTANT speed)
	int status;		// use this for etc data, or miss timing.
	//int bar;
	//int measure;
};

/*
 * @description
 * A lane includes notes, used for rendering note.
 * <pseudocode>
 * // initalize iterator
 * Reset()
 * // update iterator(which is going to be processed) to proper pos
 * for (; Current != End && Current.pos < pos; Next) {}
 * // render from current note
 * for (c = Current; shows in screen; c.next) { Render() }
 * COMMENT: I think there can be many different types of iterator
 *          from implementation method, so I removed inner iterator. make it by your own.
 */
class BmsNoteLane {
public:
	friend void swap(BmsNoteLane &a, BmsNoteLane &b) { a.notes_.swap(b.notes_); }

	// iterator
	typedef std::map<barindex, BmsNote>::iterator Iterator;
	Iterator Begin() { return notes_.begin(); };
	Iterator Begin(int bar);
	Iterator End() { return notes_.end(); };
#if 0
	Iterator Next() { return ++iter_; };		// move inner iterator to next and return that iter.
	Iterator Current() { return iter_; };		// returns current iterator without modifying anything.
	void Reset() { iter_ = notes_.begin(); }	// reset position of current note lane(initalizing)
	void Reset(int bar) { iter_ = notes_.equal_range(bar).first; }
#endif

	// setter / getter
	BmsNote Get(barindex bar) { 
		if (notes_.find(bar) != notes_.end()) return notes_[bar]; 
		else return BmsNote();
	}
	// only allows NOT NONETYPE note.
	void Set(barindex bar, const BmsNote& v) { 
		if (v.type != BmsNote::NOTE_NONE) notes_[bar] = v; 
	}
	void Delete(barindex bar) { notes_.erase(bar); }
	void Clear() { notes_.clear(); }

	// <barpos, notecount(1)>
	// use iterator to get note existing bar
	void GetNoteExistBar(std::set<barindex> &barmap);
	int GetNoteCount(
		bool countLNend = true, 
		bool countPress = true, 
		bool countInvisible = false
	);
private:
	// data
	std::map<barindex, BmsNote> notes_;
#if 0
	// iterator for each lane
	Iterator iter_;
#endif
};

/*
 * @description
 * Store and modify note data for playing.
 * You should access note data as a lane number.
 * - SC : 0 / 10
 * - All the other keys : 1 ~ 8
 * if you want to get note using channel no., use operator[](BmsWord) instead.
 */
#define _MAX_NOTE_LANE	20

class BmsNoteManager {
private:
	BmsNoteLane lanes_[_MAX_NOTE_LANE];
public:
	// about note metadata
	int GetNoteCount(
		bool countLNend = true,
		bool countPress = true,
		bool countInvisible = false
	);
	// @DEPRECIATED. use BmsBms::GetKeys() method.
	int GetKeys();
	// <barpos, notecount(1)>
	// use iterator to get note existing bar
	void GetNoteExistBar(std::set<barindex> &barmap);

	BmsNoteLane& operator[](int i);			// return by lane index
	BmsNoteLane& operator[](BmsWord v);		// return by channel index -> lane

	// about note modifing
	// - better then modifying original bms file,
	//   because we should load that BMS file again if we need to reuse that BMS file.
	//   that's quite resource wasting.
	void Random(unsigned int seed, int s = 1, int e = 7);
	void RRandom(unsigned int seed, int s = 1, int e = 7);
	void HRandom(unsigned int seed, int s = 1, int e = 7);		// no combo note
	void SRandom(unsigned int seed, int s = 1, int e = 7);
	void Mirror(int s = 1, int e = 7);
	void MoreLongNote(double ratio, int s = 1, int e = 7);
	void LessLongNote(double ratio, int s = 1, int e = 7);
	void AllScratch(int seed, int sc = 0, int s = 1, int e = 7);
	void MoreNote(double ratio, int s = 1, int e = 7);
	void LessNote(double ratio, int s = 1, int e = 7);
	void MoreMine(double ratio, int s = 1, int e = 7);
	void LessMine(double ratio, int s = 1, int e = 7);
	void Flip();
	void SP_TO_DP(unsigned int seed);
	void DP_TO_SP();
	void Battle();
	
	// calculated TOTAL (http://hitkey.nekokan.dyndns.info/cmds.htm).
	double GetTotalFromNoteCount(int notecount);
	double GetTotalFromNoteCount();

	void FixIncorrectLongNote();
};
