#include "SDL/SDL_timer.h"

class Timer {
private:
	Uint32 mStartTick;
	Uint32 mPausedTick;
	bool mPaused;
	bool mStarted;
public:
	Timer();
	bool IsPaused();
	bool IsStarted();
	Uint32 GetTick();
	void Start();
	void Pause();
	void UnPause();
	void Stop();
};