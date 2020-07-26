#include <ostream>
#include <utility>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <pl/string_view.hpp>

#include "error.hpp"

namespace bfc {
namespace {
inline pl::string_view kind_to_string(Error::Kind kind)
{
  switch (kind) {
#define BFC_ERROR_KIND_X(kind) \
  case Error::kind: return #kind;
    BFC_ERROR_KIND
#undef BFC_ERROR_KIND_X
  }

  // TODO: UNREACHABLE
}
} // namespace

std::ostream& operator<<(std::ostream& os, const Error& error)
{
  return os << fmt::format(
           R"(Error{{"kind": "{}", "file": "{}", "function": "{}", "line"    : {}, "message": "{}"}})",
           kind_to_string(error.kind()),
           error.file(),
           error.function(),
           error.line(),
           error.message());
}

Error::Error(
  Kind        kind,
  std::string file,
  std::string function,
  std::size_t line,
  std::string message)
  : m_kind{kind}
  , m_file{std::move(file)}
  , m_function{std::move(function)}
  , m_line{line}
  , m_message{std::move(message)}
{
}

Error::Kind Error::kind() const noexcept { return m_kind; }

const std::string& Error::file() const noexcept { return m_file; }

const std::string& Error::function() const noexcept { return m_function; }

std::size_t Error::line() const noexcept { return m_line; }

const std::string& Error::message() const noexcept { return m_message; }
} // namespace bfc
