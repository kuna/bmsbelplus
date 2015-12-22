#include "bms_word.h"

class BmsNote {
public:
	static int NOTE_NORMAL;
	static int NOTE_LN;
	static int NOTE_BOMB;

public:
	BmsNote(int type, int channel, BmsWord value, double beat, double time, double pos);
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