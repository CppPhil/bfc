#ifndef INCG_BFC_DIRECTORY_LISTING_HPP
#define INCG_BFC_DIRECTORY_LISTING_HPP
#include <iosfwd>
#include <string>
#include <vector>

#include <pl/string_view.hpp>

namespace bfc {
class DirectoryListing {
public:
  using value_type             = std::string;
  using container_type         = std::vector<value_type>;
  using const_iterator         = container_type::const_iterator;
  using iterator               = container_type::iterator;
  using const_reverse_iterator = container_type::const_reverse_iterator;
  using reverse_iterator       = container_type::reverse_iterator;

  friend std::ostream& operator<<(
    std::ostream&           os,
    const DirectoryListing& directoryListing);

  explicit DirectoryListing(const std::string& path);

  [[nodiscard]] bool contains(pl::string_view path) const noexcept;

  iterator begin() noexcept;

  const_iterator begin() const noexcept;

  const_iterator cbegin() const noexcept;

  iterator end() noexcept;

  const_iterator end() const noexcept;

  const_iterator cend() const noexcept;

  reverse_iterator rbegin() noexcept;

  const_reverse_iterator rbegin() const noexcept;

  const_reverse_iterator crbegin() const noexcept;

  reverse_iterator rend() noexcept;

  const_reverse_iterator rend() const noexcept;

  const_reverse_iterator crend() const noexcept;

private:
  std::vector<std::string> m_paths;
};
} // namespace bfc
#endif // INCG_BFC_DIRECTORY_LISTING_HPP
