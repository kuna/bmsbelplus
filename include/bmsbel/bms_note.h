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
	static int NOTE_NONE;
	static int NOTE_NORMAL;
	static int NOTE_LNSTART;
	static int NOTE_LNEND;
	static int NOTE_MINE;

public:
	BmsNote();		// NONEtype note
	BmsNote(int type, int channel, const BmsWord& value);
	int type;		// available note type - refered on top
	int channel;	// key

	BmsWord value;

	// get key index from channel data
	int GetKey();
	bool IsVisible();
};

class BmsNoteContainer {
private:
	std::vector<BmsNote> notes[20];		// 20 key data will be enough
public:
	void Resize(int size);				// this will make data initalized

	// about note metadata
	int GetNoteCount();
	int GetKeys();

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

	// notedata value
	std::vector<BmsNote>& GetNoteArray(int channel);
	std::vector<BmsNote>& GetNoteArray(const BmsWord& word);
	BmsNote& GetNoteData(int channel, int idx);
	BmsNote* SetNoteData(int channel, int idx, const BmsNote& note);
	std::vector<BmsNote>::iterator Begin(int channel);
	std::vector<BmsNote>::iterator End(int channel);
};
