
#include <cxxtest/TestSuite.h>
#include <iostream>

class SimplerandomTestSuite : public CxxTest::TestSuite
{
public:
    void testNothing()
    {
        TS_ASSERT_EQUALS(1, 1);
    }
};
