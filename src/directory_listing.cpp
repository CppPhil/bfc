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
    "DirectoryListing\n[\n{}\n]",
    fmt::join(directoryListing.m_paths, ",\n  "));
  return os;
}

DirectoryListing::DirectoryListing(const std::string& path) : m_paths{}
{
  Poco::File{path}.list(m_paths);
}

bool DirectoryListing::contains(pl::string_view path) const noexcept
{
  return pl::algo::any_of(m_paths, [path](const std::string& fileInDirectory) {
    return fileInDirectory == path;
  });
}
} // namespace bfc
