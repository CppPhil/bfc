#ifndef INCG_BFC_TRANSPILE_HPP
#define INCG_BFC_TRANSPILE_HPP
#include <iosfwd>

#include "error.hpp"

namespace bfc {
Expected<void> transpile(std::istream& in, std::ostream& out);
} // namespace bfc
#endif // INCG_BFC_TRANSPILE_HPP
