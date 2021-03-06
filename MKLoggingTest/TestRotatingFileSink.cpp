// what we're testing:
#include "LogRotatingFileSink.h"

// what we need for for testing
#include <gtest/gtest.h>

using namespace testing;

// make (protected) implementation methods public, so they can be tested
class TestableLogRotatingFileSink : public CLogRotatingFileSink
{
public:
    TestableLogRotatingFileSink(const std::wstring& sDirectory)
        : CLogRotatingFileSink(sDirectory)
    {
    }
    std::filesystem::path GetNextFileName() const
    {
        return CLogRotatingFileSink::GetNextFileName();
    }
    std::wstring GenerateFileName(time_t now = time(nullptr)) const
    {
        return CLogRotatingFileSink::GenerateFileName(now);
    }
};

TEST(RotatingLogFile, GenerateFileName)
{
    // we don't really care about the exact format of the generated file name, as long as it's different

    TestableLogRotatingFileSink sink(L"");

    time_t now;
    time(&now);

    EXPECT_NE(sink.GenerateFileName(now), sink.GenerateFileName(now + 1));
    // also test that the file names are generated in an alphabetically sortable way, i.e. the filename generated at t=T should be lower in rank than for t=(T+1)
    EXPECT_LT(sink.GenerateFileName(now), sink.GenerateFileName(now + 1));
    EXPECT_EQ(sink.GenerateFileName(now), sink.GenerateFileName(now));
}
