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
	BmsTimeManager&		GetTimeManager();

	// オブジェが存在する最大の小節数を取得
	int GetObjectExistsMaxBarPosition(void) const;

	void MultiplyBarDivisionCount(unsigned int multiplier);
	void CalculateTimeTable();

	void Merge(const BmsBms& other);

	std::wstring ToString(void) const;

	void Clear(void);

	// get BPM/STOPs
	void GetBPMtable(std::map<BmsWord, double> &extended_bpm_table);
	void GetSTOPtable(std::map<BmsWord, int> &stop_sequence_table);

	// bpm
	double GetBPM(double time);
	double GetCommonBPM();
	double GetMaxBPM();
	double GetMinBPM();
	double GetBaseBPM();

	// note
	bool IsLongNoteExists();
	bool IsMineNoteExists();
	void GetNotes(std::vector<BmsNote> &bmsnote_table);
	int GetNoteCount();
	int GetKey();

	// get other metedatas
	// must call after CalculateTimeTable()
	double GetBMSLength();
	//std::wstring& GetTitle();
	//int GetRank();
private:
	BmsHeaderTable		headers_;
	BmsRegistArraySet	array_set_;
	BmsChannelManager	channel_manager_;
	BmsBarManager		bar_manager_;
	BmsTimeManager		time_manager_;

};

#endif // BMSBEL_BMS_H_
