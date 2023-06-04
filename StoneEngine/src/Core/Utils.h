#pragma once

#include "Core/Logger.h"
#include <fstream>
#include <format>

namespace StoneEngine::Core
{
#define DISALLOW_COPY_AND_MOVE(classname) \
    classname(const classname&) = delete; \
    classname(classname&&) = delete; \
    classname& operator=(const classname&) = delete; \
    classname& operator=(classname&&) = delete;

#define DISALLOW_COPY(className) \
    className(const className&) = delete; \
    className& operator=(const className&) = delete;

    inline void FatalAssert(bool expression, const std::string& errorMessage)
    {
        if (!expression)
        {
            Core::LogError(errorMessage);
            exit(1);
        }
    }

    static std::vector<char> ReadFileAsBinary(const std::string& filePath)
    {
        std::ifstream file( filePath, std::ios::ate | std::ios::binary );

        FatalAssert(file.is_open(), std::format("ReadFileAsBinary -> Error reading file with path: {}", filePath));
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
}
