#include "Clock.h"

namespace StoneEngine::Core
{
	Clock::Clock() noexcept
	{
		mStartTime = clock::now();
	}


	Clock::Clock(const Clock& clock) noexcept
	{
		mStartTime = clock.mStartTime;
	}

	Clock::Clock(Clock&& clock) noexcept
	{
		mStartTime = clock.mStartTime;
	}

	Clock& Clock::operator=(const Clock& clock) noexcept
	{
		mStartTime = clock.mStartTime;
		return *this;
	}

	Clock& Clock::operator=(Clock&& clock) noexcept
	{
		mStartTime = clock.mStartTime;
		return *this;
	}

	U64 Clock::Reset() noexcept
	{
		mStartTime = clock::now();
		return mStartTime.time_since_epoch().count();
	}

	U64 Clock::GetElapsedTimeInMicroSeconds() const noexcept
	{
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(end - mStartTime).count();
	}

	U64 Clock::GetElapsedTimeInMilliSeconds() const noexcept
	{
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(end - mStartTime).count();
	}

	U64 Clock::GetElapsedTimeInSeconds() const noexcept
	{
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::seconds>(end - mStartTime).count();
	}

}
