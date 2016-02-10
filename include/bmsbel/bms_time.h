#include "bms_define.h"
#include "bms_word.h"
#include "bms_bar.h"
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
	BmsTimeManager(BmsBarManager &bar_);

	void Clear();
	/** @brief Resets private iterator. Automatically called. */
	void Reset();
	/** @brief adds timemarker. Automatically calls Reset() */
	void Add(barindex bar, const BmsTime& bmstime);
	/** @brief */
	void Delete(barindex bar);

	// iterator
	typedef std::map<barindex, BmsTime>::iterator Iterator;
	Iterator Begin() { return array_.begin(); }
	Iterator End() { return array_.end(); }

	// before you get any information about time or something,
	// convert time into bar position
	double			GetBarFromTime(double sec);
	// COMMENT: GetTimeFromBar may wrong in negative Bpm;
	// no, it is true in exactly, but suppose in this case:
	// - Bpm is negative, and song ends, so last note has lower timestamp then previous one.
	// in that case, last note bar's timestamp != song's total play time.
	// you might need to find max timestamp by yourself, comparing each other.
	double			GetTimeFromBar(double bar);

	// BPM
	double			GetBPMFromTime(double time);
	double			GetBPMFromBar(barindex bar);
	double			GetMediumBPM();
	double			GetMaxBPM();
	double			GetMinBPM();

	// faster/slower bms speed
	void			SetRate(double freq);
private:
	// bar is somtimes necessary
	BmsBarManager& bar_;

	// std::pair<bar number / time related object>
	std::map<barindex, BmsTime> array_;

	// for fast iteration
	Iterator iter_;
	Iterator iternext_;
};