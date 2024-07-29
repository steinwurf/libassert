#include <gtest/gtest.h>
#include <libassert/assert.hpp>

TEST(test_libassert, simple)
{
    int a = 42;
    ASSERT(a == 42);
    ASSERT(a = 21);
    ASSERT(a == 21);
}


