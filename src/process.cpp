#include <stdio.h>

#include <cassert>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <pl/os.hpp>

#include "process.hpp"

namespace bfc {
namespace {
[[nodiscard]] FILE* processOpen(const char* command, const char* mode)
{
#if PL_OS == PL_OS_LINUX
  return popen(command, mode);
#elif PL_OS == PL_OS_WINDOWS
  return _popen(command, mode);
#else
#error "Unsupported operating system!"
#endif
}

[[nodiscard]] bool processClose(FILE* stream)
{
#if PL_OS == PL_OS_LINUX
  return pclose(stream) != -1;
#elif PL_OS == PL_OS_WINDOWS
  return _pclose(stream) != -1;
#else
#error "Unsupported operating system!"
#endif
}
} // namespace

Expected<Process> Process::create(pl::string_view command, pl::string_view mode)
{
  FILE* stream{processOpen(command.c_str(), mode.c_str())};

  if (stream == nullptr) {
    return BFC_UNEXPECTED(
      Error::FailureToStartProcess,
      fmt::format(
        "popen failed for command \"{}\" and mode \"{}\".", command, mode));
  }

  return Process{stream};
}

Process::Process(this_type&& other) noexcept : m_file{other.m_file}
{
  other.m_file = nullptr;
}

Process& Process::operator=(this_type&& other) noexcept
{
  auto tmp     = other.m_file;
  other.m_file = m_file;
  m_file       = tmp;
  return *this;
}

Process::~Process()
{
  if (m_file == nullptr) { return; }

  if (!processClose(m_file)) {
    fmt::print(stderr, "~Process(): Couldn't close m_file!\n");
    assert(false && "Failure to close process stream!");
  }
}

std::FILE* Process::file() noexcept { return m_file; }

const std::FILE* Process::file() const noexcept
{
  return const_cast<this_type*>(this)->file();
}

Process::Process(std::FILE* file) noexcept : m_file{file} {}
} // namespace bfc
