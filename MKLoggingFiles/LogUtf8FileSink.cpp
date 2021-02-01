#include "LogUtf8FileSink.h"
#include "Logger.h"
#include <cassert>

CLogUtf8FileSink::~CLogUtf8FileSink()
{
    Close();
}

bool CLogUtf8FileSink::Create(const std::filesystem::path& directoryPath, const std::filesystem::path& fileName)
{
    // File access behavior:
    // - the first bytes in the log file are the Byte Order Mark (BOM) for UTF-8
    // - in case of fwprintf, '\n' is translated to "\r\n" - on Windows only
    // - fprintf is an atomic action in POSIX standard - no additional thread serialisation required (see https://www.gnu.org/software/libc/manual/html_node/Streams-and-Threads.html)

    // fopen supports Unicode file streams. To open a Unicode file, pass a ccs flag that specifies the desired encoding to fopen, as follows.

    //FILE *fp = fopen("newfile.txt", "rt+, ccs=encoding");
    //Allowed values of encoding are UNICODE, UTF-8, and UTF-16LE.
    //
    //When a file is opened in Unicode mode, input functions translate the data that's read from the file into UTF-16 data stored as type wchar_t. Functions that write to a file opened in Unicode mode expect buffers that contain UTF-16 data stored as type wchar_t. If the file is encoded as UTF-8, then UTF-16 data is translated into UTF-8 when it is written, and the file's UTF-8-encoded content is translated into UTF-16 when it is read. An attempt to read or write an odd number of bytes in Unicode mode causes a parameter validation error. To read or write data that's stored in your program as UTF-8, use a text or binary file mode instead of a Unicode mode. You are responsible for any required encoding translation.
    //
    //If the file already exists and is opened for reading or appending, the Byte Order Mark (BOM), if it present in the file, determines the encoding. The BOM encoding takes precedence over the encoding that is specified by the ccs flag. The ccs encoding is only used when no BOM is present or the file is a new file.

    LOGD(L"directory=" << directoryPath << L", fileName=" << fileName);

    // create the output directory if necessary
    if (!std::filesystem::exists(directoryPath))
    {
        if (!std::filesystem::create_directories(directoryPath))
        {
            LOGW(L"could not create output directory " << directoryPath);
        }
    }

    if (m_pFile != nullptr)
    {
        Close();
    }

    std::filesystem::path logFilePath = directoryPath / fileName;
    m_pFile = _wfsopen(logFilePath.c_str(), L"wt, ccs=UTF-8", _SH_DENYWR);
    return (m_pFile != nullptr);
}

void CLogUtf8FileSink::Close()
{
    if (m_pFile != nullptr)
    {
        const int iResult = fclose(m_pFile);
        m_pFile = nullptr;
        if (iResult != 0)
        {
            LOGE(L"fclose failed with error " << iResult);
        }
    }
}

long CLogUtf8FileSink::GetFileSize() const
{
    return m_iWrittenByteCount;
}

bool CLogUtf8FileSink::IsOpen() const
{
    return (m_pFile != nullptr);
}

// ILogSink
void CLogUtf8FileSink::OutputString(const std::wstring& text)
{
    assert(IsOpen());

    // Since the file is created as UTF-8, an automatic conversion from UCS2 to UTF-8 is performed.
    // The number of written bytes is roughly (in most cases exactly) equal to the number of UCS2 characters.
    const int iWrittenByteCount = std::fwprintf(m_pFile, text.c_str());
    if (iWrittenByteCount < 0)
    {
        LOGE(L"fwprintf failed");
    }
    else
    {
        m_iWrittenByteCount += iWrittenByteCount;
    }

    const int iFlushResult = std::fflush(m_pFile); // Flush, because what good is a tool for post-mortem analysis if the log contents remain in volatile memory?
    if (iFlushResult != 0)
    {
        LOGE(L"Result of flushing the file stream: " << iFlushResult);
    }
}