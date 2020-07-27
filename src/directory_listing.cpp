#include <cassert>
#include <cstdio>

#include <ostream>
#include <type_traits>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <pl/algo/ranged_algorithms.hpp>
#include <pl/os.hpp>

#if PL_OS == PL_OS_LINUX
#include <dirent.h>
#include <sys/types.h>
#elif PL_OS == PL_OS_WINDOWS
#include <Windows.h>
#include <tchar.h>
#else
#error "Unsupported operating system!"
#endif

#include "directory_listing.hpp"

namespace bfc {
namespace {
[[nodiscard]] bool listFiles(
  pl::string_view           directory,
  std::vector<std::string>& out)
{
#if PL_OS == PL_OS_LINUX
  auto dir{opendir(directory.c_str())};

  if (dir == nullptr) { return false; }

  for (struct dirent* entry{nullptr}; (entry = readdir(dir)) != nullptr;) {
    out.push_back(entry->d_name);
  }

  const int r{closedir(dir)};

  if (r == EBADF) { return false; }

  return true;
#elif PL_OS == PL_OS_WINDOWS
  WIN32_FIND_DATA ffd;
  std::string     buf{directory.to_string()};
  buf += "\\*";
  HANDLE hFind{INVALID_HANDLE_VALUE};

  if constexpr (std::is_same_v<TCHAR, char>) {
    hFind = FindFirstFile(buf.c_str(), &ffd);
  }
  else if constexpr (std::is_same_v<TCHAR, wchar_t>) {
    const int    requiredBufferSize{MultiByteToWideChar(
      /* CodePage */ CP_UTF8,
      /* dwFlags */ 0,
      /* lpMultiByteStr */ buf.c_str(),
      /* cbMultiByte */ -1,
      /* lpWideCharStr */ nullptr,
      /* cchWideChar */ 0)};
    std::wstring buffer(
      static_cast<std::wstring::size_type>(requiredBufferSize), L' ');

    const int i{MultiByteToWideChar(
      /* CodePage */ CP_UTF8,
      /* dwFlags */ 0,
      /* lpMultiByteStr */ buf.c_str(),
      /* cbMultiByte */ -1,
      /* lpWideCharStr */ buffer.data(),
      /* cchWideChar */ buffer.size())};

    if (i == 0) { return false; }

    hFind = FindFirstFile(buffer.c_str(), &ffd);
  }

  if (hFind == INVALID_HANDLE_VALUE) { return false; }

  do {
    if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      if constexpr (std::is_same_v<TCHAR, char>) {
        out.push_back(ffd.cFileName);
      }
      else if constexpr (std::is_same_v<TCHAR, wchar_t>) {
        const int bytesNeeded{WideCharToMultiByte(
          /* CodePage */ CP_UTF8,
          /* dwFlags */ 0,
          /* lpWideCharStr */ ffd.cFileName,
          /* cchWideChar */ -1,
          /* lpMultiByteStr */ nullptr,
          /* cbMultiByte */ 0,
          /* lpDefaultChar */ nullptr,
          /* lpUsedDefaultChar */ nullptr)};

        std::string string(bytesNeeded, ' ');

        const int i{WideCharToMultiByte(
          /* CodePage */ CP_UTF8,
          /* dwFlags */ 0,
          /* lpWideCharStr */ ffd.cFileName,
          /* cchWideChar */ -1,
          /* lpMultiByteStr */ string.data(),
          /* cbMultiByte */ string.size(),
          /* lpDefaultChar */ nullptr,
          /* lpUsedDefaultChar */ nullptr)};

        if (i == 0) { return false; }

        out.push_back(std::move(string));
      }
    }
  } while (FindNextFile(hFind, &ffd) != 0);

  DWORD dwError{GetLastError()};

  if (dwError != ERROR_NO_MORE_FILES) {
    FindClose(hFind);
    return false;
  }

  const BOOL errC{FindClose(hFind)};

  if (!errC) { return false; }

  return dwError == 0UL;
#endif
}
} // namespace

std::ostream& operator<<(
  std::ostream&           os,
  const DirectoryListing& directoryListing)
{
  fmt::print(
    os,
    "DirectoryListing\n[\n  {}\n]",
    fmt::join(directoryListing.m_paths, ",\n  "));
  return os;
}

DirectoryListing::DirectoryListing(const std::string& path) : m_paths{}
{
  const bool result{listFiles(path, m_paths)};

  if (!result) {
    std::fprintf(
      stderr,
      "listFiles failed in %s with path being \"%s\"!\n",
      __func__,
      path.c_str());
    assert(false && "listFiles failed!");
    throw std::runtime_error{"listFiles failed!"};
  }

  pl::algo::sort(m_paths);
}

bool DirectoryListing::contains(pl::string_view path) const noexcept
{
  return pl::algo::any_of(m_paths, [path](const std::string& fileInDirectory) {
    return fileInDirectory == path;
  });
}

DirectoryListing::iterator DirectoryListing::begin() noexcept
{
  return m_paths.begin();
}

DirectoryListing::const_iterator DirectoryListing::begin() const noexcept
{
  return m_paths.begin();
}

DirectoryListing::const_iterator DirectoryListing::cbegin() const noexcept
{
  return m_paths.cbegin();
}

DirectoryListing::iterator DirectoryListing::end() noexcept
{
  return m_paths.end();
}

DirectoryListing::const_iterator DirectoryListing::end() const noexcept
{
  return m_paths.end();
}

DirectoryListing::const_iterator DirectoryListing::cend() const noexcept
{
  return m_paths.cend();
}

DirectoryListing::reverse_iterator DirectoryListing::rbegin() noexcept
{
  return m_paths.rbegin();
}

DirectoryListing::const_reverse_iterator DirectoryListing::rbegin()
  const noexcept
{
  return m_paths.rbegin();
}

DirectoryListing::const_reverse_iterator DirectoryListing::crbegin()
  const noexcept
{
  return m_paths.crbegin();
}

DirectoryListing::reverse_iterator DirectoryListing::rend() noexcept
{
  return m_paths.rend();
}

DirectoryListing::const_reverse_iterator DirectoryListing::rend() const noexcept
{
  return m_paths.rend();
}

DirectoryListing::const_reverse_iterator DirectoryListing::crend()
  const noexcept
{
  return m_paths.crend();
}
} // namespace bfc
