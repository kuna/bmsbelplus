#include "bmsbel\bms_time.h"
#include "bmsbel\bms_exception.h"

void BmsTimeManager::Clear() {
	array_.clear();
}

void BmsTimeManager::Resize(int size) {
	array_.resize(size);
}

void BmsTimeManager::AddRow(const BmsTime& bmstime) {
	if (array_.size() > 0 && array_.back().time >= bmstime.time) {
		throw BmsTimeWrongException();
	}
	array_.push_back(bmstime);
}

void BmsTimeManager::SetRow(const BmsTime& bmstime, int idx) {
	array_[idx] = bmstime;
}

const BmsTime& BmsTimeManager::GetRow(int idx) {
	return array_[idx];
}

double BmsTimeManager::GetEndTime() {
	if (array_.size() <= 0)
		return 0;
	return array_.back().time;
}

double BmsTimeManager::GetBeatFromTime(double time) {
	if (array_.size() < 2) {
		// exception BmsTooLittleRowData
		// but, it shouldn't be happened
		throw BmsParseNoObjectArrayException(0x7fffffff);
	}
	// if less then first data
	// then no scroll
	if (time < array_[0].time)
		return array_[0].beat;

	double db_dt;
	double beat = 0;
	for (int i = 1; i < array_.size(); i++) {
		if (array_[i - 1].time <= time && time < array_[i].time) {
			db_dt = (array_[i].beat - array_[i - 1].beat) / (array_[i].time - array_[i - 1].time);
			return array_[i - 1].beat + db_dt * (time - array_[i - 1].time);
		}
		time -= array_[i].stop;
		if (time < array_[i].time)
			time = array_[i].time;
	}
	db_dt = (array_.back().beat - (array_.end() - 1)->beat) / (array_.back().time - (array_.end() - 1)->time);
	return (array_.end() - 1)->beat + db_dt * (time - (array_.end() - 1)->time);
}

double BmsTimeManager::GetAbsBeatFromTime(double time) {
	if (array_.size() < 2) {
		// exception BmsTooLittleRowData
		// but, it shouldn't be happened
		throw BmsParseNoObjectArrayException(0x7fffffff);
	}
	// if less then first data
	// then no scroll
	if (time < array_[0].time)
		return array_[0].absbeat;
	// (CHECK: in case of stop exists)

	double db_dt;
	for (int i = 1; i < array_.size(); i++) {
		if (array_[i - 1].time <= time && time < array_[i].time) {
			db_dt = (array_[i].absbeat - array_[i - 1].absbeat) / (array_[i].time - array_[i - 1].time - array_[i - 1].stop);
			double t = (time - array_[i - 1].time - array_[i - 1].stop);
			if (t < 0)
				t = 0;
			return array_[i - 1].absbeat + db_dt * t;
		}
	}
	db_dt = (array_.back().absbeat - (array_.end() - 1)->absbeat) / (array_.back().time - (array_.end() - 1)->time);
	double t = (time - (array_.end() - 1)->time - (array_.end() - 1)->stop);
	if (t < 0)
		t = 0;
	return (array_.end() - 1)->absbeat + db_dt * t;
}