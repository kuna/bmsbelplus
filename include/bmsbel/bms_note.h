#pragma once

#include "bms_word.h"
#include <vector>

class BmsNoteContainer {
private:
	std::vector<BmsNote> notes[20];		// 20 key data will be enough
	int notecount;
public:
	void SetSize(int size);				// this will make data initalized

	// about note modifing
	void Random(int player, int seed, bool includeScratch = false);
	void RRandom(int player, int seed, bool includeScratch = false);
	void HRandom(int player, int seed, bool includeScratch = false);		// no combo note
	void SRandom(int player, int seed, bool includeScratch = false);
	void Mirror(int player);
	void AllLongNote(int player);
	void NoLongNote();
	void AllScratch(int player);
	void MoreNote(double ratio);
	void LessNote(double ratio);
	void MoreMine(double ratio);
	void LessMine(double ratio);
	void SP_TO_DP(double ratio);

	std::vector<BmsNote> GetNoteData(int channel);
	std::vector<BmsNote>::iterator Begin(int channel);
	std::vector<BmsNote>::iterator End(int channel);
};

/*
 * BmsNote
 * 
 * Compiled note data for play
 */
class BmsNote {
public:
	static int NOTE_NORMAL;
	static int NOTE_LNSTART;
	static int NOTE_LNEND;
	static int NOTE_MINE;

public:
	BmsNote(int type, int channel, const BmsWord& value);
	int type;		// available note type - refered on top
	int channel;	// key

	BmsWord value;

	// get key index from channel data
	int GetKey();
	bool IsVisible();
};