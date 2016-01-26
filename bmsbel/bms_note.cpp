#include "bmsbel\bms_note.h"

const int BmsNote::NOTE_NONE ;
const int BmsNote::NOTE_NORMAL;
const int BmsNote::NOTE_LNSTART;
const int BmsNote::NOTE_LNEND;
const int BmsNote::NOTE_MINE;
const int BmsNote::NOTE_HIDDEN;

BmsNote::BmsNote() :
type(NOTE_NONE) {}

BmsNote::BmsNote(int type, const BmsWord& value) :
type(type),
value(value) {}

/*
 * BmsNoteContainer
 *
 *
 */
void BmsNoteManager::Resize(int size) {
	for (int i = 0; i < _MAX_NOTE_LANE; i++)
		notes[i].resize(size);
}

int BmsNoteManager::GetSize() { return notes[0].size(); }

int BmsNoteManager::GetNoteCount(bool longnotedoublecount) {
	int cnt = 0;
	for (int i = 0; i < _MAX_NOTE_LANE; i++) {
		for (auto it = notes[i].begin(); it != notes[i].end(); ++it) {
			if (it->type == BmsNote::NOTE_NORMAL || it->type == BmsNote::NOTE_LNSTART)
				cnt++;
			if (longnotedoublecount && it->type == BmsNote::NOTE_LNEND)
				cnt++;
		}
	}
	return cnt;
}

int BmsNoteManager::GetKeys() {
	int keycnt[_MAX_NOTE_LANE] = { 0, };
	for (int i = 0; i < _MAX_NOTE_LANE; i++) {
		for (auto it = notes[i].begin(); it != notes[i].end(); ++it) {
			keycnt[i] ++;
		}
	}
	
	// check DP first
	if (keycnt[18])
		return 18;
	else if (keycnt[16] || keycnt[17])
		return 14;
	else if (keycnt[15] || keycnt[14] || keycnt[13] || keycnt[12] || keycnt[11])
		return 10;
	else if (keycnt[8])
		return 9;
	else if (keycnt[6] || keycnt[7])
		return 7;
	else if (keycnt[5] || keycnt[4] || keycnt[3] || keycnt[2] || keycnt[1])
		return 5;

	// unknown ..?
	return 0;
}

namespace {
	int channel_to_lane[] = {
		0, 1, 2, 3, 4, 5, 0, 8, 6, 7,
		10, 11, 12, 13, 14, 15, 10, 18, 16, 17,
	};
}

std::vector<BmsNote>& BmsNoteManager::operator [](int lane) {
	return notes[lane];
}
std::vector<BmsNote>& BmsNoteManager::operator [](const BmsWord& word) {
	int b = (word.ToInteger() - 36) / 36;
	int s = word.ToInteger() % 36 % 10;
	return (*this)[channel_to_lane[(b % 2) * 10 + s]];
}
BmsNote* BmsNoteManager::SetNoteData(const BmsNote& note, int lane, int bar) {
	notes[lane][bar] = note;
	return &notes[lane][bar];
}
std::vector<BmsNote>::iterator BmsNoteManager::Begin(int lane) {
	return notes[lane].begin();
}
std::vector<BmsNote>::iterator BmsNoteManager::End(int lane) {
	return notes[lane].end();
}

void BmsNoteManager::Random(int player, unsigned int seed, bool includeScratch) {
	// get random channels & keys to make channel-mapping
	srand(seed);
	int keycount = GetKeys();
	std::vector<int> channels;
	int mapping[20];
	for (int i = 0; i < _MAX_NOTE_LANE; i++) mapping[i] = i;
	for (int i = includeScratch ? 0 : 1; i <= keycount; i++)
		channels.push_back(player * 10 + i);
	for (int i = includeScratch ? 0 : 1; i <= keycount; i++) {
		int n = rand() % channels.size();
		mapping[player * 10 + i] = channels[n];
		channels.erase(channels.begin() + n);
	}
	// and scramble channels
	std::vector<BmsNote> _cached_notes[_MAX_NOTE_LANE];
	for (int i = 0; i < _MAX_NOTE_LANE; i++) {
		_cached_notes[i] = notes[i];
	}
	for (int i = 0; i < _MAX_NOTE_LANE; i++) {
		notes[i] = _cached_notes[mapping[i]];
	}
}

// http://bemaniwiki.com/index.php?beatmania%20IIDX%2022%20PENDUAL%2F%B2%D4%C6%AF%C1%B0%BE%F0%CA%F3
void BmsNoteManager::RRandom(int player, unsigned int seed, bool includeScratch) {
	// get random channels & keys to make channel-mapping
	srand(seed);
	int keycount = GetKeys();
	int mapping[_MAX_NOTE_LANE] = { 0, };
	for (int i = 0; i < _MAX_NOTE_LANE; i++) mapping[i] = i;
	int push = rand();
	int domirror = rand() % 2;	// mirror or not?
	for (int i = includeScratch ? 0 : 1; i <= keycount; i++) {
		if (domirror) 
			mapping[player * 10 + i] = player * 10 + (keycount - i + push) % (keycount - 1) + 1;
		else
			mapping[player * 10 + i] = player * 10 + (i + push) % (keycount - 1) + 1;
	}
	// and scramble channels
	std::vector<BmsNote> _cached_notes[_MAX_NOTE_LANE];
	for (int i = 0; i < _MAX_NOTE_LANE; i++) {
		_cached_notes[i] = notes[i];
	}
	for (int i = 0; i < _MAX_NOTE_LANE; i++) {
		notes[i] = _cached_notes[mapping[i]];
	}
}
void BmsNoteManager::HRandom(int player, unsigned int seed, bool includeScratch) {
	srand(seed);
	int keycount = GetKeys();
	bool isprev[_MAX_NOTE_LANE] = { false, };	// is there any note placed previously?
	bool isln[_MAX_NOTE_LANE] = { false, };		// is there occupying longnote currently?
	for (int i = 0; i < GetSize(); i++) {
		bool iscurrent[_MAX_NOTE_LANE] = { false, };
		BmsNote currentrow[_MAX_NOTE_LANE];
		int attempt = 0;
		for (int c = player * 10 + includeScratch ? 0 : 1; c < player * 10 + 10; c++) {
			int newlane = rand() % keycount;
			while (isln[newlane] || iscurrent[newlane] || isprev[newlane]) {
				newlane = (newlane + 1) % keycount;
				attempt++;
				if (attempt > keycount) {
					while (iscurrent[newlane]) newlane = (newlane + 1) % keycount;
					break;
				}
			}
			currentrow[newlane] = (*this)[i][c];
			switch ((*this)[i][c].type) {
			case BmsNote::NOTE_LNSTART:
				isln[newlane] = true;
				break;
			case BmsNote::NOTE_LNEND:
				isln[newlane] = false;
				break;
			}
			iscurrent[i] = true;
		}
		// copy
		for (int c = player * 10 + includeScratch ? 0 : 1; c < player * 10 + 10; c++)
			(*this)[i][c] = currentrow[c];
		memcpy(isprev, iscurrent, sizeof(isprev));
	}
}
void BmsNoteManager::SRandom(int player, unsigned int seed, bool includeScratch) {
	// similar to HRandom
	srand(seed);
	int keycount = GetKeys();
	bool isprev[_MAX_NOTE_LANE] = { false, };	// is there any note placed previously?
	bool isln[_MAX_NOTE_LANE] = { false, };		// is there occupying longnote currently?
	for (int i = 0; i < GetSize(); i++) {
		bool iscurrent[_MAX_NOTE_LANE] = { false, };
		BmsNote currentrow[_MAX_NOTE_LANE];
		int attempt = 0;
		for (int c = player * 10 + includeScratch ? 0 : 1; c < player * 10 + 10; c++) {
			int newlane = rand() % keycount;
			while (isln[newlane] || iscurrent[newlane]) {
				newlane = (newlane + 1) % keycount;
				attempt++;
				if (attempt > keycount) {
					while (iscurrent[newlane]) newlane = (newlane + 1) % keycount;
					break;
				}
			}
			currentrow[newlane] = (*this)[i][c];
			switch ((*this)[i][c].type) {
			case BmsNote::NOTE_LNSTART:
				isln[newlane] = true;
				break;
			case BmsNote::NOTE_LNEND:
				isln[newlane] = false;
				break;
			}
			iscurrent[i] = true;
		}
		// copy
		for (int c = player * 10 + includeScratch ? 0 : 1; c < player * 10 + 10; c++)
			(*this)[i][c] = currentrow[c];
		memcpy(isprev, iscurrent, sizeof(isprev));
	}
}
void BmsNoteManager::Mirror(int player) {
	// soooooooo easy one
	int mapping[_MAX_NOTE_LANE] = { 0, };
	int keycount = GetKeys();
	for (int i = 0; i < _MAX_NOTE_LANE; i++) mapping[i] = i;
	for (int i = player * 10 + 1; i < player * 10 + keycount; i++)
		mapping[i] = player * 10 + (keycount - i % keycount) + 1;
	// copy channels
	std::vector<BmsNote> _cached_notes[_MAX_NOTE_LANE];
	for (int i = player * 10 + 1; i < player * 10 + keycount; i++) {
		_cached_notes[i] = notes[i];
	}
	for (int i = player * 10 + 1; i < player * 10 + keycount; i++) {
		notes[i] = _cached_notes[mapping[i]];
	}
}
void BmsNoteManager::MoreLongNote(double ratio) {
	// hmm ...
	BmsNote *prev[_MAX_NOTE_LANE] = { 0, };		// previously accessed note
	bool islnstart[_MAX_NOTE_LANE] = { false, };
	for (int i = 0; i < GetSize(); i++) {
		for (int c = 1; c < _MAX_NOTE_LANE; c++) {
			switch ((*this)[c][i].type) {
			case BmsNote::NOTE_NORMAL:
				prev[c] = &(*this)[c][i];
				if (islnstart[c]) {
					prev[c]->type = BmsNote::NOTE_LNEND;
					islnstart[c] = false;
				}
				else {
					if (rand() % 100 <= ratio * 100) {
						prev[c]->type = BmsNote::NOTE_LNSTART;
						islnstart[c] = true;
					}
				}
			case BmsNote::NOTE_LNSTART:
				if (prev && prev[c]->type == BmsNote::NOTE_LNSTART)
					prev[c]->type = BmsNote::NOTE_NORMAL;
				islnstart[c] = false;
				break;
			}
		}
	}
	for (int c = 0; c < _MAX_NOTE_LANE; c++) {
		if (islnstart[c]) prev[c]->type = BmsNote::NOTE_NORMAL;
	}
}
void BmsNoteManager::LessLongNote(double ratio) {
	bool islnstart[_MAX_NOTE_LANE] = { false, };
	for (int i = 0; i < GetSize(); i++) {
		for (int c = 1; c < _MAX_NOTE_LANE; c++) {
			switch ((*this)[c][i].type) {
			case BmsNote::NOTE_LNSTART:
				if (rand() % 100 <= ratio * 100) {
					(*this)[c][i].type = BmsNote::NOTE_NORMAL;
					islnstart[c] = true;
				}
				break;
			case BmsNote::NOTE_LNEND:
				if (islnstart[c]) (*this)[c][i].type = BmsNote::NOTE_NORMAL;
				islnstart[c] = false;
				break;
			}
		}
	}
}
void BmsNoteManager::AllScratch(int seed) {
	// get a note randomly from column
	srand(seed);
	int keycount = GetKeys();
	int range = keycount >= 10 ? 20 : 10;
	std::vector<int> channels;
	for (int i = 0; i < GetSize(); i++) {
		// if scratch is already existing then go out
		if ((*this)[0][i].type != BmsNote::NOTE_NONE
			|| (*this)[10][i].type != BmsNote::NOTE_NONE)
			continue;
		// find available note list
		for (int c = 1; c < range; c++) {
			switch ((*this)[c][i].type) {
			case BmsNote::NOTE_NORMAL:
				channels.push_back(c);
			}
		}
		// push scratch from available note list
		int sc_channel = 0;
		if (keycount >= 10 && rand() % 2 == 1) sc_channel = 10;
		int nc = channels[rand() % channels.size()];
		(*this)[sc_channel][i].type = BmsNote::NOTE_NORMAL;
		(*this)[sc_channel][i].value = (*this)[nc][i].value;
		(*this)[nc][i].type = BmsNote::NOTE_NONE;

		channels.clear();
	}
}
void BmsNoteManager::MoreNote(double ratio) {
	// creates no-sound-note
	// only create new note in rows where note is already exists
	int keycount = GetKeys();
	int range = keycount >= 10 ? 20 : 10;
	for (int i = 0; i < GetSize(); i++) {
		int existingnote = 0;
		for (int c = 1; c < range; c++) {
			switch ((*this)[c][i].type) {
			case BmsNote::NOTE_NORMAL:
				existingnote++;
			}
		}
		while (existingnote > 0) {
			int newcol = rand() % (range - 1) + 1;
			if ((*this)[newcol][i].type == BmsNote::NOTE_NONE)
				(*this)[newcol][i].type = BmsNote::NOTE_NORMAL;
		}
	}
}
void BmsNoteManager::LessNote(double ratio) {
	//
	for (int i = 0; i < GetSize(); i++) {
		for (int c = 1; c < _MAX_NOTE_LANE; c++) {
			switch ((*this)[c][i].type) {
			case BmsNote::NOTE_NORMAL:
				if (rand() % 100 <= ratio * 100) {
					(*this)[c][i].type = BmsNote::NOTE_NONE;
				}
			}
		}
	}
}
void BmsNoteManager::MoreMine(double ratio) {
	for (int i = 0; i < GetSize(); i++) {
		for (int c = 1; c < _MAX_NOTE_LANE; c++) {
			switch ((*this)[c][i].type) {
			case BmsNote::NOTE_NORMAL:
				if (rand() % 100 <= ratio * 100) {
					(*this)[c][i].type = BmsNote::NOTE_MINE;
				}
			}
		}
	}
}
void BmsNoteManager::LessMine(double ratio) {
	for (int i = 0; i < GetSize(); i++) {
		for (int c = 1; c < _MAX_NOTE_LANE; c++) {
			switch ((*this)[c][i].type) {
			case BmsNote::NOTE_MINE:
				if (rand() % 100 <= ratio * 100) {
					(*this)[c][i].type = BmsNote::NOTE_NORMAL;
				}
			}
		}
	}
}
void BmsNoteManager::Flip() {
	// change 1P & 2P
	int mapping[20] = {
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	};
	// copy channels
	std::vector<BmsNote> _cached_notes[20];
	for (int i = 0; i < 20; i++) {
		_cached_notes[i] = notes[i];
	}
	for (int i = 0; i < 20; i++) {
		notes[i] = _cached_notes[mapping[i]];
	}
}
void BmsNoteManager::SP_TO_DP(unsigned int seed) {
	// well?
	int mapping[20] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	};
	int isln = 0;
	srand(seed);
	BmsNote prev[10];
	for (int i = 0; i < GetSize(); i++) {
		if (i % 256 == 0 && isln == 0) { // TODO
			// mangle it - except during LN
			for (int c = 0; c < 10; c++)
				mapping[i] = c + (rand() % 2) * 10;
		}

		for (int c = 0; c < 10; c++) {
			prev[c] = (*this)[c][i];
			if ((*this)[c][i].type == BmsNote::NOTE_LNSTART) isln++;
			if ((*this)[c][i].type == BmsNote::NOTE_LNEND) isln--;
		}
		for (int c = 0; c < 10; c++) {
			if ((*this)[mapping[c]][i].type == BmsNote::NOTE_NONE)
				(*this)[mapping[c]][i] = prev[c];
		}
	}
}
void BmsNoteManager::DP_TO_SP(unsigned int seed) {
	srand(seed);
	bool isln[10] = { false, };
	for (int i = 0; i < GetSize(); i++) {
		// move SC
		if ((*this)[0][i].type == BmsNote::NOTE_NONE)
			(*this)[0][i] = (*this)[10][0];
		(*this)[10][0].type = BmsNote::NOTE_NONE;

		// move DP LN to SP
		for (int c = 11; c < 20; c++) {
			if ((*this)[c][i].type != BmsNote::NOTE_NONE) {
				int attempt = 0;
				int nc = c % 10;
				for (; attempt < 10; attempt++) {
					if (isln[nc]) {
						if ((*this)[c][i].type != BmsNote::NOTE_LNEND)
							continue;
					}
					if ((*this)[nc][i].type == BmsNote::NOTE_NONE) break;
					nc = (nc + 1) % 10;
				}
				(*this)[nc][i] = (*this)[c][i];
			}
			(*this)[c][i].type = BmsNote::NOTE_NONE;
		}

		// check ln
		for (int c = 0; c < 10; c++) {
			if ((*this)[c][i].type == BmsNote::NOTE_LNSTART) isln[c] = true;
			else if ((*this)[c][i].type == BmsNote::NOTE_LNEND) isln[c] = false;
		}
	}
}
void BmsNoteManager::Battle() {
	// copy SP to DP position
	std::vector<BmsNote> _cached_notes[10];
	for (int i = 0; i < 10; i++) {
		_cached_notes[i] = notes[i];
	}
	for (int i = 10; i < 20; i++) {
		notes[i] = _cached_notes[i - 10];
	}
}