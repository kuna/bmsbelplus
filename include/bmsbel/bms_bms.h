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
#include "bms_stp.h"

// -- BmsBms -------------------------------------------------------------
class BmsBms {
public:
	BmsBms();
	~BmsBms();

	BmsHeaderTable&		GetHeaders();
	BmsRegistArraySet&	GetRegistArraySet();
	BmsChannelManager&	GetChannelManager();
	BmsBarManager&		GetBarManager();
	BmsTimeManager&		GetTimeManager();
	BmsSTPManager&		GetSTPManager();

	int GetObjectExistsMaxMeasure() const;
	int GetObjectExistsMaxBar() const;
	int GetPlayableMaxBar() const;

	void Merge(const BmsBms& other);

	void Clear(void);

	// time (automatically called when bms is loaded)
	void InvalidateTimeTable();

	/*
	 * @brief returns #TOTAL of this song
	 * you may can set fallback from total calculated from BmsNoteManager.
	 */
	double GetTotal(double fallback = 200);
	double GetIIDXTotal();						// calculated total in IIDX method

	// note / key / bpm (metadata)
	bool IsLongNoteExists();
	bool IsMineNoteExists();
	bool IsDouble();
	int GetKey();
	void GetNoteData(BmsNoteManager &note_manager_);
	double GetBaseBPM();

	// convert bms data to string. save this file to make new bms data.
	std::string ToString(void) const;
	// load (utf8 path; use BmsUtil::ConvertToUtf8 if you're TCHAR)
	bool SaveBmsFile(const char* path);
	// load (utf8 path; use BmsUtil::ConvertToUtf8 if you're TCHAR)
	bool LoadBmsFile(const char* path);
#ifdef USE_MBCS
	bool SaveBmsFile(const wchar_t* path);
	bool LoadBmsFile(const wchar_t* path);
#endif
private:
	BmsHeaderTable		headers_;
	BmsRegistArraySet	array_set_;
	BmsChannelManager	channel_manager_;
	BmsBarManager		bar_manager_;
	BmsTimeManager		time_manager_;
	BmsSTPManager		stp_manager_;

	void GetBPMtable(std::map<BmsWord, double> &extended_bpm_table);
	void GetSTOPtable(std::map<BmsWord, int> &stop_sequence_table);
};

#endif // BMSBEL_BMS_H_
