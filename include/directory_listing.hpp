#ifndef INCG_BFC_DIRECTORY_LISTING_HPP
#define INCG_BFC_DIRECTORY_LISTING_HPP
#include <string>
#include <vector>

#include <pl/string_view.hpp>

namespace bfc {
class DirectoryListing {
public:
  explicit DirectoryListing(const std::string& path);

  [[nodiscard]] bool contains(pl::string_view path) const noexcept;

private:
  std::vector<std::string> m_paths;
};
} // namespace bfc
#endif // INCG_BFC_DIRECTORY_LISTING_HPP
