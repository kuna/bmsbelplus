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

	unsigned int GetObjectExistsMaxMeasure() const;
	unsigned int GetObjectExistsFirstMeasure() const;
	barindex GetObjectExistsMaxBar() const;
	barindex GetPlayableMaxBar() const;

	void Merge(const BmsBms& other);

	/*
	 * bms modification
	 */
	// copy & repeat only that part - just for training.
	// don't put this function in note class, 'cause this work need to modify ALL channels.
	// need quite much calculation, maybe?
	void Cut(measureindex startmeasure, measureindex endmeasure);
	void Repeat(int repeat);
	void Push(barindex bars);	// how much want to move bar

	void Copy(BmsBms& out);		// produce same BmsBms Object

	void Clear(void);

	// time (automatically called when bms is loaded)
	void InvalidateTimeTable();
	// useful if negative BPM is included
	// (actually don't used much, we need to calculate audio time in the end.)
	double GetEndTime();

	/*
	 * @brief returns #TOTAL of this song
	 * you may can set fallback from total calculated from BmsNoteManager.
	 */
	double GetTotal(double fallback = 200);

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
