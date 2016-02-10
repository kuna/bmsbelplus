#include "bmsbel/bms_note.h"
#include "bmsbel/bms_define.h"

#include <memory.h>


namespace {
	int GetLaneIndex(int channel) {
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
		int channel_to_lane[] = {
			0, 1, 2, 3, 4, 5, 0, 8, 6, 7,
			10, 11, 12, 13, 14, 15, 10, 18, 16, 17,
		};
		int b = (channel - 36) / 36;
		int s = channel % 36 % 10;
		return ((b % 2) * 10 + channel_to_lane[s]);
	}
}

//
// BmsNote
//

BmsNote::BmsNote() :
type(NOTE_NONE), status(0) {}

BmsNote::BmsNote(int type, const BmsWord& value, unsigned int time, double pos) :
type(type),
value(value),
pos(pos),
time(time),
status(0) {}



//
// BmsNoteLane
//

int BmsNoteLane::GetNoteCount(bool countLNend, bool countPress, bool countInvisible) {
	int r = 0;
	for (auto it = Begin(); it != End(); ++it) {
		switch (it->second.type) {
		case BmsNote::NOTE_LNEND:
			if (countLNend) r++;
			break;
		case BmsNote::NOTE_PRESS:
			if (countPress) r++;
			break;
		case BmsNote::NOTE_HIDDEN:
			if (countInvisible) r++;
			break;
		case BmsNote::NOTE_MINE:
		case BmsNote::NOTE_NONE:
			break;
		default:
			r++;
		}
	}
	return r;
}

void BmsNoteLane::GetNoteExistBar(std::set<barindex> &barmap) {
	for (Iterator i = Begin(); i != End(); ++i) {
		barmap.insert(i->first);
	}
}

BmsNoteLane::Iterator BmsNoteLane::Begin(int bar) {
	Iterator it = notes_.equal_range(bar).second;
	if (it == notes_.begin()) return it;
	else {
		// must create new iterator as it's lvalue
		Iterator it2 = --it;
		return it2;
	}
}


//
// BmsNoteManager
//

BmsNoteLane& BmsNoteManager::operator[](int v)
{
	return lanes_[v];
}

BmsNoteLane& BmsNoteManager::operator[](BmsWord v)
{
	return lanes_[GetLaneIndex(v.ToInteger())];
}

int BmsNoteManager::GetNoteCount(bool countLNend, bool countPress, bool countInvisible) {
	int cnt = 0;
	for (int i = 0; i < _MAX_NOTE_LANE; i++) {
		cnt += lanes_[i].GetNoteCount(countLNend, countPress, countInvisible);
	}
	return cnt;
}

int BmsNoteManager::GetKeys() {
	int keycnt[_MAX_NOTE_LANE] = { 0, };
	for (int i = 0; i < _MAX_NOTE_LANE; i++)
		keycnt[i] = lanes_[i].GetNoteCount();
	
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

void BmsNoteManager::GetNoteExistBar(std::set<barindex> &barmap) {
	for (int i = 0; i < _MAX_NOTE_LANE; i++)
		lanes_[i].GetNoteExistBar(barmap);
}

/*
 * obsolete code - left for backup
 * (converts channel to lane number)
 */
#if 0
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
#endif

void BmsNoteManager::Random(unsigned int seed, int s, int e) {
	// prepare
	srand(seed);
	int range = e - s + 1;
	// use swap method to make random channel-mapping
	for (int i = s; i <= e; i++) {
		int lane = i;
		int target = i + rand() % range--;
		if (lane != target) swap(lanes_[lane], lanes_[target]);
	}
}

// http://bemaniwiki.com/index.php?beatmania%20IIDX%2022%20PENDUAL%2F%B2%D4%C6%AF%C1%B0%BE%F0%CA%F3
void BmsNoteManager::RRandom(unsigned int seed, int s, int e) {
	// prepare
	srand(seed);
	// mirror or not
	if (rand() % 2) Mirror(s, e);
	// push channel
	int pushcnt = rand() % 10;
	for (int i = 0; i < pushcnt; i++) {
		for (int j = s; j < e; j++) {
			swap(lanes_[j], lanes_[j + 1]);
		}
	}
}

void BmsNoteManager::HRandom(unsigned int seed, int s, int e) {
	// prepare
	srand(seed);
	int keycount = e - s + 1;
	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	// start to iterate all bars
	bool isprev[_MAX_NOTE_LANE] = { false, };	// is there any note placed previously?
	bool isln[_MAX_NOTE_LANE] = { false, };		// is there occupying longnote currently?
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex row = *it;
		bool iscurrent[_MAX_NOTE_LANE] = { false, };
		BmsNote currentrow[_MAX_NOTE_LANE];
		int attempt = 0;
		// initalize row data
		for (int c = s; c <= e; c++) {
			currentrow[c].type = BmsNote::NOTE_NONE;
		}
		// make new row data
		for (int c = s; c <= e; c++) {
			attempt = 0;
			// check previous note data to check valid
			BmsNote currentnote = lanes_[c].Get(row);
			if (currentnote.type == BmsNote::NOTE_NONE) continue;

			/*
			* find new lane to place note
			* only difference(from HRANDOM) is here:
			* we don't check isprev[] (is previously note existed?)
			*/
			int newlane = rand() % keycount + s;
			while (isln[newlane] || iscurrent[newlane] || isprev[newlane]) {
				newlane = (newlane - s + 1) % keycount + s;
				attempt++;
				_ASSERT(attempt <= keycount);	// this shouldn't happen
			}
			// copy previous note data to new temp row
			currentrow[newlane] = currentnote;
			switch (currentnote.type) {
			case BmsNote::NOTE_LNSTART:
				isln[newlane] = true;
				break;
			case BmsNote::NOTE_LNEND:
				isln[newlane] = false;
				break;
			}
			iscurrent[newlane] = true;
		}
		// copy
		for (int c = s; c <= e; c++) {
			lanes_[c].Delete(row);
			if (currentrow[c].type != BmsNote::NOTE_NONE)
				lanes_[c].Set(row, currentrow[c]);
		}
		memcpy(isprev, iscurrent, sizeof(isprev));
	}
}
void BmsNoteManager::SRandom(unsigned int seed, int s, int e) {
	// similar to HRandom
	// prepare
	srand(seed);
	int keycount = e - s + 1;
	int failure = 0;			// failed notecount to randomize
	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	// start to iterate all bars
	bool isprev[_MAX_NOTE_LANE] = { false, };	// is there any note placed previously?
	bool isln[_MAX_NOTE_LANE] = { false, };		// is there occupying longnote currently?
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex row = *it;
		bool iscurrent[_MAX_NOTE_LANE] = { false, };
		BmsNote currentrow[_MAX_NOTE_LANE];
		int attempt = 0;
		// initalize row data
		for (int c = s; c <= e; c++) {
			currentrow[c].type = BmsNote::NOTE_NONE;
		}
		// make new row data
		for (int c = s; c <= e; c++) {
			attempt = 0;
			// check previous note data to check valid
			BmsNote currentnote = lanes_[c].Get(row);
			if (currentnote.type == BmsNote::NOTE_NONE) continue;

			/*
			 * find new lane to place note
			 * only difference(from HRANDOM) is here:
			 * we don't check isprev[] (is previously note existed?)
			 */
			int newlane = rand() % keycount + s;
			while (isln[newlane] || iscurrent[newlane]) {
				newlane = (newlane - s + 1) % keycount + s;
				attempt++;
				_ASSERT(attempt <= keycount);	// this shouldn't happen
			}
			// copy previous note data to new temp row
			currentrow[newlane] = currentnote;
			switch (currentnote.type) {
			case BmsNote::NOTE_LNSTART:
				isln[newlane] = true;
				break;
			case BmsNote::NOTE_LNEND:
				isln[newlane] = false;
				break;
			}
			iscurrent[newlane] = true;
		}
		// copy
		for (int c = s; c <= e; c++) {
			lanes_[c].Delete(row);
			if (currentrow[c].type != BmsNote::NOTE_NONE) 
				lanes_[c].Set(row, currentrow[c]);
		}
		memcpy(isprev, iscurrent, sizeof(isprev));
	}
}
void BmsNoteManager::Mirror(int s, int e) {
	// soooooooo easy one
	// (excludes scratch)
	int keycount = e - s + 1;
	for (int i = 0; i < keycount / 2; i++) {
		int lane = s + i;
		int target = e - i;
		swap(lanes_[lane], lanes_[target]);
	}
}
void BmsNoteManager::MoreLongNote(double ratio, int s, int e) {
	// hmm ...
	// (excludes scratch)
	BmsNote *prev[_MAX_NOTE_LANE] = { 0, };		// previously accessed note
	bool islnstart[_MAX_NOTE_LANE] = { false, };
	for (int c = s; c <= e; c++) {
		for (auto it = lanes_[c].Begin(); it != lanes_[c].End(); ++it) {
			BmsNote& note = it->second;
			switch (note.type) {
			case BmsNote::NOTE_NORMAL:
				prev[c] = &note;
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
	// if some note hadn't finished longnote conversion, cancel it.
	for (int c = 0; c < _MAX_NOTE_LANE; c++) {
		if (islnstart[c]) prev[c]->type = BmsNote::NOTE_NORMAL;
	}
}
void BmsNoteManager::LessLongNote(double ratio, int s, int e) {
	bool islnstart[_MAX_NOTE_LANE] = { false, };
	for (int c = s; c <= e; c++) {
		for (auto it = lanes_[c].Begin(); it != lanes_[c].End(); ++it) {
			BmsNote& note = it->second;
			switch (note.type) {
			case BmsNote::NOTE_LNSTART:
				if (rand() % 100 <= ratio * 100) {
					note.type = BmsNote::NOTE_NORMAL;
					islnstart[c] = true;
				}
				break;
			case BmsNote::NOTE_LNEND:
				if (islnstart[c]) note.type = BmsNote::NOTE_NORMAL;
				islnstart[c] = false;
				break;
			}
		}
	}
}
void BmsNoteManager::AllScratch(int seed, int sc, int s, int e) {
	// get a note randomly from column
	srand(seed);
	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	std::vector<int> channels;
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex bar = *it;
		// SP
		if (lanes_[0].Get(bar).type == BmsNote::NOTE_NONE) {
			// find available note list
			for (int c = s; c <= e; c++) if (c != sc) {
				switch (lanes_[c].Get(bar).type) {
				case BmsNote::NOTE_NORMAL:
					channels.push_back(c);
				}
			}
			// push scratch from available note list
			int nc = channels[rand() % channels.size()];
			lanes_[sc].Set(bar, lanes_[nc].Get(bar));
			lanes_[nc].Delete(bar);
			channels.clear();
		}
	}
}
void BmsNoteManager::MoreNote(double ratio, int s, int e) {
	// creates no-sound-note
	// only create new note in rows where note is already exists
	int keycount = e - s + 1;
	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex bar = *it;
		int existingnote = 0;
		for (int c = s; c <= e; c++) if (c % 10 > 0) {
			switch (lanes_[c].Get(bar).type) {
			case BmsNote::NOTE_NORMAL:
				existingnote++;
			}
		}
		for (; existingnote > 0; --existingnote) {
			int newcol = s + rand() % keycount;
			BmsNote _tmp = lanes_[newcol].Get(bar);
			if (_tmp.type == BmsNote::NOTE_NONE) {
				// TODO need to set tmp note's bar position or it won't drawn correctly
				_tmp.type = BmsNote::NOTE_NORMAL;
				lanes_[newcol].Set(bar, _tmp);
			}
		}
	}
}
void BmsNoteManager::LessNote(double ratio, int s, int e) {
	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex bar = *it;
		for (int c = s; c <= e; c++) {
			switch (lanes_[c].Get(bar).type) {
			case BmsNote::NOTE_NORMAL:
				if (rand() % 100 <= ratio * 100) {
					lanes_[c].Delete(bar);
				}
			}
		}
	}
}
void BmsNoteManager::MoreMine(double ratio, int s, int e) {
	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex bar = *it;
		for (int c = s; c <= e; c++) {
			switch (lanes_[c].Get(bar).type) {
			case BmsNote::NOTE_NORMAL:
				if (rand() % 100 <= ratio * 100) {
					BmsNote mine = lanes_[c].Get(bar);
					mine.type = BmsNote::NOTE_MINE;
					lanes_[c].Set(bar, mine);
				}
			}
		}
	}
}
void BmsNoteManager::LessMine(double ratio, int s, int e) {
	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex bar = *it;
		for (int c = s; c <= e; c++) {
			switch (lanes_[c].Get(bar).type) {
			case BmsNote::NOTE_MINE:
				if (rand() % 100 <= ratio * 100) {
					lanes_[c].Delete(bar);
				}
			}
		}
	}
}
void BmsNoteManager::Flip() {
	for (int i = 0; i < 10; i++) {
		swap(lanes_[i], lanes_[i + 10]);
	}
}
void BmsNoteManager::SP_TO_DP(unsigned int seed) {
	// copy some part of lanes to another
	// this isn't accurate. just for ease.
	int mapping[10];
	srand(seed);

	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex bar = *it;
		// TODO: add cached pos/beat to note object
		if (bar % BmsConst::BAR_DEFAULT_RESOLUTION == 0) {
			// if not during LN && okay
			// then mangle mapping
			mapping[0] = (rand() % 2) * 10;
			for (int c = 1; c < 10; c++)
				mapping[c] = c + (rand() % 2) * 10;
		}
		// just move notes
		for (int c = 0; c < 10; c++) {
			BmsNote note = lanes_[c].Get(bar);
			if (note.type != BmsNote::NOTE_NONE) {
				lanes_[c].Delete(bar);
				lanes_[mapping[c]].Set(bar, note);
			}
		}
	}

	// check ln validation
	FixIncorrectLongNote();
}
void BmsNoteManager::DP_TO_SP() {
	// just merge note at first
	for (int c = 10; c < 20; c++) {
		for (auto it = lanes_[c].Begin(); it != lanes_[c].End(); ++it) {
			lanes_[c - 10].Set(it->first, it->second);
		}
		lanes_[c].Clear();
	}

	// after merge, check LN validation.
	FixIncorrectLongNote();
}
void BmsNoteManager::FixIncorrectLongNote() {
	// there's may be 2 cases -
	// - LN_START after LN_START / LN_END after LN_END
	// - NORMAL_NOTE during LN
	// first case must be fixed.
	// second case well.. removing note isn't solution because keysound will be deleted.
	// (you'll get miss if you don't hit not during LN... or act like HELLCHARGE note.)
	bool isln[20] = { false, };
	std::set<barindex> notemap;
	GetNoteExistBar(notemap);
	for (auto it = notemap.begin(); it != notemap.end(); ++it) {
		barindex bar = *it;
		for (int c = 0; c < 20; c++) {
			BmsNote _tmp = lanes_[c].Get(bar);
			/*
			 * case: LNSTART reached, but currently considered as longnote status
			 * solution: ignore current note
			 */
			if (_tmp.type == BmsNote::NOTE_LNSTART) {
				if (isln[c] == true) {
					lanes_[c].Delete(bar);
				}
				isln[c] = true;
			}
			/*
			 * case: LNEND reached, but current note wasn't longnote.
			 * solution: set NORMAL
			 */
			else if (_tmp.type == BmsNote::NOTE_LNEND) {
				if (isln[c] == false) {
					auto nextit = it;
					if (++nextit == notemap.end())
						_tmp.type = BmsNote::NOTE_NORMAL;
					else
						_tmp.type = BmsNote::NOTE_LNSTART;
					lanes_[c].Set(bar, _tmp);
				}
				isln[c] = false;
			}
		}
	}
}
void BmsNoteManager::Battle() {
	// copy SP to DP position
	for (int i = 0; i < 10; i++)
		lanes_[i + 10] = lanes_[i];
}

double BmsNoteManager::GetTotalFromNoteCount() {
	return GetTotalFromNoteCount(GetNoteCount());
}

double BmsNoteManager::GetTotalFromNoteCount(int notecount) {
	// 7.605*NOTES/(0.01*NOTES+6.5)
	// 260 is applied when a value of #TOTAL becomes smaller than 260
	double r = 7.605 * notecount / (0.01 * notecount + 6.5);
	if (r < 260) r = 260;
	return r;
}
