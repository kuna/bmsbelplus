#include "bmsbel\bms_time.h"
#include "bmsbel\bms_exception.h"

#include <algorithm>
#include <vector>

#define BEGIN(a) ((a).begin()->second)
#define RBEGIN(a) ((a).rbegin()->second)
#define IT(a) ((a)->second)

BmsTimeManager::BmsTimeManager(BmsBarManager& bar_) : bar_(bar_) {
	// add basic(120 BPM) time signature
	array_[0] = BmsTime(0, 0, BmsConst::BMS_BASIC_BPM);
}

void BmsTimeManager::Clear() {
	array_.clear();
}

void BmsTimeManager::Reset() {
	iter_ = array_.begin();
	iternext_ = iter_;
	++iternext_;
}

void BmsTimeManager::Add(int bar, const BmsTime& bmstime) {
	array_[bar] = bmstime;
	Reset();
}

void BmsTimeManager::Delete(int bar) {
	array_.erase(bar);
}

double BmsTimeManager::GetBarFromTime(double time) {
	if (array_.size() < 1) {
		// cannot get proper time
		return 0;
	}
	BmsTime *row;
	// if less then first data
	// then no scroll
	row = &BEGIN(array_);
	if (time < row->time)
		return array_.begin()->first;
	// else then linear assumption
	for (; iternext_ != array_.end();) {
		if (IT(iter_).time <= time && time < IT(iternext_).time) {
			time -= IT(iter_).stop;
			if (time < IT(iter_).time) time = IT(iter_).time;
			double db_dt = (iternext_->first - iter_->first) / (IT(iternext_).time - IT(iter_).time - IT(iter_).stop);
			return iter_->first + db_dt * (time - IT(iter_).time);
		}
		// in case of negative BPM - between positive & negative note are inaccessible (stepmania method)
		// that is, position moves instantly. we don't need to care about that thing.
		// basic algorithm processes acts like that way :)
		++iternext_;
		++iter_;
	}
	// if bigger then last data
	// then calculate bar using last BPM
	// (1 beat consists with MAX_BAR_COUNT, regardless of measure ratio)
	// COMMENT: it won't work well in negative BPM - take care of it.
	row = &RBEGIN(array_);
	if (time >= row->time) {
		double lasttime = time - row->time;
		return array_.rbegin()->first
			+ lasttime / 60 * row->bpm * bar_.GetResolution() / 4;
	}
	// if nothing found, then try again after Reset()
	Reset();
	return GetBarFromTime(time);
}

double BmsTimeManager::GetTimeFromBar(double bar) {
	if (array_.size() < 1) {
		// cannot get proper pos
		return 0;
	}
	BmsTime *row;
	// if less then first data
	// then no scroll
	if (bar < array_.begin()->first)
		return BEGIN(array_).time;
	// if bigger then last bar
	// then calculate time from last BPM
	// (time depends on BPM, regardless of measure ratio)
	if (bar >= array_.rbegin()->first) {
		double lastbar = bar - array_.rbegin()->first;
		return RBEGIN(array_).time
			+ lastbar / bar_.GetResolution() * 4 / RBEGIN(array_).bpm * 60;
	}
	// else then linear assumption
	// CAUTION: consider STOP time
	auto iter__ = (--array_.equal_range(bar).second);
	auto iternext__ = iter__;	iternext__++;
	return IT(iter__).time + IT(iter__).stop +
		(double)(bar - iter__->first) / (iternext__->first - iter__->first) *
		(IT(iternext__).time - IT(iter__).time - IT(iter__).stop);
}

double BmsTimeManager::GetBPMFromTime(double time) {
	// convert time to bar
	return GetBPMFromBar(GetBarFromTime(time));
}

double BmsTimeManager::GetBPMFromBar(double bar) {
	if (bar < 0) return array_.begin()->second.bpm;
	/*
	 * equal_range.first = return key-mapped-object if same key exists, return next when not.
	 * equal_range.second = always return next object
	 * so, --equal_range.second may be the best choice.
	 */
	return (--array_.equal_range(bar).second)->second.bpm;
}

namespace {
	struct _BPMData {
		int barsize;
		double bpm;
	};
	bool _comp(_BPMData &a, _BPMData &b) { return a.barsize < b.barsize; }
}

double BmsTimeManager::GetMediumBPM() {
	std::map<double, int> _bpm;
	std::vector<_BPMData> _bpmarray;
	int prevbar = 0;
	for (auto it = array_.begin(); it != array_.end(); ++it) {
		_bpm[it->second.bpm] += it->first - prevbar;
		prevbar = it->first;
	}
	for (auto it = _bpm.begin(); it != _bpm.end(); ++it) {
		_bpmarray.push_back({ it->first, it->second });
	}
	std::sort(_bpmarray.begin(), _bpmarray.end(), _comp);
	return _bpmarray[_bpmarray.size() / 2].bpm;
}

double BmsTimeManager::GetMaxBPM() {
	double bpm = array_.begin()->second.bpm;
	for (auto it = ++array_.begin(); it != array_.end(); ++it) {
		bpm = std::max(bpm, IT(it).bpm);
	}
	return bpm;
}
 
double BmsTimeManager::GetMinBPM() {
	double bpm = array_.begin()->second.bpm;
	for (auto it = ++array_.begin(); it != array_.end(); ++it) {
		bpm = std::min(bpm, IT(it).bpm);
	}
	return bpm;
}

void BmsTimeManager::SetRate(double freq) {
	for (auto it = array_.begin(); it != array_.end(); ++it) {
		it->second.time *= freq;
	}
}