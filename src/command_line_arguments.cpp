#include <cstring>

#include <ostream>
#include <variant>

#include <pl/numeric.hpp>
#include <pl/os.hpp>
#include <pl/overload.hpp>
#include <pl/string_view.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "command_line_arguments.hpp"

namespace bfc {
namespace {
constexpr int maximumArgumentCount{2};

struct transpile {
};

std::variant<std::string, transpile, Error> secondOption(
  int    actualArgumentCount,
  char** argv)
{
  constexpr int compilerTranspilationIndex{2};

  if (actualArgumentCount == maximumArgumentCount) {
    const char* const option{argv[compilerTranspilationIndex]};

    if (std::strcmp(option, "--transpile") == 0) { return transpile{}; }

    constexpr pl::string_view compilerOptionBegin{"--compiler="};
    pl::string_view           optionStringView{option};

    if (optionStringView.starts_with(compilerOptionBegin)) {
      optionStringView.remove_prefix(compilerOptionBegin.size());
      return optionStringView.to_string();
    }

    return BFC_UNEXPECTED(
      Error::InvalidArgument,
      fmt::format("\"{}\" could not be parsed.", option));
  }
  else {
#if PL_OS == PL_OS_LINUX
    using namespace std::string_literals;
    return "/usr/bin/cc"s;
#endif // PL_OS == PL_OS_LINUX
  }
}

struct Result {
  std::string compilerPath;
  bool        isJustTranspilation;
};

} // namespace

Expected<CommandLineArguments> CommandLineArguments::parse(
  int    argc,
  char** argv)
{
  constexpr int argumentCountOffset{1};
  const int     actualArgumentCount{argc - argumentCountOffset};
  constexpr int minimumArgumentCount{1};

  if (!pl::is_between(
        actualArgumentCount, minimumArgumentCount, maximumArgumentCount)) {
    return BFC_UNEXPECTED(
      Error::InvalidArgument,
      fmt::format(
        "{} command line arguments were given but at least {} and at most {} "
        "were expected.",
        actualArgumentCount,
        minimumArgumentCount,
        maximumArgumentCount));
  }

  constexpr int inputFileIndex{1};

  const char* const inputFilePath{argv[inputFileIndex]};

  const std::variant<std::string, transpile, Error> secondOptionResult{
    secondOption(actualArgumentCount, argv)};

  const Expected<Result> expectedResult{std::visit(
    pl::overload(
      [](const std::string& compilerPath) {
        return Expected<Result>
        {
          Result { compilerPath, false }
        }
      },
      [](transpile) {
        return Expected<Result>
        {
          Result { "", true }
        }
      },
      [](const Error& error) -> Expected<Result> { return error; }),
    secondOptionResult)};

  if (!expectedResult.has_value()) { return expectedResult.error(); }

  const Result& result{expectedResult.value()};

  return CommandLineArguments{
    std::string{inputFilePath}, result.compiler, result.isJustTranspilation};
}

void CommandLineArguments::printHelp(std::ostream& os)
{
  // TODO: HERE
}

const std::string& CommandLineArguments::inputFilePath() const noexcept
{
  return m_inputFilePath;
}

const std::string& CommandLineArguments::compiler() const noexcept
{
  return m_compiler;
}

bool CommandLineArguments::shouldJustTranspile() const noexcept
{
  return m_shouldJustTranspile;
}

CommandLineArguments::CommandLineArguments(
  std::string&& inputFilePath,
  std::string&& compiler,
  bool          shouldJustTranspile) noexcept
  : m_inputFilePath{std::move(inputFilePath)}
  , m_compiler{std::move(compiler)}
  , m_shouldJustTranspile{shouldJustTranspile}
{
}
} // namespace bfc
