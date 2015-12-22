#pragma once

#include "bms_bms.h"
#include "bms_word.h"
#include <algorithm>
using namespace std;

class BmsObject {
public:
	static int NOTE_NORMAL;
	static int NOTE_LN;
	static int NOTE_BOMB;
	static int MEASURE;
	static int BACKGROUND;

public:
	BmsObject(int type, int channel, BmsWord value, double beat, double time, double pos);
	int type;		// available note type - refered on top
	int channel;	// key

	BmsWord value;
	double beat;
	double absbeat;	// absolute beat from base BPM (related with note position)
	double time;

	// for LN
	BmsWord endvalue;
	double endbeat;
	double endabsbeat;
	double endtime;

	// get key index from channel data
	int GetKey();
};

//
// used for BmsObjectManager::ProcessBMS()
//
struct BmsProcessOption {
	/* TODO: move these options to bms_bms.cpp */
	// ignoreLongNote: (no comment)
	bool ignoreLongNote;
	// ignoreMineNote: (no comment)
	bool ignoreMineNote;

	// fullLongNote: same effect like LR2 (Longnote 100%)
	bool fullLongNote;
	// addNote: between 0.0 ~ 1.0. decide to add more notes (from BGM; if no BGM, notes won't be added)
	double addNote;
	// addMine: between 0.0 ~ 1.0. decide to add more mines (number of mine follows )
	double addMine;
	// fullNormalNote: change longnote to normal note
	bool fullNormalNote;
	// note changed in random.
	// CAUTION: we suggest to use this option in BmsObjectManager (can cause parsing error in #LNOBJ)
	int random;
};

//
// processed BMS data
// for play, or another processing
// It does not contains information about judge, score, and so on.
//
class BmsObjectManager {
private:
	BmsBms& bms;
	std::vector<BmsObject> objects;

	double songlength;		// second time unit
	int notecount;
	int total;
	int maxbpm;
	int minbpm;
	int commonbpm;
	bool is_LN_exists;
	bool is_bomb_exists;

	// process objects from ***objects*** array
	void ProcessObjects();
public:
	// if you don't processObject,
	// It'll only calculate songlength, maxbpm, minbpm, etc...
	// things just related to metadata.
	// [processObjects = FALSE]
	// - will make notecount, is_LN_exists, is_bomb_exists not be calculated. (won't use metadata about note)
	BmsObjectManager(BmsBms& bms, bool processObjects = true);
	void ProcessBMS(BmsBms& bms, bool processObjects = true);

	std::vector<BmsObject*> notes;
	std::vector<BmsObject*> bgms;
	std::vector<BmsObject*> bgas;
	std::vector<BmsObject*> measures;
	std::vector<BmsObject*> bpms;

	double GetBeatFromTime(double timesec) const;
	double GetAbsBeatFromTime(double timesec) const;
	int GetNoteCount();
	double GetSongLength();	// returns second time unit

	// metadata
	std::wstring& GetTitle();
	std::wstring& GetSubtitle();
	std::wstring& GetArtist();
	std::wstring& GetWAVFileName(BmsWord id);
	std::wstring& GetBMPFileName(BmsWord id);
	int GetMaxBPM();
	int GetMinBPM();
	int GetBaseBPM();
	int GetCommonBPM();
	bool IsLongNoteExists();
	bool IsBombExists();

	// mix note channel
	void Random();
	void H_Random();
};