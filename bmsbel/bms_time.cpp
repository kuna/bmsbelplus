#include "bmsbel\bms_time.h"
#include "bmsbel\bms_exception.h"

#define BEGIN(a) ((a).begin()->second)
#define RBEGIN(a) ((a).rbegin()->second)
#define IT(a) ((a)->second)

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

double BmsTimeManager::GetEndTime() {
	if (array_.size() <= 0)
		return 0;
	return RBEGIN(array_).time;
}

double BmsTimeManager::GetBeatFromTime(double time) {
	if (array_.size() < 1) {
		// cannot get proper time
		return 0;
	}
	BmsTime *row;
	// if less then first data
	// then no scroll
	row = &BEGIN(array_);
	if (time < row->time)
		return row->beat;
	// if bigger then last data
	// then calculate beat from last BPM
	row = &RBEGIN(array_);
	if (time >= row->time) {
		double lasttime = time - row->time;
		return row->beat
			+ lasttime / 60 * row->bpm;
	}
	// else then linear assumption
	for (; iternext_ != array_.end();) {
		if (IT(iter_).time <= time && time < IT(iternext_).time) {
			time -= IT(iter_).stop;
			if (time < IT(iter_).time) time = IT(iter_).time;
			double db_dt = (IT(iternext_).beat - IT(iter_).beat) / (IT(iternext_).time - IT(iter_).time - IT(iter_).stop);
			return IT(iter_).beat + db_dt * (time - IT(iter_).time);
		}
		++iternext_;
		++iter_;
	}
	// if nothing found, then try again after Reset()
	Reset();
	return GetBeatFromTime(time);
}

double BmsTimeManager::GetPosFromTime(double time) {
	if (array_.size() < 1) {
		// cannot get proper pos
		return 0;
	}
	BmsTime *row;
	// if less then first data
	// then no scroll
	row = &BEGIN(array_);
	if (time < row->time)
		return row->beat;
	// if bigger then last data
	// then calculate from last BPM
	row = &RBEGIN(array_);
	if (time >= row->time) {
		double lasttime = time - row->time;
		return row->pos
			+ lasttime / 60 * row->bpm;
	}
	// else then linear assumption
	for (; iternext_ != array_.end();) {
		if (IT(iter_).time <= time && time < IT(iternext_).time) {
			time -= IT(iter_).stop;
			if (time < IT(iter_).time) time = IT(iter_).time;
			double db_dt = (IT(iternext_).beat - IT(iter_).beat) / (IT(iternext_).time - IT(iter_).time - IT(iter_).stop);
			return IT(iter_).beat + db_dt * (time - IT(iter_).time);
		}
		++iternext_;
		++iter_;
	}
	// if nothing found, then try again after Reset()
	Reset();
	return GetBeatFromTime(time);
}

int BmsTimeManager::GetBarNumberFromTime(double time) {
	if (array_.size() < 1) {
		return 0;
	}
	// if less then first data
	// then no scroll
	if (time < array_[0].time)
		return 0;

	for (int i = 1; i < array_.size(); i++) {
		if (array_[i - 1].time <= time && time < array_[i].time) {
			return i - 1;
		}
	}
	return GetSize() - 1;
}