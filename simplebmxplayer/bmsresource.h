#pragma once

#include "bmsbel\bms_define.h"
#include <vector>

template <typename WAV, typename BMP>
class BmsResource {
public:
	class BmsWav {
	private:
		WAV *wav;
	public:
		BmsWav();
		~BmsWav();
		bool IsLoaded();
		WAV* GetWAV();
		void SetWAV(WAV* wav);
	};

	class BmsBmp {
	private:
		BMP *bmp;
	public:
		BmsBmp();
		~BmsBmp();
		bool IsLoaded();
		BMP* GetBMP();
		void SetBMP(BMP* wav);
	};

public:
	BmsResource();
	bool IsWAVLoaded(int channel);
	WAV* GetWAV(int channel);
	void SetWAV(int channel, WAV* wav);
	bool IsBMPLoaded(int channel);
	BMP* GetBMP(int channel);
	void SetBMP(int channel, BMP* wav);
	void Clear();
private:
	BmsWav wav_table[BmsConst::WORD_MAX_COUNT];
	BmsBmp bmp_table[BmsConst::WORD_MAX_COUNT];
};

template <typename WAV, typename BMP>
BmsResource<WAV, BMP>::BmsWav::BmsWav() { wav = 0; }

template <typename WAV, typename BMP>
BmsResource<WAV, BMP>::BmsWav::~BmsWav() { if (wav) delete wav; }

template <typename WAV, typename BMP>
bool BmsResource<WAV, BMP>::BmsWav::IsLoaded() { return wav; }

template <typename WAV, typename BMP>
WAV* BmsResource<WAV, BMP>::BmsWav::GetWAV() { return wav; }

template <typename WAV, typename BMP>
void BmsResource<WAV, BMP>::BmsWav::SetWAV(WAV *wav) { this->wav = wav; }

// -------------------------------------------------

template <typename WAV, typename BMP>
BmsResource<WAV, BMP>::BmsBmp::BmsBmp() { bmp = 0; }

template <typename WAV, typename BMP>
BmsResource<WAV, BMP>::BmsBmp::~BmsBmp() { if (bmp) delete bmp; }

template <typename WAV, typename BMP>
bool BmsResource<WAV, BMP>::BmsBmp::IsLoaded() { return bmp; }

template <typename WAV, typename BMP>
BMP* BmsResource<WAV, BMP>::BmsBmp::GetBMP() { return bmp; }

template <typename WAV, typename BMP>
void BmsResource<WAV, BMP>::BmsBmp::SetBMP(BMP *bmp) { this->bmp = bmp; }

// -------------------------------------------------

template <typename WAV, typename BMP>
BmsResource<WAV, BMP>::BmsResource() {  }

// --------- WAV ---------

template <typename WAV, typename BMP>
WAV* BmsResource<WAV, BMP>::GetWAV(int channel) {
	if (wav_table[channel].IsLoaded())
		return wav_table[channel].GetWAV();
	else
		return 0;
}

template <typename WAV, typename BMP>
void BmsResource<WAV, BMP>::SetWAV(int channel, WAV* wav) {
	wav_table[channel].SetWAV(wav);
}

template <typename WAV, typename BMP>
bool BmsResource<WAV, BMP>::IsWAVLoaded(int channel) {
	return wav_table[channel].IsLoaded();
}

// -------- BMP ---------

template <typename WAV, typename BMP>
BMP* BmsResource<WAV, BMP>::GetBMP(int channel) {
	if (bmp_table[channel].IsLoaded())
		return bmp_table[channel].GetBMP();
	else
		return 0;
}

template <typename WAV, typename BMP>
void BmsResource<WAV, BMP>::SetBMP(int channel, BMP* bmp) {
	bmp_table[channel].SetBMP(bmp);
}

template <typename WAV, typename BMP>
bool BmsResource<WAV, BMP>::IsBMPLoaded(int channel) {
	return bmp_table[channel].IsLoaded();
}

// -------- Common ---------
template <typename WAV, typename BMP>
void BmsResource<WAV, BMP>::Clear() {
	for (int i = 0; i < BmsConst::WORD_MAX_COUNT; i++) {
		if (wav_table[i].IsLoaded()) {
			delete wav_table[i].GetWAV();
			wav_table[i].SetWAV(0);
		}
		if (bmp_table[i].IsLoaded()) {
			delete bmp_table[i].GetBMP();
			bmp_table[i].SetBMP(0);
		}
	}
}