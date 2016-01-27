#include "bms_define.h"
#include "bms_word.h"
#include <map>

/*
 * @comment
 * you can get bar's absolute(rendering) position
 * by using BarManager::GetBarNumberByMeasure()
 * and GetPosFromBarNumber()
 */

struct BmsTime {
	BmsTime() {}
	BmsTime(double time, double stop, double beat, double pos, double bpm) :
		time(time), stop(stop), beat(beat), pos(pos), bpm(bpm) {}
	double time;
	double stop;
	double beat;
	/** @brief rendering position (1 per screen height). calculated from bar number. */
	double pos;
	double bpm;
};

/*
 * @description
 * retains information about time, position, beat
 * CAUTION: must add at least 1 row
 */
class BmsTimeManager {
private:
	// std::pair<bar number / time related object>
	std::map<int, BmsTime> array_;

	// for fast iteration
	std::map<int, BmsTime>::iterator iter_;
	std::map<int, BmsTime>::iterator iternext_;
public:
	void Clear();
	/** @brief Resets private iterator. Automatically called. */
	void Reset();
	/** @brief adds timemarker. Automatically calls Reset() */
	void Add(int bar, const BmsTime& bmstime);

	int GetBarNumberFromTime(double sec);
	double GetPosFromTime(double sec);
	double GetBeatFromTime(double sec);
	double GetEndTime();

	double GetPosFromBarNumber(int bar);
	double GetTimeFromBarNumber(int bar);

	// bpm
	double GetBPMFromTime(double time);
	double GetCommonBPM();
	double GetMaxBPM();
	double GetMinBPM();

	// bga
	BmsWord GetMissBGAFromTime(double time);
};