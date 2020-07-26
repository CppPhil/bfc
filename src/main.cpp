#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <iostream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "command_line_arguments.hpp"
#include "compile.hpp"
#include "transpile.hpp"

int main(int argc, char** argv)
{
  const char* const thisApp{argv[0]};

  for (int i{0}; i < argc; ++i) {
    if (
      (std::strcmp(argv[i], "-h") == 0)
      || (std::strcmp(argv[i], "--help") == 0)) {
      bfc::CommandLineArguments::printHelp(thisApp, std::cout);
      return EXIT_SUCCESS;
    }
  }

  const bfc::Expected<bfc::CommandLineArguments> expectedCommandLineArguments{
    bfc::CommandLineArguments::parse(argc, argv)};

  if (!expectedCommandLineArguments.has_value()) {
    fmt::print(
      stderr,
      "Failure to parse command line arguments: {}\n",
      expectedCommandLineArguments.error());
    std::cout << '\n';
    bfc::CommandLineArguments::printHelp(thisApp, std::cout);
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
    fmt::print(stderr, "Couldn't transpile: {}\n", result.error());
    return EXIT_FAILURE;
  }

  ifs.close();
  ofs.close();

  if (commandLineArguments.shouldJustTranspile()) { return EXIT_SUCCESS; }

  if (!bfc::compile(commandLineArguments.compiler(), outputFilePath)) {
    fmt::print(stderr, "Compilation failed!\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
