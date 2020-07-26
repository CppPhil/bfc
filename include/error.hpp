#ifndef INCG_BFC_ERROR_HPP
#define INCG_BFC_ERROR_HPP
#include <cstddef>

#include <iosfwd>
#include <string>

#include <tl/expected.hpp>

#include <pl/current_function.hpp>

#define BFC_ERROR_KIND              \
  BFC_ERROR_KIND_X(InvalidArgument) \
  BFC_ERROR_KIND_X(Stream)

namespace bfc {
class [[nodiscard]] Error
{
public:
  enum Kind {
#define BFC_ERROR_KIND_X(kind) kind,
    BFC_ERROR_KIND
#undef BFC_ERROR_KIND_X
  };

  friend std::ostream& operator<<(std::ostream& os, const Error& error);

  Error(
    Kind        kind,
    std::string file,
    std::string function,
    std::size_t line,
    std::string message);

  Kind kind() const noexcept;

  const std::string& file() const noexcept;

  const std::string& function() const noexcept;

  std::size_t line() const noexcept;

  const std::string& message() const noexcept;

private:
  Kind        m_kind;
  std::string m_file;
  std::string m_function;
  std::size_t m_line;
  std::string m_message;
};

template<typename Ty>
using Expected = tl::expected<Ty, Error>;
} // namespace bfc

#define BFC_UNEXPECTED(kind, message) \
  ::tl::make_unexpected(              \
    ::bfc::Error{kind, __FILE__, PL_CURRENT_FUNCTION, __LINE__, message})
#endif // INCG_BFC_ERROR_HPP
