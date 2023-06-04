#pragma once

#include "../pch.h"
#define SPDLOG_ACTIVE_LEVEL 0
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"

namespace StoneEngine::Core
{
	enum LogLevel : int
	{
		Debug = 0, // All debug logs are displayed
		Info = 1, // Only LogInfo and above are displayed
		Performance = 2 // Strip away all but errors and critical logs
	};

	static inline void SetLogLevel(LogLevel level)
	{
		switch (level)
		{
			case Debug:
				spdlog::set_level(spdlog::level::debug);
				break;
			case Info:
				spdlog::set_level(spdlog::level::info);
				break;
			case Performance:
				spdlog::set_level(spdlog::level::warn);
				break;
		}

	}

	template<typename... Args>
	static inline void LogInfo(const fmt::format_string<Args...>& fmt, Args&&... args)
	{
		spdlog::info(fmt, std::forward<Args>(args)...);
	};

	template<typename... Args>
	static inline void LogDebug(const fmt::format_string<Args...>& fmt, Args&&... args)
	{
		spdlog::debug(fmt, std::forward<Args>(args)...);
	};

	template<typename... Args>
	static inline void LogWarn(const fmt::format_string<Args...>& fmt, Args&&... args)
	{
		spdlog::warn(fmt, std::forward<Args>(args)...);
	};

	template<typename... Args>
	static inline void LogError(const fmt::format_string<Args...>& fmt, Args&&... args)
	{
		spdlog::error(fmt, std::forward<Args>(args)...);
	};

	static inline void LogError(const std::string_view& error)
	{
		LogError("{}", error);
	}

	static inline void LogDebug(const std::string_view& error)
	{
		LogDebug("{}", error);
	}

	static inline void LogWarn(const std::string_view& error)
	{
		LogWarn("{}", error);
	}

	static inline void LogInfo(const std::string_view& error)
	{
		LogInfo("{}", error);
	}
}