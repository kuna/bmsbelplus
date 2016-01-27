#include "bmsbel\bms_bar.h"

#include "bmsbel\bms_exception.h"

// -- BmsBarManager ------------------------------------------------------
BmsBarManager::BmsBarManager(void)
{
	// fill all array with default bar count
	for (int i = 0; i < BmsConst::BAR_MAX_COUNT; i++)
		barcount_[i] = BmsConst::BAR_DIVISION_COUNT_MAX;
}

unsigned int
BmsBarManager::operator [](unsigned int pos)
{
	return barcount_[pos];
}

void
BmsBarManager::SetMeasureLength(unsigned int pos, double l) {
	if (l < 0) return;
	barcount_[pos] *= l;
}

double 
BmsBarManager::GetMeasureLength(unsigned int pos) {
	return (double)BmsConst::BAR_DIVISION_COUNT_MAX / barcount_[pos];
}


unsigned int
BmsBarManager::GetMeasureByBarNumber(unsigned int pos) const
{
	unsigned int current = 0;
	for (unsigned int i = 0; i < pos; ++i) {
		current += barcount_[i];
	}
	return current;
}

unsigned int
BmsBarManager::GetBarNumberByMeasure(unsigned int pos) const
{
	unsigned int bar = 0;
	for (unsigned int current = 0; bar <= BmsConst::BAR_MAX_VALUE; ++bar) {
		if (current > pos) {
			break;
		}
		current += barcount_[bar];
	}
	return bar - 1;
}

double
BmsBarManager::GetPosFromBarNumber(unsigned int bar) const {

}

double
BmsBarManager::GetPosFromBarNumber(double bar) const {

}