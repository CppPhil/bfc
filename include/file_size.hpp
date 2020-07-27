#ifndef INCG_BFC_FILE_SIZE_HPP
#define INCG_BFC_FILE_SIZE_HPP
#include <cstdint>

#include <pl/string_view.hpp>

#include "error.hpp"

namespace bfc {
[[nodiscard]] Expected<std::uint64_t> fileSize(pl::string_view filePath);
} // namespace bfc
#endif // INCG_BFC_FILE_SIZE_HPP
