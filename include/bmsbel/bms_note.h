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
	const static int NOTE_NONE;
	const static int NOTE_NORMAL;
	const static int NOTE_LNSTART;
	const static int NOTE_LNEND;
	const static int NOTE_MINE;
	const static int NOTE_HIDDEN;

public:
	BmsNote();		// NONEtype note
	BmsNote(int type, int channel, const BmsWord& value);
	int type;		// available note type - refered on top
	int channel;	// key

	BmsWord value;

	// get key index from channel data
	int GetKey() const;
	bool IsVisible() const;
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
	std::vector<BmsNote>& operator [](int channel);
	std::vector<BmsNote>& operator [](const BmsWord& word);
	BmsNote* SetNoteData(const BmsNote& note, int idx);
	std::vector<BmsNote>::iterator Begin(int channel);
	std::vector<BmsNote>::iterator End(int channel);

};
