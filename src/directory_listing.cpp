#include <ostream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <Poco/File.h>

#include <pl/algo/ranged_algorithms.hpp>

#include "directory_listing.hpp"

namespace bfc {
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
  Poco::File{path}.list(m_paths);
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
