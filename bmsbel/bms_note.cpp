#include "bmsbel\bms_note.h"

int BmsNote::NOTE_NORMAL = 0;
int BmsNote::NOTE_LNSTART = 1;
int BmsNote::NOTE_LNEND = 2;
int BmsNote::NOTE_MINE = 3;

BmsNote::BmsNote(int type, int channel, const BmsWord& value) :
type(type),
channel(channel),
value(value) {}

int BmsNote::GetKey() {
	// normal note
	// 11 -- 17
	// 21 -- 27
	// invisible note
	// 31 -- 36
	// 41 -- 46
	// longnote (loose)
	// 51 -- 59
	// 61 -- 69
	// minefield
	// D1 -- D9
	// E1 -- E9
	return (channel - 36) % 72;
}

bool BmsNote::IsVisible() {
	return !(channel > 108 && channel < 180);
}