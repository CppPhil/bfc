#include <cstdlib>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <pl/os.hpp>

#if PL_OS == PL_OS_LINUX
#include <sys/wait.h>
#elif PL_OS == PL_OS_WINDOWS
#include <stdio.h>
#endif

#include "compile.hpp"

namespace bfc {
bool compile(pl::string_view compiler, pl::string_view cFile)
{
#if PL_OS == PL_OS_LINUX
  const int result{std::system(
    fmt::format("{} -std=c89 -O3 {} -o {}.out", compiler, cFile, cFile)
      .c_str())};

  return WEXITSTATUS(result) == 0;
#elif PL_OS == PL_OS_WINDOWS
  _flushall();
  const int result{
    std::system(fmt::format("{} /TC /O2 {}", compiler, cFile).c_str())};

  return result == 0;
#else
#error "Unsupported operating system!"
#endif
}
} // namespace bfc
