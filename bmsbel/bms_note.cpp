#include "bmsbel\bms_note.h"

int BmsNote::NOTE_NONE = 0;
int BmsNote::NOTE_NORMAL = 1;
int BmsNote::NOTE_LNSTART = 2;
int BmsNote::NOTE_LNEND = 3;
int BmsNote::NOTE_MINE = 4;

BmsNote::BmsNote() :
type(NOTE_NONE) {}

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


/*
 * BmsNoteContainer
 *
 *
 */
void BmsNoteContainer::Resize(int size) {
	for (int i = 0; i < 20; i++)
		notes[i].resize(size);
}

int BmsNoteContainer::GetNoteCount() {
	int cnt = 0;
	for (int i = 0; i < 20; i++) {
		for (auto it = notes[i].begin(); it != notes[i].end(); ++it) {
			if (it->type == BmsNote::NOTE_NORMAL || it->type == BmsNote::NOTE_LNSTART)
				cnt++;
		}
	}
	return cnt;
}

int BmsNoteContainer::GetKeys() {
	// TODO
	return 0;
}


std::vector<BmsNote>& BmsNoteContainer::GetNoteArray(int channel) {
	return notes[channel];
}
BmsNote& BmsNoteContainer::GetNoteData(int channel, int idx) {
	return notes[channel][idx];
}
BmsNote* BmsNoteContainer::SetNoteData(int channel, int idx, const BmsNote& note) {
	notes[channel][idx] = note;
	return &notes[channel][idx];
}
std::vector<BmsNote>::iterator BmsNoteContainer::Begin(int channel) {
	return notes[channel].begin();
}
std::vector<BmsNote>::iterator BmsNoteContainer::End(int channel) {
	return notes[channel].end();
}