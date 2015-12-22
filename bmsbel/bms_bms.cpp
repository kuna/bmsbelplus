#include <stdio.h>

#include "bmsbel\bms_bms.h"

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_util.h"
#include "bmsbel\bms_exception.h"

#include <algorithm>


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

BmsTimeManager&
BmsBms::GetTimeManager()
{
	return time_manager_;
}


int
BmsBms::GetObjectExistsMaxBarPosition(void) const
{
	int pos = -1;
	int bar = 0;

	for (BmsChannelManager::ConstIterator m_it = channel_manager_.Begin(); m_it != channel_manager_.End(); ++m_it) {
		for (BmsChannel::Iterator c_it = m_it->second->Begin(); c_it != m_it->second->End(); ++c_it) {
			for (int i = pos > 0 ? pos : 0; i < static_cast<int>((*c_it)->GetLength()); ++i) {
				if ((*c_it)->At(i) != BmsWord::MIN && i >= pos) {
					while (pos <= i) {
						pos += bar_manager_[bar].GetLength();
						++bar;
						if (bar > BmsConst::BAR_MAX_VALUE) {
							return bar - 1;
						}
					}
					i = pos;
				}
			}
		}
	}
	return bar - 1;
}


void
BmsBms::MultiplyBarDivisionCount(unsigned int multiplier)
{
	bar_manager_.MultiplyBarDivisionCount(multiplier);
	channel_manager_.MultiplyBarDivisionCount(multiplier);
}


namespace {
	void one_work(const BmsRegistArray& from, BmsRegistArray& to, const BmsWord& word) {
		if (from.IsExists(word)) {
			if (to.IsExists(word)) {
				throw BmsDuplicateHeaderException(from.GetName() + word.ToCharPtr());
			}
			to.Set(word, from[word]);
		}
	}
}


// merge information with another BmsBms Object
void
BmsBms::Merge(const BmsBms& other)
{
	// merge BmsHeaderTable
	// throw exception when duplicated header exists
	for (BmsHeaderTable::Iterator it = other.headers_.Begin(); it != other.headers_.End(); ++it) {
		if (headers_.IsExists(it->first)) {
			throw BmsDuplicateHeaderException(it->first);
		}
		headers_.Set(it->first, it->second);
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

	// merge bar division count
	int lcm = BmsUtil::LCM(bar_manager_.GetBarDivisionCount(), other.bar_manager_.GetBarDivisionCount());
	if (lcm > static_cast<int>(bar_manager_.GetBarDivisionCount())) {
		this->MultiplyBarDivisionCount(lcm / bar_manager_.GetBarDivisionCount());
	}

	// set bar length
	for (unsigned int i = 0; i < BmsConst::BAR_MAX_COUNT; ++i) {
		if (other.bar_manager_[i].GetRatio() != 1.0) {
			if (bar_manager_[i].GetRatio() != 1.0) {
				throw BmsDuplicateBarChangeException(i);
			}
			bar_manager_[i].SetLength(other.bar_manager_[i].GetLength() * (lcm / other.bar_manager_.GetBarDivisionCount()));
		}
	}

	// merge channel
	for (BmsChannelManager::ConstIterator it = other.channel_manager_.Begin(); it != other.channel_manager_.End(); ++it) {
		unsigned int i = 0;
		BmsChannel& current_channel = *it->second;
		for (BmsChannel::ConstIterator current_buffer = current_channel.Begin(); current_buffer != current_channel.End(); ++current_buffer) {
			if (bar_manager_.GetBarDivisionCount() == other.bar_manager_.GetBarDivisionCount()) {
				channel_manager_[current_channel.GetChannelNumber()][i].Merge(**current_buffer);
			}
			else {
				BmsBuffer buf = (*current_buffer)->SubBufferLength(0, (*current_buffer)->GetLength());
				channel_manager_[current_channel.GetChannelNumber()][i].Merge(buf);
			}
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
	int max_bar = this->GetObjectExistsMaxBarPosition();
	unsigned int pos = 0;
	for (int i = 0; i <= BmsConst::BAR_MAX_VALUE; ++i) {
		BmsBar current_bar = bar_manager_[i];
		// 小節長変更出力
		if (current_bar.GetRatio() != 1.0) {
			char buf[1024];
			sprintf(buf, "#%03d%s:%f\n", i, "02", current_bar.GetRatio());
			tmp.append(buf);
		}
		if (i > max_bar) {
			// オブジェが無くても小節長変更はあるかもしれないのでループは 999 小節までだが
			// チャンネルのオブジェ出力はここまで
			continue;
		}

		for (BmsChannelManager::ConstIterator it = channel_manager_.Begin(); it != channel_manager_.End(); ++it) {
			BmsChannel& current_channel = *it->second;
			for (BmsChannel::ConstIterator current_buffer = current_channel.Begin(); current_buffer != current_channel.End(); ++current_buffer) {
				int step = current_bar.GetLength();
				for (unsigned int k = 1; k < current_bar.GetLength(); ++k) {
					if ((**current_buffer)[pos + k] != BmsWord::MIN) {
						step = BmsUtil::GCD(step, k);
					}
				}
				std::string object_array_str;
				for (unsigned int k = pos; k < pos + current_bar.GetLength(); k += step) {
					object_array_str.append((**current_buffer)[k].ToCharPtr());
				}
				if (current_channel.GetChannelNumber() == BmsWord("01") ||
					object_array_str.length() > 2 ||
					(object_array_str.length() == 2 && object_array_str != "00")) {
					char buf[1024];
					sprintf(buf, "#%03d%s:", i, current_channel.GetChannelNumber().ToCharPtr());
					tmp.append(buf);
					tmp.append(object_array_str);
					tmp.append("\n");
				}
			}
		}
		pos += bar_manager_[i].GetLength();
	}
	return tmp;
}


// some new features ...
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
BmsBms::GetBPM(double time) {
	return 0;	// TODO
}

double 
BmsBms::GetCommonBPM() {
	return 0;	// TODO
}

double 
BmsBms::GetMaxBPM() {
	return 0;	// TODO
}

double 
BmsBms::GetMinBPM() {
	return 0;	// TODO
}

double 
BmsBms::GetBaseBPM() {
	double bpm = 120.0;
	if (this->GetHeaders().IsExists("BPM")) {
		if (NOT(BmsUtil::StringToFloat(this->GetHeaders()["BPM"], &bpm)) || bpm == 0.0) {
			throw InvalidFormatAsBpmHeaderException();
		}
	}
	return bpm;
}

bool
BmsBms::IsLongNoteExists()
{
	int lntype = 0;
	if (this->GetHeaders().IsExists("LNTYPE")) {
		if (!BmsUtil::StringToInteger(this->GetHeaders()["LNTYPE"], &lntype, 10)) {
			throw InvalidLongNoteType(this->GetHeaders()["LNTYPE"]);
		}
		else if (lntype == 2) {
			throw UnsupportedLongNoteType(this->GetHeaders()["LNTYPE"]);
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
		if (it->second->IsMineChannel())
		{
			return true;
		}
	}
	return false;
}


void 
BmsBms::GetNotes(std::vector<BmsNote> &bmsnote_table) 
{

}

int 
BmsBms::GetNoteCount() 
{
	// TODO: remove bombs?
	std::vector<BmsNote> bmsnote_table;
	GetNotes(bmsnote_table);
	return bmsnote_table.size();
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
BmsBms::CalculateTimeTable()
{
	// flush previous time table
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
	// get start position and end position (by row)
	//
	int start_channel_position = static_cast<int>(
		GetBarManager().GetChannelPositionByBarNumber(0));
	int end_channel_position =
		GetChannelManager().GetObjectExistsMaxPosition(&BmsChannel::IsShouldPlayWavChannel);

	//
	// scan for BPM/STOP channels
	//
	std::vector<BmsChannelBuffer*> BpmChannelBuffer;
	std::vector<BmsChannelBuffer*> ExtendedBpmChannelBuffer;
	std::vector<BmsChannelBuffer*> StopChannelBuffer;
	for (BmsChannelManager::ConstIterator it = GetChannelManager().Begin(); it != GetChannelManager().End(); ++it) {
		BmsChannel& current_channel = *it->second;
		for (BmsChannel::ConstIterator it2 = current_channel.Begin(); it2 != current_channel.End(); ++it2) {
			if (current_channel.IsBpmChangeChannel())
				BpmChannelBuffer.push_back(*it2);
			else if (current_channel.IsExtendedBpmChangeChannel())
				ExtendedBpmChannelBuffer.push_back(*it2);
			else if (current_channel.IsStopSequenceChannel())
				StopChannelBuffer.push_back(*it2);
		}
	}

	// check current bar
	int barposition = 0;
	int barlength = GetBarManager().At(0).GetLength();
	int barleft = 0;
	//
	// create time data
	//
	double time = 0;
	double absbeat_per_each_row = 1.0 / GetBarManager().GetBarDivisionCount();
	double absbeat = 0;
	double measure = 0;
	double measure_per_each_row = 1.0 / barlength;
	for (int i = 0; i <= end_channel_position; i++) {
		if (barleft == 0) {
			barlength = barleft = GetBarManager().At(++barposition).GetLength();
			measure_per_each_row = 1.0 / barlength;
		}
		barleft--;

		double stop_sequence_time = 0.0;
		for (auto it = BpmChannelBuffer.begin(); it != BpmChannelBuffer.end(); ++it) {
			BmsWord current_word((**it)[i]);
			if (current_word == BmsWord::MIN) continue;
			int new_bpm;
			if (NOT(BmsUtil::StringToInteger(current_word.ToCharPtr(), &new_bpm, 16))) {
				throw InvalidFormatAsBpmChangeValueException(current_word);
			}
			bpm = static_cast<double>(new_bpm);
		}
		for (auto it = ExtendedBpmChannelBuffer.begin(); it != ExtendedBpmChannelBuffer.end(); ++it) {
			BmsWord current_word((**it)[i]);
			if (current_word == BmsWord::MIN) continue;
			if (extended_bpm_table.find(current_word) == extended_bpm_table.end()) {
				throw ExtendedBpmChangeEntryNotExistException(current_word);
			}
			else {
				bpm = extended_bpm_table[current_word];
			}
		}
		for (auto it = StopChannelBuffer.begin(); it != StopChannelBuffer.end(); ++it) {
			BmsWord current_word((**it)[i]);
			if (current_word == BmsWord::MIN) continue;
			if (stop_sequence_table.find(current_word) == stop_sequence_table.end()) {
				throw StopSequenceEntryNotExistException(current_word);
			}
			else {
				// 192 指定で 1 小節分停止
				double new_stop = (static_cast<double>(stop_sequence_table[current_word]) / 192.0 * 4.0) * (60.0 / bpm);
				if (stop_sequence_time == 0.0 || new_stop > stop_sequence_time) {
					stop_sequence_time = new_stop;
				}
			}
		}
		// TODO: add BPM information
		// TODO: add is measure grid? information
		time_manager_.AddRow(BmsTime(time, stop_sequence_time, measure, absbeat));

		measure += measure_per_each_row;
		absbeat += absbeat_per_each_row;
		time += ((60.0 / bpm) /
			(static_cast<double>(GetBarManager().GetBarDivisionCount()) / 4.0));
		time += stop_sequence_time;
	}
}

double
BmsBms::GetBMSLength() {
	return time_manager_.GetEndTime();
}

void
BmsBms::Clear(void)
{
	headers_.Clear();
	array_set_.Clear();
	channel_manager_.Clear();
	bar_manager_.Clear();
	time_manager_.Clear();
}
