#pragma once
#include "pch.h"

namespace StoneEngine::Core
{
	class Clock
	{
    public:
        typedef std::chrono::high_resolution_clock clock;

        Clock() noexcept;
        Clock(const Clock& clock) noexcept;
        Clock(Clock&& clock) noexcept;

        Clock& operator=(const Clock& clock) noexcept;
        Clock& operator=(Clock&& clock) noexcept;

        ~Clock() = default;

        double Reset() noexcept;
        double GetElapsedTimeInMicroSeconds() const noexcept;
        double GetElapsedTimeInMilliSeconds() const noexcept;
        double GetElapsedTimeInSeconds() const noexcept;

    private:
        clock::time_point mStartTime;
	};
}

