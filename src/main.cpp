#include <cstdio>
#include <cstdlib>

#include <fstream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "command_line_arguments.hpp"
#include "transpile.hpp"

int main(int argc, char** argv)
{
  const bfc::Expected<bfc::CommandLineArguments> expectedCommandLineArguments{
    bfc::CommandLineArguments::parse(argc, argv)};

  if (!expectedCommandLineArguments.has_value()) {
    fmt::print(
      stderr,
      "Failure to parse command line arguments: \"{}\".\n",
      expectedCommandLineArguments.error());
    return EXIT_FAILURE;
  }

  const bfc::CommandLineArguments& commandLineArguments{
    expectedCommandLineArguments.value()};

  std::ifstream     ifs{commandLineArguments.inputFilePath()};
  const std::string outputFilePath{commandLineArguments.inputFilePath() + ".c"};
  std::ofstream     ofs{outputFilePath};

  if (!ifs) {
    fmt::print(
      stderr,
      "Couldn't open \"{}\" for reading.\n",
      commandLineArguments.inputFilePath());
    return EXIT_FAILURE;
  }

  if (!ofs) {
    fmt::print(stderr, "Couldn't open \"{}\" for writing.\n", outputFilePath);
    return EXIT_FAILURE;
  }

  const bfc::Expected<void> result{bfc::transpile(ifs, ofs)};

  if (!result.has_value()) {
    fmt::print(stderr, "Couldn't transpile: \"{}\".\n", result.error());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
