#ifndef INCG_BFC_COMMAND_LINE_ARGUMENTS_HPP
#define INCG_BFC_COMMAND_LINE_ARGUMENTS_HPP
#include <iosfwd>
#include <string>

#include "error.hpp"

namespace bfc {
class CommandLineArguments {
public:
  static Expected<CommandLineArguments> parse(int argc, char** argv);

  static void printHelp(const char* thisApp, std::ostream& os);

  [[nodiscard]] const std::string& inputFilePath() const noexcept;

  [[nodiscard]] const std::string& compiler() const noexcept;

  [[nodiscard]] bool shouldJustTranspile() const noexcept;

private:
  CommandLineArguments(
    std::string&& inputFilePath,
    std::string&& compiler,
    bool          shouldJustTranspile) noexcept;

  std::string m_inputFilePath;
  std::string m_compiler;
  bool        m_shouldJustTranspile;
};
} // namespace bfc
#endif // INCG_BFC_COMMAND_LINE_ARGUMENTS_HPP
