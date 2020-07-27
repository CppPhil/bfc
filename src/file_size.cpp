#include <cstring>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <pl/os.hpp>

#if PL_OS == PL_OS_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif PL_OS == PL_OS_WINDOWS
#include <Windows.h>
#else
#error "Unsupported operating system!"
#endif

#include "file_size.hpp"

namespace bfc {
Expected<std::uint64_t> fileSize(pl::string_view filePath)
{
#if PL_OS == PL_OS_LINUX
  struct stat buf;
  std::memset(&buf, 0, sizeof(buf));
  const int i{stat(filePath.c_str(), &buf)};

  if (i == -1) {
    return BFC_UNEXPECTED(
      Error::Filesystem, fmt::format("stat failed for file \"{}\"", filePath));
  }

  return static_cast<std::uint64_t>(buf.st_size);
#elif PL_OS == PL_OS_WINDOWS
  const HANDLE hFile{CreateFileA(
    /* lpFileName */ filePath.c_str(),
    /* dwDesiredAccess */ GENERIC_READ,
    /* dwShareMode */ 0,
    /* lpSecurityAttributes */ nullptr,
    /* dwCreationDisposition */ OPEN_EXISTING,
    /* dwFlagsAndAttributes */ FILE_ATTRIBUTE_NORMAL,
    /* hTemplateFile */ nullptr)};

  if (hFile == INVALID_HANDLE_VALUE) {
    return BFC_UNEXPECTED(
      Error::Filesystem,
      fmt::format("CreateFileA failed for file \"{}\"", filePath));
  }

  LARGE_INTEGER largeIntegerFileSize;
  std::memset(&largeIntegerFileSize, 0, sizeof(largeIntegerFileSize));
  const BOOL b{GetFileSizeEx(hFile, &largeIntegerFileSize)};

  if (b == 0) {
    CloseHandle(hFile);
    return BFC_UNEXPECTED(
      Error::Filesystem,
      fmt::format("GetFileSizeEx failed for file \"{}\"", filePath));
  }

  const BOOL errorCode{CloseHandle(hFile)};

  if (errorCode == 0) {
    return BFC_UNEXPECTED(
      Error::Filesystem,
      fmt::format("CloseHandle failed for file \"{}\"", filePath));
  }

  return static_cast<std::uint64_t>(largeIntegerFileSize.QuadPart);
#endif
}
} // namespace bfc
