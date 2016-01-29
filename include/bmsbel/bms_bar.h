#ifndef BMSBEL_BAR_H_
#define BMSBEL_BAR_H_

#include <map>

#include "bms_define.h"
#include "bms_buffer.h"

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

// -- BmsBarManager ------------------------------------------------------
class BmsBarManager {
public:
	BmsBarManager();

	// set to original state
	void Clear();

	// returns bar count by measure index
	unsigned int operator [](unsigned int measure) const;

	void SetRatio(unsigned int measure, double l);
	double GetRatio(unsigned int measure) const;

	unsigned int	GetMeasureByBarNumber(unsigned int bar) const;
	double			GetMeasureByBar(double bar) const;
	unsigned int	GetBarNumberByMeasure(unsigned int measure) const;
	double			GetBarByMeasure(double measure) const;
	double			GetPosByBar(double bar) const;
	int				GetBarByPos(double pos, int step = 4) const;

	/** @brief returns division(step) of measure */
	unsigned int	GetDivision(const BmsBuffer& channelbuf, unsigned int measure) const;
	/** @brief get resolution of bar */
	unsigned int	GetResolution() const { return barresolution_; }
	/** @brief set resolution of bar */
	void			SetResolution(double d);
private:
	/** @brief stores each bar count per measure */
	unsigned int		barcount_[BmsConst::BAR_MAX_COUNT];
	/** @brief cached for fast bar/measure search <total bar / measure index> */
	std::map<int, int>	barcache_;
	/** @brief calculate cache. called when you SetRatio() */
	void InvalidateCache();
	/*
	 * @brief bars per measure.
	 * basically it's value is BmsConst::MAX_BAR_CONST
	 */
	unsigned int barresolution_;
};

#endif // BMSBEL_BAR_H_
