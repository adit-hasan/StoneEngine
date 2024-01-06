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

	double Clock::Reset() noexcept
	{
		mStartTime = clock::now();
		return mStartTime.time_since_epoch().count();
	}

	double Clock::GetElapsedTimeInMicroSeconds() const noexcept
	{
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::chrono::microseconds::period>(end - mStartTime).count();
	}

	double Clock::GetElapsedTimeInMilliSeconds() const noexcept
	{
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::chrono::milliseconds::period>(end - mStartTime).count();
	}

	double Clock::GetElapsedTimeInSeconds() const noexcept
	{
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::chrono::seconds::period>(end - mStartTime).count();
	}

}
