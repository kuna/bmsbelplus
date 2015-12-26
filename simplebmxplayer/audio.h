#pragma once

#include "common.h"
#include "SDL/SDL_mixer.h"

class Audio {
private:
	Mix_Chunk *sdlaudio;
	int channel;
public:
	Audio(std::wstring& filepath, int channel=-1);
	~Audio();
	bool IsLoaded();
	void Play();
	void Stop();
};
