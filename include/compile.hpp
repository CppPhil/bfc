#ifndef INCG_BFC_COMPILE_HPP
#define INCG_BFC_COMPILE_HPP
#include <pl/string_view.hpp>

namespace bfc {
[[nodiscard]] bool compile(pl::string_view compiler, pl::string_view cFile);
} // namespace bfc
#endif // INCG_BFC_COMPILE_HPP
