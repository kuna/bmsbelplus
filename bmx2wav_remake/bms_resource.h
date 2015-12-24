#pragma once

#include <vector>

template <typename WAV>
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

public:
	BmsWavResource();
	WAV* GetWAV(int channel);
	void SetWAV(int channel, WAV* wav);
private:
	BmsWav wav_table[1296];
};