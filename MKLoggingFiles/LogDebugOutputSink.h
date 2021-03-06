#pragma once

#include "ILogSink.h"

// Use OutputDebugString for output during debugging
class CLogDebugOutputSink : public ILogSink
{
public: // ILogSink
    void OutputString(const std::string& text) noexcept override;
};
