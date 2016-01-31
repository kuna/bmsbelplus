#ifndef BMSBEL_BMS_STP_
#define BMSBEL_BMS_STP_

#include "bms_define.h"
#include "bms_bar.h"

#include <sstream>
#include <map>
#include <stdio.h>

class BmsSTPManager {
public:
	void Clear() { stp_.clear(); }
	void Add(double measure, int time) { stp_[measure] += time; };
	void Delete(double measure) { stp_.erase(measure); };
	int operator[](double measure) { 
		if (stp_.find(measure) != stp_.end()) return stp_[measure]; 
		else return 0; 
	}
	
	std::string ToString() const {
		std::ostringstream ss;
		for (ConstIterator it = Begin(); it != End(); ++it) {
			char fmt[10]; sprintf_s(fmt, "%03.03f", it->first);
			ss << "#STP " << fmt << " " << it->second << "\n";
		}
		return ss.str();
	}

	typedef std::map<double, int>::const_iterator ConstIterator;
	ConstIterator Begin() const { return stp_.begin(); }
	ConstIterator End() const { return stp_.end(); }
	typedef std::map<double, int>::iterator Iterator;
	Iterator Begin() { return stp_.begin(); }
	Iterator End() { return stp_.end(); }
private:
	// double position had changed into bar position
	std::map<double, int>	stp_;
};

#endif
