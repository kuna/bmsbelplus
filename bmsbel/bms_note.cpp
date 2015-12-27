#include "bmsbel\bms_note.h"

const int BmsNote::NOTE_NONE ;
const int BmsNote::NOTE_NORMAL;
const int BmsNote::NOTE_LNSTART;
const int BmsNote::NOTE_LNEND;
const int BmsNote::NOTE_MINE;
const int BmsNote::NOTE_HIDDEN;

BmsNote::BmsNote() :
type(NOTE_NONE) {}

BmsNote::BmsNote(int type, int channel, const BmsWord& value) :
type(type),
channel(channel),
value(value) {}

int BmsNote::GetKey() const {
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
	int b = (channel - 36) / 36;
	int s = channel % 36 % 10;
	return ((b % 2) * 10 + s);
}

bool BmsNote::IsVisible() const {
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
	int keycnt[20] = { 0, };
	for (int i = 0; i < 20; i++) {
		for (auto it = notes[i].begin(); it != notes[i].end(); ++it) {
			keycnt[it->GetKey()] ++;
		}
	}
	
	// check DP first
	if (keycnt[17])
		return 18;
	else if (keycnt[19] || keycnt[18])
		return 14;
	else if (keycnt[15] || keycnt[14] || keycnt[13] || keycnt[12] || keycnt[11])
		return 10;
	else if (keycnt[7])
		return 9;
	else if (keycnt[9] || keycnt[8])
		return 7;
	else if (keycnt[5] || keycnt[4] || keycnt[3] || keycnt[2] || keycnt[1])
		return 5;

	// unknown ..?
	return 0;
}


std::vector<BmsNote>& BmsNoteContainer::operator [](int channel) {
	return notes[channel];
}
std::vector<BmsNote>& BmsNoteContainer::operator [](const BmsWord& word) {
	int b = (word.ToInteger() - 36) / 36;
	int s = word.ToInteger() % 36 % 10;
	return (*this)[(b % 2) * 10 + s];
}
BmsNote* BmsNoteContainer::SetNoteData(const BmsNote& note, int idx) {
	notes[note.GetKey()][idx] = note;
	return &notes[note.GetKey()][idx];
}
std::vector<BmsNote>::iterator BmsNoteContainer::Begin(int channel) {
	return notes[channel].begin();
}
std::vector<BmsNote>::iterator BmsNoteContainer::End(int channel) {
	return notes[channel].end();
}