#include "bms_define.h"
#include <deque>
using namespace std;

struct BmsTime {
	BmsTime() {}
	BmsTime(double time, double stop, double beat, double absbeat, double bpm, bool measure) :
		time(time), stop(stop), beat(beat), absbeat(absbeat), bpm(bpm), measure(measure) {}
	double time;
	double stop;
	double beat;
	double absbeat;
	double bpm;
	bool measure;
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
	double GetBarIndexFromTime(double time, int start = 0);
	double GetAbsBeatFromTime(double time);
	double GetBeatFromTime(double time);
	double GetBPMFromTime(double time);
	double GetEndTime();
};