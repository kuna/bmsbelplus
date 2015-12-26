#include "audio.h"

Audio::Audio(std::wstring& filepath, int channel) : channel(channel) {
	char path_utf8[1024];
	ENCODING::wchar_to_utf8(filepath.c_str(), path_utf8, 1024);
	sdlaudio = Mix_LoadWAV(path_utf8);
}

Audio::~Audio() {
	if (IsLoaded())
		Mix_FreeChunk(sdlaudio);
}

bool Audio::IsLoaded() {
	return sdlaudio != 0;
}
void Audio::Play() {
	Mix_PlayChannel(channel, sdlaudio, 1);
}

void Audio::Stop() {
	Mix_HaltChannel(channel);
}