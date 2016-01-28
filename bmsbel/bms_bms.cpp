#include <stdio.h>

#include "bmsbel\bms_bms.h"

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_util.h"
#include "bmsbel\bms_exception.h"

#include <algorithm>

#define ITER_CHANNEL(channel, iter)\
	for (auto iter = GetChannelManager()[channel].GetBuffer().Begin(); iter != GetChannelManager()[channel].GetBuffer().End(); ++iter)

BmsBms::BmsBms(void) :
headers_(),
array_set_(),
channel_manager_(),
bar_manager_()
{
	array_set_.MakeNewArray("WAV");
	array_set_.MakeNewArray("BMP");
	array_set_.MakeNewArray("BPM");
	array_set_.MakeNewArray("STOP");
}

BmsBms::~BmsBms()
{
}

BmsHeaderTable&
BmsBms::GetHeaders(void)
{
	return headers_;
}

BmsRegistArraySet&
BmsBms::GetRegistArraySet(void)
{
	return array_set_;
}

BmsChannelManager&
BmsBms::GetChannelManager(void)
{
	return channel_manager_;
}

BmsBarManager&
BmsBms::GetBarManager(void)
{
	return bar_manager_;
}

int
BmsBms::GetObjectExistsMaxMeasure(void) const
{
	// from max channelbar position, calculate bar(measure) position
	return bar_manager_.GetMeasureByBarNumber(GetObjectExistsMaxBar());
}

int 
BmsBms::GetObjectExistsMaxBar() const
{
	return channel_manager_.GetObjectExistsMaxPosition(&BmsChannel::IsChannel);
}

int 
BmsBms::GetPlayableMaxBar() const
{
	return channel_manager_.GetObjectExistsMaxPosition(&BmsChannel::IsShouldPlayWavChannel);
}


namespace {
	void one_work(const BmsRegistArray& from, BmsRegistArray& to, const BmsWord& word) {
		if (from.IsExists(word)) {
			if (to.IsExists(word)) {
				throw BmsDuplicateHeaderException(from.GetName() + word.ToString());
			}
			to.Set(word, from[word]);
		}
	}
}


// Merge channels with another BmsBms Object
// (not insert/append, just merge.)
void
BmsBms::Merge(const BmsBms& other)
{
	// merge BmsHeaderTable
	// throw exception when duplicated header exists
	for (BmsHeaderTable::Iterator it = other.headers_.Begin(); it != other.headers_.End(); ++it) {
		if (headers_.IsExists(it->first)) {
			throw BmsDuplicateHeaderException(it->first);
		}
		headers_.Set(it->first, it->second.ToString());
	}
	// merge all registed information (#WAV, #BMP, etc channel)
	// if same key exists, then raise exception
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
		BmsWord word(i);
		for (BmsRegistArraySet::ConstIterator it = other.array_set_.Begin(); it != other.array_set_.End(); ++it) {
			if (NOT(array_set_.Exists(it->first))) {
				array_set_.MakeNewArray(it->first);
			}
			if (other.array_set_[it->first].IsExists(word)) {
				if (array_set_[it->first].IsExists(word)) {
					throw BmsDuplicateHeaderException(it->first);
				}
				array_set_[it->first].Set(word, other.array_set_[it->first][word]);
			}
		}
	}
	// merge STP
	for (auto it = other.stp_manager_.Begin(); it != other.stp_manager_.End(); ++it) {
		stp_manager_.Add(it->first, it->second);
	}

	// merge channel
	for (BmsChannelManager::ConstIterator it = other.channel_manager_.Begin(); it != other.channel_manager_.End(); ++it) {
		unsigned int i = 0;
		BmsChannel& current_channel = *it->second;
		for (BmsChannel::ConstIterator current_buffer = current_channel.Begin(); current_buffer != current_channel.End(); ++current_buffer) {\
			channel_manager_[current_channel.GetChannelNumber()][i].Merge(**current_buffer);
			++i;
		}
	}
}


std::string
BmsBms::ToString(void) const
{
	std::string tmp;

	if (headers_.GetCount() > 0) {
		tmp.append(headers_.ToString());
		tmp.append("\n");
	}
	for (BmsRegistArraySet::ConstIterator it = array_set_.Begin(); it != array_set_.End(); ++it) {
		tmp.append(it->second->ToString());
		tmp.append("\n");
	}
	tmp.append(stp_manager_.ToString());
	int max_bar = this->GetObjectExistsMaxBar();
	unsigned int pos = 0;
	for (int i = 0; i <= BmsConst::BAR_MAX_VALUE; ++i) {
		unsigned int current_bar_count = bar_manager_[i];
		double current_bar_ratio = bar_manager_.GetRatio(i);
		// print if bar ratio isn't normal
		if (current_bar_ratio != 1.0) {
			char buf[1024];
			sprintf(buf, "#%03d%ls:%f\n", i, L"02", current_bar_ratio);
			tmp.append(buf);
		}
		if (i > max_bar) {
			// There might be ratio change although no more note left,
			// so don't break the loop but continue. (CHILD commented)
			// オブジェが無くても小節長変更はあるかもしれないのでループは 999 小節までだが
			// チャンネルのオブジェ出力はここまで
			continue;
		}
		// print note object
		for (BmsChannelManager::ConstIterator it = channel_manager_.Begin(); it != channel_manager_.End(); ++it) {
			BmsChannel& current_channel = *it->second;
			for (BmsChannel::ConstIterator current_buffer = current_channel.Begin(); current_buffer != current_channel.End(); ++current_buffer) {
				// get GCD for current measure
				int division = bar_manager_.GetDivision(**current_buffer, i);
				int step = BmsConst::BAR_DIVISION_COUNT_MAX / division;
				std::string object_array_str;
				for (unsigned int k = pos; k < pos + current_bar_count; k += step) {
					object_array_str.append((**current_buffer).Get(k).ToString());
				}
				if (current_channel.GetChannelNumber() == BmsWord("01") ||
					object_array_str.length() > 2 ||
					(object_array_str.length() == 2 && object_array_str != "00")) {
					char buf[1024];
					sprintf(buf, "#%03d%S:", i, current_channel.GetChannelNumber().ToCharPtr());
					tmp.append(buf);
					tmp.append(object_array_str);
					tmp.append("\n");
				}
			}
		}
		pos += current_bar_count;
	}
	return tmp;
}

void
BmsBms::GetBPMtable(std::map<BmsWord, double> &extended_bpm_table)
{
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; i++) {
		BmsWord current_word(i);
		if (this->GetRegistArraySet()["BPM"].IsExists(current_word)) {
			double tmp;
			if (NOT(BmsUtil::StringToFloat(this->GetRegistArraySet()["BPM"][current_word], &tmp)) || tmp == 0.0) {
				throw InvalidFormatAsExtendedBpmChangeValueException(current_word, GetRegistArraySet()["BPM"][current_word]);
			}
			else {
				extended_bpm_table[current_word] = tmp;
			}
		}
	}
}

void
BmsBms::GetSTOPtable(std::map<BmsWord, int> &stop_sequence_table)
{
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; i++) {
		BmsWord current_word(i);
		if (this->GetRegistArraySet()["STOP"].IsExists(BmsWord(i))) {
			int tmp;
			if (NOT(BmsUtil::StringToInteger(this->GetRegistArraySet()["STOP"][current_word], &tmp, 10))) {
				throw InvalidFormatAsStopSequenceException(current_word, GetRegistArraySet()["STOP"][current_word]);
			}
			else {
				stop_sequence_table[current_word] = tmp;
			}
		}
	}
}

double 
BmsBms::GetBaseBPM() {
	double bpm = BmsConst::BMS_BASIC_BPM;
	if (this->GetHeaders().IsExists("BPM")) {
		if ((bpm = this->GetHeaders()["BPM"].ToFloat()) == 0.0) {
			throw InvalidFormatAsBpmHeaderException();
		}
	}
	return bpm;
}

double
BmsBms::GetTotal(double fallback) {
	double r = fallback;
	headers_.Query("TOTAL", &r);
	return r;
}

bool
BmsBms::IsLongNoteExists()
{
	int lntype = 0;
	if (this->GetHeaders().IsExists("LNTYPE")) {
		if (!this->GetHeaders()["LNTYPE"].IsInteger())
			throw InvalidLongNoteType(this->GetHeaders()["LNTYPE"].ToString());
		lntype = this->GetHeaders()["LNTYPE"].ToInteger();
		if (lntype == 2) {
			throw UnsupportedLongNoteType(this->GetHeaders()["LNTYPE"].ToString());
		}
		else {
			return true;
		}
	}
	bool lnchannel = false;
	for (auto it = this->channel_manager_.Begin(); it != this->channel_manager_.End(); ++it) {
		if (it->second->IsLongNoteChannel())
		{
			return true;
		}
	}
	return false;
}

bool
BmsBms::IsMineNoteExists()
{
	for (auto it = this->channel_manager_.Begin(); it != this->channel_manager_.End(); ++it) {
		if (it->second->IsMineNoteChannel())
		{
			return true;
		}
	}
	return false;
}

int
BmsBms::GetKey()
{
	bool isDouble = false;
	int key = 5;
	for (BmsChannelManager::ConstIterator it = GetChannelManager().Begin(); it != GetChannelManager().End(); ++it) {
		// check 2P side
		if (it->second->IsSecondPlayerChannel()) {
			isDouble = true;
		}

		// check 5/7/9key area
		if (it->second->Is5KeyChannel()) {
			// key is already 5, so don't do anything
		} else if (it->second->Is7KeyChannel()) {
			key = std::max(key, 7);
		} else if (it->second->Is9KeyChannel()) {
			key = std::max(key, 9);
		} else {
			if (BmsBelOption::ERROR_ON_UNKNOWN_CHANNEL) {
				throw UnknownChannelException(it->first);
			}
		}
	}

	if (isDouble) {
		return key * 2;
	}
	else {
		return key;
	}
}

//
// private method
// - should called when BmsBarManager is created/modified.
//
void
BmsBms::InvalidateTimeTable()
{
	// clear current time table
	time_manager_.Clear();

	//
	// get BPM/STOP table
	//
	double bpm;
	std::map<BmsWord, double> extended_bpm_table;
	std::map<BmsWord, int> stop_sequence_table;
	bpm = GetBaseBPM();
	GetBPMtable(extended_bpm_table);
	GetSTOPtable(stop_sequence_table);

	//
	// Add Base BPM first
	//
	time_manager_.Add(0, BmsTime(0, 0, GetBaseBPM()));

	//
	// input BPM channel
	//
	ITER_CHANNEL(BmsChannelType::BPM, iter) {
		BmsWord current_word(iter->second);
		if (current_word == BmsWord::MIN) continue;
		int new_bpm;
		if (NOT(BmsUtil::StringToInteger(current_word.ToCharPtr(), &new_bpm, 16))) {
			throw InvalidFormatAsBpmChangeValueException(current_word);
		}
		time_manager_.Add(iter->first, BmsTime(0, 0, new_bpm));
	}
	ITER_CHANNEL(BmsChannelType::EXTBPM, iter) {
		BmsWord current_word(iter->second);
		if (current_word == BmsWord::MIN) continue;
		if (extended_bpm_table.find(current_word) == extended_bpm_table.end()) {
			throw ExtendedBpmChangeEntryNotExistException(current_word);
		}
		else {
			int bpm = extended_bpm_table[current_word];
			time_manager_.Add(iter->first, BmsTime(0, 0, bpm));
		}
	}

	//
	// input STOP/STP channel
	//
	ITER_CHANNEL(BmsChannelType::STOP, iter) {
		BmsWord current_word(iter->second);
		if (current_word == BmsWord::MIN) continue;
		if (stop_sequence_table.find(current_word) == stop_sequence_table.end()) {
			throw StopSequenceEntryNotExistException(current_word);
		}
		else {
			// stop for 1 / 192 beat
			// it doesn't consider the length(Ratio) of that measure.
			double new_stop = (static_cast<double>(stop_sequence_table[current_word]) / 192.0 * 4.0) * (60.0 / bpm);
			if (new_stop > 0) {
				BmsTime t(0, 0, time_manager_.GetBPMFromBar(iter->first));
				t.stop = new_stop;
				time_manager_.Add(iter->first, t);
			}
		}
	}
	for (auto it = GetSTPManager().Begin(); it != GetSTPManager().End(); ++it) {
		unsigned int bar = GetBarManager().GetBarByMeasure(it->first);
		BmsTime t(0, 0, time_manager_.GetBPMFromBar(bar));
		t.stop = it->second / 1000.0;
		time_manager_.Add(bar, t);
	}

	//
	// now calculate time data
	//
	BmsTime *currentsig = 0;
	int prevbar = 0;
	double totaltime = 0;
	for (auto it = time_manager_.Begin(); it != time_manager_.End(); ++it) {
		if (currentsig) {
			int eslapedbar = it->first - prevbar + 1;
			double stop = currentsig->stop;
			double bpm = currentsig->bpm;
			/*
			* BPM: 4 measure( == 1 beat) per minute
			* - convert measure to beat (divide by 4)
			* - convert bpm to spb (second per beat)
			*/
			double time =
				totaltime += stop +
				(60.0 / bpm) / (((double)eslapedbar / BmsConst::BAR_DIVISION_COUNT_MAX) / 4.0);

			it->second.time = time;
		}
		prevbar = it->first;
		currentsig = &it->second;
	}
}

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

void
BmsBms::GetNoteData(BmsNoteManager &note_manager_)
{
	// prepare
	// check longnote type
	int LNtype = 0;
	if (GetHeaders().IsExists("LNTYPE")) {
		if (!GetHeaders()["LNTYPE"].IsInteger())
			throw InvalidLongNoteType(GetHeaders()["LNTYPE"].ToString());
		LNtype = GetHeaders()["LNTYPE"].ToInteger();
	}
	BmsWord lnobj_word(0);
	if (GetHeaders().IsExists("LNOBJ") && BmsWord::CheckConstruction(GetHeaders()["LNOBJ"].ToString())) {
		lnobj_word = BmsWord(GetHeaders()["LNOBJ"].ToString());
	}

	// fill note data
	BmsNote* channelLastNote[BmsConst::WORD_MAX_COUNT] = { 0, };
	for (int c = 0; c < 32; c++) if (c % 16 < 10) {
		// normal note (#LNOBJ)
		if (channel_manager_.Contains(c + 16)) {	// 11 ~ 29
			int channel = c + 16;
			int laneidx = GetLaneIndex(channel);
			int prev = 0;	// where previous note existed bar
			ITER_CHANNEL(channel, iter) {
				int bar = iter->first;
				BmsWord current_word(iter->second);
				if (current_word == lnobj_word && prev) {
					// it means end of the longnote
					// if previous longnote not exists, then don't make it longnote (wrong BMS)
					BmsNote prevnote = note_manager_[laneidx].Get(prev);
					prevnote.type = BmsNote::NOTE_LNSTART;
					note_manager_[laneidx].Set(prev, prevnote);
					note_manager_[laneidx].Set(bar, BmsNote(BmsNote::NOTE_LNEND, current_word));
					prev = 0;
				}
				else {
					if (current_word == lnobj_word)
						printf("Bms Warning: wrong end of the longnote found.\n");
					note_manager_[laneidx].Set(bar, BmsNote(BmsNote::NOTE_NORMAL, current_word));
					prev = bar;
				}
			}
		}
		// long note (#LNTYPE)
		if (channel_manager_.Contains(c + 80)) {	// 51 ~ 69
			int channel = c + 80;
			int laneidx = GetLaneIndex(channel);
			bool isln = false;
			int step;	// used to find #LN
			ITER_CHANNEL(channel, iter) {
				int bar = iter->first;
				BmsWord current_word(iter->second);
				if (!isln) {
					note_manager_[laneidx].Set(bar, BmsNote(BmsNote::NOTE_LNSTART, current_word));
				}
				else {
					if (LNtype == 2) {
						// if next note exists or not 00
						// then it's PRESS note(HELL CHARGE)
						auto iternext = iter; ++iternext;
						if (iternext != GetChannelManager()[channel].GetBuffer().End() && iternext->second != BmsWord(0)) {
							note_manager_[laneidx].Set(bar, BmsNote(BmsNote::NOTE_PRESS, current_word));
						}
						else {
							note_manager_[laneidx].Set(bar, BmsNote(BmsNote::NOTE_LNEND, current_word));
						}
					}
				}
				isln = !isln;
			}
		}
		// mine note
		if (channel_manager_.Contains(c + 64)) {
			int channel = c + 80;
			int laneidx = GetLaneIndex(channel);
			ITER_CHANNEL(channel, iter) {
				int bar = iter->first;
				BmsWord current_word(iter->second);
				note_manager_[laneidx].Set(bar, BmsNote(BmsNote::NOTE_MINE, current_word));
			}
		}
	}
}

void
BmsBms::Clear(void)
{
	headers_.Clear();
	array_set_.Clear();
	channel_manager_.Clear();
	bar_manager_.Clear();
}
