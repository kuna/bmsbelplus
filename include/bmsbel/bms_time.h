#include "bms_define.h"
#include <deque>
using namespace std;

struct BmsTime {
	BmsTime() {}
	BmsTime(double time, double beat, double absbeat) :
		time(time), stop(0), beat(beat), absbeat(absbeat) {}
	BmsTime(double time, double stop, double beat, double absbeat) :
		time(time), stop(stop), beat(beat), absbeat(absbeat) {}
	double time;
	double stop;
	double beat;
	double absbeat;
};

// BmsTimeManager
// retains information about time, position, beat (relative to BmsBar)
class BmsTimeManager {
private:
	std::deque<BmsTime> array_;
public:
	void Clear();
	void AddRow(const BmsTime& bmstime);
	const BmsTime& GetRow(int idx);
	double GetAbsBeatFromTime(double time);
	double GetBeatFromTime(double time);
	double GetEndTime();
};