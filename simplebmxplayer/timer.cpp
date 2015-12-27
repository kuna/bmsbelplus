#include "timer.h"

//
// original source from http://lazyfoo.net
//
Timer::Timer()
{
	//Initialize the variables
	mStartTick = 0;
	mPausedTick = 0;

	mPaused = false;
	mStarted = false;
}

void Timer::Start()
{
	//Start the timer
	mStarted = true;

	//Unpause the timer
	mPaused = false;

	//Get the current clock time
	mStartTick = SDL_GetTicks();
	mPausedTick = 0;
}

void Timer::Stop()
{
	//Stop the timer
	mStarted = false;

	//Unpause the timer
	mPaused = false;

	//Clear tick variables
	mStartTick = 0;
	mPausedTick = 0;
}

void Timer::Pause()
{
	//If the timer is running and isn't already paused
	if (mStarted && !mPaused)
	{
		//Pause the timer
		mPaused = true;

		//Calculate the paused ticks
		mPausedTick = SDL_GetTicks() - mStartTick;
		mStartTick = 0;
	}
}

void Timer::UnPause()
{
	//If the timer is running and paused
	if (mStarted && mPaused)
	{
		//Unpause the timer
		mPaused = false;

		//Reset the starting ticks
		mStartTick = SDL_GetTicks() - mPausedTick;

		//Reset the paused ticks
		mPausedTick = 0;
	}
}

Uint32 Timer::GetTick()
{
	//The actual timer time
	Uint32 time = 0;

	//If the timer is running
	if (mStarted)
	{
		//If the timer is paused
		if (mPaused)
		{
			//Return the number of ticks when the timer was paused
			time = mPausedTick;
		}
		else
		{
			//Return the current time minus the start time
			time = SDL_GetTicks() - mStartTick;
		}
	}

	return time;
}

bool Timer::IsStarted()
{
	//Timer is running and paused or unpaused
	return mStarted;
}

bool Timer::IsPaused()
{
	//Timer is running and paused
	return mPaused && mStarted;
}