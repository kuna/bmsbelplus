#include "bms_define.h"
#include "bms_word.h"
#include <map>

/*
 * @comment
 * timestamp of bms (translated data)
 */

struct BmsTime {
	BmsTime() {}
	BmsTime(double time, double stop, double bpm) :
		time(time), stop(stop), bpm(bpm) {}
	/** @brief incicates accessing time of current time signature (second) */
	double time;
	/** @brief incicates STOP time. don't include this in time value. */
	double stop;
	/** @brief rendering position (1 per screen height). calculated from bar number. */
	double bpm;
};

/*
 * @description
 * retains information about time, position, beat
 * this class only transforms between time <-> bar.
 * if you want to translate bar into beat / pos, use BmsBarManager class.
 * CAUTION: must add at least 1 row (first timestamp)
 *          add last note's timestamp (if you want to GetMediumBPM() work well)
 */
class BmsTimeManager {
public:
	BmsTimeManager();

	void Clear();
	/** @brief Resets private iterator. Automatically called. */
	void Reset();
	/** @brief adds timemarker. Automatically calls Reset() */
	void Add(int bar, const BmsTime& bmstime);
	/** @brief */
	void Delete(int bar);

	// iterator
	typedef std::map<int, BmsTime>::iterator Iterator;
	Iterator Begin() { return array_.begin(); }
	Iterator End() { return array_.end(); }

	// before you get any information about time or something,
	// convert time into bar position
	double			GetBarFromTime(double sec);
	double			GetTimeFromBar(double bar);

	// BPM
	double			GetBPMFromTime(double time);
	double			GetBPMFromBar(double bar);
	double			GetMediumBPM();
	double			GetMaxBPM();
	double			GetMinBPM();
private:
	// std::pair<bar number / time related object>
	std::map<int, BmsTime> array_;

	// for fast iteration
	Iterator iter_;
	Iterator iternext_;
};