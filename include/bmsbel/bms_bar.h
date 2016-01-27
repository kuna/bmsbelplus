#ifndef BMSBEL_BAR_H_
#define BMSBEL_BAR_H_

#include <map>

#include "bms_define.h"

class BmsBarManager;

/*
 * @description
 * manages bar/time related data(bar length, bpm of bar position)
 */

/*
 * @comment
 * by changing structure of BmsBuffer, big number of each channelbar isn't much for memory or cpu cost.
 * so, now Bar length is always BmsConst::BAR_DIVISION_COUNT_MAX
 * (by setting this, resetting bar size is nearly unnecessary. that'll make cpu cost less).
 */

struct BmsTime {
	BmsTime() {}
	BmsTime(double time, double stop, double bpm) :
		time(time), stop(stop), bpm(bpm) {}
	double time;
	double stop;
	/** @brief rendering position (1 per screen height). calculated from bar number. */
	double bpm;
};

// -- BmsBarManager ------------------------------------------------------
class BmsBarManager {
public:
	BmsBarManager();

	// returns bar count by measure index
	unsigned int operator [](unsigned int measure);

	void SetMeasureLength(unsigned int measure, double l);
	double GetMeasureLength(unsigned int measure);

	unsigned int GetMeasureByBarNumber(unsigned int bar) const;
	unsigned int GetBarNumberByMeasure(unsigned int measure) const;

	// about some basic metadata
	// this is different from lastnote bar, get position of 
	//double GetEndBar();

	// before you get any information about time or something,
	// convert time into bar position
	double GetBarFromTime(double sec);
	double GetBarFromBeat(double beat);
	int GetBarNumberFromTime(double sec);

	// get absolute position from bar position
	double GetPosFromBar(double bar);
	double GetTimeFromBar(double bar);

	// get beat or time from bar number
	double GetBeatFromBarNumber(unsigned int bar);
	double GetTimeFromBarNumber(unsigned int bar);

	// bpm
	double GetBPMFromTime(double time);
	double GetBPMFromBar(double bar);
	double GetMediumBPM();
	double GetMaxBPM();
	double GetMinBPM();

	// set time signature
	void DeleteBPM(unsigned int bar);
	void SetBPM(unsigned int bar, double bpm);
	void DeleteSTOP(unsigned int bar);
	void SetSTOP(unsigned int bar, double stop);

	// reset iterator pos to first (automatically called)
	void Reset();
private:
	/** @brief stores each bar count per measure */
	unsigned int barcount_[BmsConst::BAR_MAX_COUNT];
	/** @brief stores time/stop information per bar number */
	std::map<int, BmsTime> array_;

	// for fast iteration
	std::map<int, BmsTime>::iterator iter_;
	std::map<int, BmsTime>::iterator iternext_;
};

#endif // BMSBEL_BAR_H_
