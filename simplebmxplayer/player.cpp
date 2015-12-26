#include "player.h"
#include "define.h"
#include <time.h>

const int Grade::JUDGE_PGREAT = 5;
const int Grade::JUDGE_GREAT = 4;
const int Grade::JUDGE_GOOD = 3;
const int Grade::JUDGE_POOR = 2;
const int Grade::JUDGE_BAD = 1;
const int Grade::JUDGE_EARLY = 10;
const int Grade::JUDGE_LATE = 11;

const int Grade::GRADE_AAA = 8;
const int Grade::GRADE_AA = 7;
const int Grade::GRADE_A = 6;
const int Grade::GRADE_B = 5;
const int Grade::GRADE_C = 4;
const int Grade::GRADE_D = 3;
const int Grade::GRADE_E = 2;
const int Grade::GRADE_F = 1;

Grade::Grade() : Grade(0) {}
Grade::Grade(int notecnt): notecnt(notecnt) {}
int Grade::CalculateScore() {
	return grade[Grade::JUDGE_PGREAT] * 2 + grade[Grade::JUDGE_GREAT];
}
double Grade::CalculateRate() {
	return (double)CalculateScore() / notecnt * 100;
}
int Grade::CalculateGrade() {
	double rate = CalculateRate();
	if (rate >= 8.0 / 9)
		return Grade::GRADE_AAA;
	else if (rate >= 7.0 / 9)
		return Grade::GRADE_AA;
	else if (rate >= 6.0 / 9)
		return Grade::GRADE_A;
	else if (rate >= 5.0 / 9)
		return Grade::GRADE_B;
	else if (rate >= 4.0 / 9)
		return Grade::GRADE_C;
	else if (rate >= 3.0 / 9)
		return Grade::GRADE_D;
	else if (rate >= 2.0 / 9)
		return Grade::GRADE_E;
	else
		return Grade::GRADE_F;
}
void Grade::AddGrade(const int type) {
	grade[type]++;
	if (type >= Grade::JUDGE_GOOD) {
		combo++;
		if (maxcombo < combo) maxcombo = combo;
	}
	else {
		combo = 0;
	}
}

// -------------------------------------------------------

int Player::CheckJudgeByTiming(double delta) {
	double abs_delta = abs(delta);
	if (abs_delta <= BmsJudgeTiming::PGREAT)
		return Grade::JUDGE_PGREAT;
	else if (abs_delta <= BmsJudgeTiming::GREAT)
		return Grade::JUDGE_GREAT;
	else if (abs_delta <= BmsJudgeTiming::GOOD)
		return Grade::JUDGE_GOOD;
	else if (abs_delta <= BmsJudgeTiming::BAD)
		return Grade::JUDGE_BAD;
	else if (abs_delta <= BmsJudgeTiming::POOR)
		return Grade::JUDGE_POOR;
	else if (delta < 0)
		return Grade::JUDGE_LATE;
	else
		return Grade::JUDGE_EARLY;
}

bool Player::IsNoteAvailable(int notechannel) {
	return (noteindex[notechannel] >= 0);
}

int Player::GetAvailableNoteIndex(int notechannel, int start) {
	// we also ignore invisible note!
	for (int i = start; i < bmsnote[notechannel].size(); i++)
		if (bmsnote[notechannel][i].type != BmsNote::NOTE_NONE 
			&& bmsnote[notechannel][i].type != BmsNote::NOTE_HIDDEN)
			return i;
	return -1;	// cannot find next available note
}

int Player::GetNextAvailableNoteIndex(int notechannel) {
	if (!IsNoteAvailable(notechannel))
		return -1;
	return GetAvailableNoteIndex(noteindex[notechannel] + 1);
}

BmsNote* Player::GetCurrentNote(int notechannel) {
	if (!IsNoteAvailable(notechannel))
		return 0;
	return &bmsnote[notechannel][noteindex[notechannel]];
}

bool Player::IsLongNote(int notechannel) {
	return longnotestartpos[notechannel] > -1;
}

void Player::Prepare(BmsBms* bms, int startpos, bool autoplay) {
	// set objects and initalize
	this->bms = bms;
	this->autoplay = autoplay;
	for (int i = 0; i < 20; i++) {
		noteindex[i] = 0;
		longnotestartpos[i] = -1;
		// make keysound
		keysound[i].clear();
		keysound[i].resize(bmstime.GetSize());
		BmsWord lastkeysound(0);
		for (int j = 0; j < bmstime.GetSize(); j++) {
			if (bmsnote[i][j].type == BmsNote::NOTE_HIDDEN
				|| bmsnote[i][j].type == BmsNote::NOTE_LNSTART
				|| bmsnote[i][j].type == BmsNote::NOTE_NORMAL) {
				// fill whole previous notes with current keysound
				lastkeysound = bmsnote[i][j].value;
				for (int k = j; k >= 0; k--) {
					if (keysound[i][k] != BmsWord::MIN) break;
					keysound[i][k] = lastkeysound;
				}
			}
		}
		for (int j = bmstime.GetSize() - 1; j >= 0 && lastkeysound != BmsWord::MIN; j--) {
			if (keysound[i][j] != BmsWord::MIN) break;
			keysound[i][j] = lastkeysound;
		}
	}

	// get time, note table
	bms->CalculateTime(bmstime);
	bms->GetNotes(bmsnote);
	grade = Grade(bmsnote.GetNoteCount());

	// initalize by finding next available note
	int startbar = 0;
	for (; bmstime.GetRow(startbar).beat < startpos && startbar < bmstime.GetSize(); startbar++);
	for (int i = 0; i < 20; i++) {
		noteindex[i] = GetAvailableNoteIndex(i, startbar);
	}
}

//
// game flow
//
void Player::Start() {
	starttime = currenttime = time(0);
}
void Player::Now() {
	// get time
	currenttime = time(0);

	// and recalculate note index/bgm index/position
	double t = GetEclipsedTime();
	int newbar = bmstime.GetBarIndexFromTime(t);
	pos = bmstime.GetAbsBeatFromTime(t);

	// if there's bar change ...
	for (; currentbar < newbar; ++currentbar)
	{
		// call event handler for BGA/BGM
		auto bgmchannel = bms->GetChannelManager()[BmsWord(1)];
		auto bgachannel = bms->GetChannelManager()[BmsWord(4)];
		auto bgachannel2 = bms->GetChannelManager()[BmsWord(7)];
		auto bgachannel3 = bms->GetChannelManager()[BmsWord(10)];
		for (auto it = bgmchannel.Begin(); it != bgmchannel.End(); ++it) {
			BmsWord current_word((**it)[currentbar]);
			if (current_word == BmsWord::MIN)
				continue;
			if (soundfunc)
				soundfunc(current_word);
		}
		for (auto it = bgachannel.Begin(); it != bgachannel.End(); ++it) {
			BmsWord current_word((**it)[currentbar]);
			if (current_word == BmsWord::MIN)
				continue;
			if (bgafunc)
				bgafunc(current_word, BmsChannelType::BGA);
		}
		for (auto it = bgachannel2.Begin(); it != bgachannel2.End(); ++it) {
			BmsWord current_word((**it)[currentbar]);
			if (current_word == BmsWord::MIN)
				continue;
			if (bgafunc)
				bgafunc(current_word, BmsChannelType::BGALAYER);
		}
		for (auto it = bgachannel3.Begin(); it != bgachannel3.End(); ++it) {
			BmsWord current_word((**it)[currentbar]);
			if (current_word == BmsWord::MIN)
				continue;
			if (bgafunc)
				bgafunc(current_word, BmsChannelType::BGALAYER2);
		}
	}

	// check for note judgement
	for (int i = 0; i < 20; i++) {
		if (!IsNoteAvailable(i)) continue;

		// if autoplay, then it'll automatically played
		if (autoplay && GetCurrentNote(i)->type != BmsNote::NOTE_MINE && bmstime.GetRow(noteindex[i]).time < t) {
			BmsNote& note = bmsnote[i][noteindex[i]];
			if (GetCurrentNote(i)->type == BmsNote::NOTE_LNSTART) {
				if (soundfunc) soundfunc(note.value);
				grade.AddGrade(Grade::JUDGE_PGREAT);
				longnotestartpos[i] = noteindex[i];
			}
			else if (GetCurrentNote(i)->type == BmsNote::NOTE_LNEND) {
				grade.AddGrade(Grade::JUDGE_PGREAT);
				if (judgefunc) judgefunc(Grade::JUDGE_GREAT, i);
				longnotestartpos[i] = -1;
			}
			else if (GetCurrentNote(i)->type == BmsNote::NOTE_NORMAL) {
				if (soundfunc) soundfunc(note.value);
				grade.AddGrade(Grade::JUDGE_PGREAT);
				if (judgefunc) judgefunc(Grade::JUDGE_GREAT, i);
			}
			note.type = BmsNote::NOTE_NONE;
			noteindex[i] = GetNextAvailableNoteIndex(i);
		}
		// if note is mine, then lets ignore as fast as we can
		else if (GetCurrentNote(i)->type == BmsNote::NOTE_MINE && bmstime.GetRow(noteindex[i]).time < t) {
			noteindex[i] = GetNextAvailableNoteIndex(i);
		}
		// if not, check timing for poor
		else if (CheckJudgeByTiming(bmstime.GetRow(noteindex[i]).time - t) == Grade::JUDGE_LATE) {
			BmsNote& note = bmsnote[i][noteindex[i]];
			// if LNSTART, also kill LNEND & 2 miss
			if (note.type == BmsNote::NOTE_LNSTART) {
				note.type = BmsNote::NOTE_NONE;
				noteindex[i] = GetNextAvailableNoteIndex(i);
				grade.AddGrade(Grade::JUDGE_POOR);
			}
			// if LNEND, reset longnotestartpos & remove LNSTART note
			else if (note.type == BmsNote::NOTE_LNEND) {
				bmsnote[i][longnotestartpos[i]].type = BmsNote::NOTE_NONE;
				longnotestartpos[i] = -1;
			}
			note.type = BmsNote::NOTE_NONE;
			// make judge
			// (CLAIM) if hidden note isn't ignored by GetNextAvailableNoteIndex(), 
			// you have to hit it or you'll get miss.
			grade.AddGrade(Grade::JUDGE_POOR);
			if (judgefunc) judgefunc(Grade::JUDGE_POOR, i);
			noteindex[i] = GetNextAvailableNoteIndex(i);
		}
	}
}

// key input
void Player::UpKey(int keychannel) {
	// ignore when autoplay
	if (autoplay)
		return;

	// make judge (if you're pressing longnote)
	if (IsLongNote(keychannel)) {
		if (IsNoteAvailable(keychannel) && GetCurrentNote(keychannel)->type == BmsNote::NOTE_LNEND) {
			double t = GetEclipsedTime();
			// make judge
			int judge = CheckJudgeByTiming(t - bmstime.GetRow(noteindex[keychannel]).time);
			if (judgefunc) judgefunc(judge, keychannel);
			// get next note and remove current longnote
			bmsnote[keychannel][longnotestartpos[keychannel]].type = BmsNote::NOTE_NONE;
			bmsnote[keychannel][noteindex[keychannel]].type = BmsNote::NOTE_NONE;
			noteindex[keychannel] = GetNextAvailableNoteIndex(keychannel);
		}
		longnotestartpos[keychannel] = -1;
	}
}
void Player::PressKey(int keychannel) {
	// ignore when autoplay
	if (autoplay)
		return;

	// make judge
	if (IsNoteAvailable(keychannel)) {
		double t = GetEclipsedTime();
		int judge = CheckJudgeByTiming(t - bmstime.GetRow(noteindex[keychannel]).time);
		if (GetCurrentNote(keychannel)->type == BmsNote::NOTE_LNSTART) {
			// store longnote start pos & set longnote status
			grade.AddGrade(judge);
			longnotestartpos[keychannel] = noteindex[keychannel];
		}
		else if (GetCurrentNote(keychannel)->type == BmsNote::NOTE_MINE) {
			// damage, ouch
			if (GetCurrentNote(keychannel)->value == BmsWord::MAX)
				health = 0;
			else
				health -= GetCurrentNote(keychannel)->value.ToInteger();
		}
		else if (GetCurrentNote(keychannel)->type == BmsNote::NOTE_NORMAL) {
			// judge
			grade.AddGrade(judge);
			if (judgefunc) judgefunc(judge, keychannel);
		}
	}

	// make sound
	if (soundfunc) soundfunc(keysound[keychannel][currentbar]);
}

// get status
double Player::GetEclipsedTime() {
	return currenttime - starttime;
}
BmsWord Player::GetCurrentMissBga() {
	return bmstime.GetRow(currentbar).miss;
}
double Player::GetSpeed() { return setting.speed; }

// handler
void Player::SetOnSound(void (*func)(BmsWord)) {
	soundfunc = func;
}
void Player::SetOnBga(void(*func)(BmsWord, BmsChannelType::BMSCHANNEL)) {
	bgafunc = func;
}
void Player::SetOnJudge(void(*func)(int ,int)) {
	judgefunc = func;
}
Grade Player::GetGrade() {
	return grade;
}