#pragma once
#include "pch.h"

namespace StoneEngine::Core
{
	class Clock
	{
        typedef std::chrono::high_resolution_clock clock;

        Clock() noexcept;
        Clock(const Clock& clock) noexcept;
        Clock(Clock&& clock) noexcept;

        Clock& operator=(const Clock& clock) noexcept;
        Clock& operator=(Clock&& clock) noexcept;

        ~Clock() = default;

        U64 Reset() noexcept;
        U64 GetElapsedTimeInMicroSeconds() const noexcept;
        U64 GetElapsedTimeInMilliSeconds() const noexcept;
        U64 GetElapsedTimeInSeconds() const noexcept;

    private:
        clock::time_point mStartTime;
	};
}

