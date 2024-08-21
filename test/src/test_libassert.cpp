#include <gtest/gtest.h>
#include <libassert/assert.hpp>

#ifdef _WIN32
__declspec(dllimport) void __stdcall OutputDebugStringA(
    const char *lpOutputString);
#endif

TEST(test_libassert, simple) {
  int a = 42;
  ASSERT(a == 42);
  ASSERT(a = 21);
  ASSERT(a == 21);
}
