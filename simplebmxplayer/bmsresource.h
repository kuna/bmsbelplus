#pragma once

#include <vector>

template <typename WAV, typename BMP>
class BmsWavResource {
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
	BmsWavResource();
	bool IsWAVLoaded(int channel);
	WAV* GetWAV(int channel);
	void SetWAV(int channel, WAV* wav);
	bool IsBMPLoaded(int channel);
	WAV* GetBMP(int channel);
	void SetBMP(int channel, BMP* wav);
private:
	BmsWav wav_table[1296];
	BmsBmp bmp_table[1296];
};

template <typename WAV, typename BMP>
BmsWavResource<WAV, BMP>::BmsWav::BmsWav() { wav = 0; }

template <typename WAV, typename BMP>
BmsWavResource<WAV, BMP>::BmsWav::~BmsWav() { delete wav; }

template <typename WAV, typename BMP>
bool BmsWavResource<WAV, BMP>::BmsWav::IsLoaded() { return wav; }

template <typename WAV, typename BMP>
WAV* BmsWavResource<WAV, BMP>::BmsWav::GetWAV() { return wav; }

template <typename WAV, typename BMP>
void BmsWavResource<WAV, BMP>::BmsWav::SetWAV(WAV *wav) { this->wav = wav; }

// -------------------------------------------------

template <typename WAV, typename BMP>
BmsWavResource<WAV, BMP>::BmsBmp::BmsBmp() { wav = 0; }

template <typename WAV, typename BMP>
BmsWavResource<WAV, BMP>::BmsBmp::~BmsBmp() { delete bmp; }

template <typename WAV, typename BMP>
bool BmsWavResource<WAV, BMP>::BmsBmp::IsLoaded() { return bmp; }

template <typename WAV, typename BMP>
WAV* BmsWavResource<WAV, BMP>::BmsBmp::GetBMP() { return bmp; }

template <typename WAV, typename BMP>
void BmsWavResource<WAV, BMP>::BmsBmp::GetBMP(BMP *bmp) { this->bmp = bmp; }

// -------------------------------------------------

template <typename WAV, typename BMP>
BmsWavResource<WAV, BMP>::BmsWavResource() {  }

template <typename WAV, typename BMP>
BMP* BmsWavResource<WAV, BMP>::GetWAV(int channel) {
	if (bmp_table[channel].IsLoaded())
		return bmp_table[channel].GetWAV();
	else
		return 0;
}

template <typename WAV, typename BMP>
void BmsWavResource<WAV, BMP>::SetWAV(int channel, BMP* wav) {
	bmp_table[channel].SetWAV(wav);
}

template <typename WAV, typename BMP>
bool BmsWavResource<WAV, BMP>::IsLoaded(int channel) {
	return bmp_table[channel].IsLoaded();
}