#include <stdio.h>

#include <cassert>
#include <cstdio>

#include <fstream>
#include <iterator>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "gtest/gtest.h"

#include <Poco/File.h>

#include <pl/os.hpp>

#if PL_OS == PL_OS_LINUX
#include <sys/wait.h>
#endif

#include "directory_listing.hpp"
#include "process.hpp"

namespace {
constexpr char dir[] = "external/rdebath_brainfuck/testing";

bfc::DirectoryListing createDirectoryListing()
{
  return bfc::DirectoryListing{dir};
}

std::string readFile(pl::string_view filePath)
{
  using namespace std::string_literals;
  std::ifstream ifs{dir + "/"s + filePath.to_string()};
  assert(static_cast<bool>(ifs) && "Failed to open file!");
  std::string buffer(
    std::istream_iterator<char>{ifs}, std::istream_iterator<char>{});
  return buffer;
}
} // namespace

TEST(compiler, shouldWork)
{
  using namespace std::string_literals;
  std::setbuf(stdout, nullptr);
  const bfc::DirectoryListing directoryListing{createDirectoryListing()};

  for (const std::string& entry : directoryListing) {
    if (Poco::File(dir + "/"s + entry).getSize() > 51200) { continue; }

    const pl::string_view     currentEntry{entry};
    constexpr pl::string_view brainfuckFileextension{".b"};

    if (currentEntry.ends_with(brainfuckFileextension)) {
      const std::string baseName{
        entry.substr(0, entry.size() - brainfuckFileextension.size())};
      const std::string outFile{baseName + ".out"};
      const std::string inFile{baseName + ".in"};
      const std::string exeName{entry + ".c.out"};

      if (directoryListing.contains(outFile)) {
        fmt::print("Starting compilation of \"{}\".\n", currentEntry);

        // TODO: This'll be different on Windows
        const int r{std::system(
          fmt::format("./build/bfc {}/{}", dir, currentEntry).c_str())};

        ASSERT_EQ(0, WEXITSTATUS(r));

        fmt::print("Successfully compiled \"{}\".\n", currentEntry);

        const std::string expectedOutput{readFile(outFile)};
        std::string       actualBuffer{};
        actualBuffer.resize(expectedOutput.size());

        if (directoryListing.contains(inFile)) {
          const std::string input{readFile(inFile)};
        }
        else {
          bfc::Expected<bfc::Process> expectedProcess{
            bfc::Process::create(fmt::format("./{}/{}", dir, exeName), "r")};
          ASSERT_TRUE(expectedProcess.has_value());
          bfc::Process&     process{expectedProcess.value()};
          const std::size_t res{std::fread(
            actualBuffer.data(), 1, actualBuffer.size(), process.file())};
          EXPECT_EQ(expectedOutput.size(), res);
          EXPECT_EQ(expectedOutput, actualBuffer);
          fmt::print("Output for \"{}\" was correct.\n", currentEntry);
        }
      }
    }
  }
}
