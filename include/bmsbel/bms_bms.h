#ifndef BMSBEL_BMS_H_
#define BMSBEL_BMS_H_

#include "bms_define.h"
#include "bms_word.h"
#include "bms_header_table.h"
#include "bms_regist_array.h"
#include "bms_channel.h"
#include "bms_bar.h"
#include "bms_time.h"
#include "bms_note.h"

// -- BmsBms -------------------------------------------------------------
class BmsBms {
public:
	explicit BmsBms(void);
	~BmsBms();

	BmsHeaderTable&		GetHeaders(void);
	BmsRegistArraySet&	GetRegistArraySet(void);
	BmsChannelManager&	GetChannelManager(void);
	BmsBarManager&		GetBarManager(void);

	// get number of measure of last object
	int GetObjectExistsMaxBarPosition(void) const;
	int GetObjectExistsMaxPosition() const;
	int GetPlayableMaxPosition() const;

	void MultiplyBarDivisionCount(unsigned int multiplier);

	void Merge(const BmsBms& other);

	std::wstring ToString(void) const;

	void Clear(void);

	// time
	void CalculateTime(BmsTimeManager& time_manager_);
	double GetBMSLength();

	// get BPM/STOPs
	void GetBPMtable(std::map<BmsWord, double> &extended_bpm_table);
	void GetSTOPtable(std::map<BmsWord, int> &stop_sequence_table);
	double GetBaseBPM();

	// note
	bool IsLongNoteExists();
	bool IsMineNoteExists();
	void GetNotes(BmsNoteContainer &note_manager_);
	int GetKey();

	// get other metedatas
	// must call after CalculateTimeTable()
	//std::wstring& GetTitle();
	//int GetRank();
private:
	BmsHeaderTable		headers_;
	BmsRegistArraySet	array_set_;
	BmsChannelManager	channel_manager_;
	BmsBarManager		bar_manager_;
};

#endif // BMSBEL_BMS_H_
