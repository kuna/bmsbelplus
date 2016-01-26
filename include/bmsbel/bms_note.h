#pragma once

#include "bms_word.h"
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

public:
	BmsNote();		// NONEtype note
	BmsNote(int type, const BmsWord& value);
	int type;		// available note type - refered on top

	BmsWord value;
};

/*
 * @description
 * this 
 * and, this class treats note channels differently
 * - SC : 0 / 10
 * - All the other keys : 1 ~ 8
 * if you want to get note using channel no., use operator[](BmsWord) instead.
 */
#define _MAX_NOTE_LANE	20

class BmsNoteManager {
private:
	std::vector<BmsNote> notes[_MAX_NOTE_LANE];		// 20 key data will be enough
public:
	void Resize(int size);				// this will make data initalized

	// about note metadata
	int GetNoteCount(bool longnotedoublecount = true);
	int GetKeys();
	int GetSize();

	// about note modifing
	// - better then modifying original bms file,
	//   because we should load that BMS file again if we need to reuse that BMS file.
	//   that's quite resource wasting.
	void Random(int player, unsigned int seed, bool includeScratch = false);
	void RRandom(int player, unsigned int seed, bool includeScratch = false);
	void HRandom(int player, unsigned int seed, bool includeScratch = false);		// no combo note
	void SRandom(int player, unsigned int seed, bool includeScratch = false);
	void Mirror(int player);
	void MoreLongNote(double ratio);
	void LessLongNote(double ratio);
	void AllScratch(int seed);
	void MoreNote(double ratio);
	void LessNote(double ratio);
	void MoreMine(double ratio);
	void LessMine(double ratio);
	void Flip();
	void SP_TO_DP(unsigned int seed);
	void DP_TO_SP(unsigned int seed);
	void Battle();

	// notedata value
	std::vector<BmsNote>& operator [](int lane);			// this method gives lane as index number.
	std::vector<BmsNote>& operator [](const BmsWord& word);	// this method gives lane channel number.
	BmsNote* SetNoteData(const BmsNote& note, int lane, int bar);
	std::vector<BmsNote>::iterator Begin(int lane);
	std::vector<BmsNote>::iterator End(int lane);
};
