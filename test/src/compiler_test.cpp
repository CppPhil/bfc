#include <stdio.h>

#include <cassert>
#include <cstdio>

#include <fstream>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "gtest/gtest.h"

#include <pl/algo/ranged_algorithms.hpp>
#include <pl/os.hpp>

#if PL_OS == PL_OS_LINUX
#include <sys/wait.h>
#endif

#include "directory_listing.hpp"
#include "file_size.hpp"
#include "process.hpp"

namespace {
template<typename T>
std::string to_str(const T& t)
{
  std::ostringstream oss{};
  oss.imbue(std::locale::classic());
  oss << t;
  return oss.str();
}

constexpr char dir[] = "external/rdebath_brainfuck/testing";

bfc::DirectoryListing createDirectoryListing()
{
  return bfc::DirectoryListing{dir};
}

std::string readFile(pl::string_view filePath)
{
  using namespace std::string_literals;
  std::string path{dir + "/"s + filePath.to_string()};

#if PL_OS == PL_OS_WINDOWS
  pl::algo::replace(path, '/', '\\');
#endif

  const auto fileSizeExpected{bfc::fileSize(path)};
  if (!fileSizeExpected.has_value()) {
    throw std::runtime_error{to_str(fileSizeExpected.error())};
  }
  const auto  fileSize{fileSizeExpected.value()};
  std::string buffer{};
  buffer.resize(fileSize);
  std::FILE* p{std::fopen(path.c_str(), "rb")};
  assert(p != nullptr && "couldn't open file.");
  [[maybe_unused]] const auto r = std::fread(&buffer[0], 1, fileSize, p);
  assert(r == fileSize && "error");
  std::fclose(p);
  return buffer;
}
} // namespace

TEST(compiler, shouldWork)
{
  using namespace std::string_literals;
  std::setbuf(stdout, nullptr);
  const bfc::DirectoryListing directoryListing{createDirectoryListing()};

  for (const std::string& entry : directoryListing) {
    const bfc::Expected<std::uint64_t> expectedFileSize{
      bfc::fileSize(dir + "/"s + entry)};
    ASSERT_TRUE(expectedFileSize.has_value());

    if (
      expectedFileSize.value() > 35840 || entry == "Bench.b"
      || entry == "Factor.b") {
      continue;
    }

    const pl::string_view     currentEntry{entry};
    constexpr pl::string_view brainfuckFileextension{".b"};

    if (currentEntry.ends_with(brainfuckFileextension)) {
      const std::string baseName{
        entry.substr(0, entry.size() - brainfuckFileextension.size())};
      const std::string outFile{baseName + ".out"};
      const std::string inFile{baseName + ".in"};
#if PL_OS == PL_OS_LINUX
      const std::string exeName{entry + ".c.out"};
#elif PL_OS == PL_OS_WINDOWS
      const std::string exeName{entry + ".exe"};
#endif

      if (directoryListing.contains(outFile)) {
#if PL_OS == PL_OS_LINUX
        const int r{std::system(
          fmt::format("./build/bfc {}/{}", dir, currentEntry).c_str())};

        ASSERT_EQ(0, WEXITSTATUS(r));
#elif PL_OS == PL_OS_WINDOWS
#if defined(RELEASE_MODE)
        const std::string str{"Release"};
#elif defined(DEBUG_MODE)
        const std::string str{"Debug"};
#endif
        const int         r{std::system(
          fmt::format("build\\{}\\bfc.exe {}\\{}", str, dir, currentEntry)
            .c_str())};
        ASSERT_EQ(0, r);
#endif

        const std::string expectedOutput{readFile(outFile)};
        std::string       actualBuffer{};
        actualBuffer.resize(expectedOutput.size());

        printf("Currently processing: \"%s\"\n", exeName.c_str());

        if (directoryListing.contains(inFile)) {
          const std::string input{readFile(inFile)};
          std::string       outputFilePath{dir + "/"s + "cur_stdout.txt"};

#if PL_OS == PL_OS_WINDOWS
          pl::algo::replace(outputFilePath, '/', '\\');
#endif

          {
#if PL_OS == PL_OS_LINUX
            bfc::Expected<bfc::Process> expectedProcess{bfc::Process::create(
              fmt::format("./{}/{} > {}", dir, exeName, outputFilePath), "w")};
#elif PL_OS == PL_OS_WINDOWS
            bfc::Expected<bfc::Process> expectedProcess{bfc::Process::create(
              fmt::format("./{} > {}", exeName, outputFilePath), "w")};
#endif
            ASSERT_TRUE(expectedProcess.has_value());
            bfc::Process& process{expectedProcess.value()};

            const std::size_t resultVal{
              std::fwrite(input.data(), 1, input.size(), process.file())};
            ASSERT_EQ(input.size(), resultVal);
          }
          actualBuffer = readFile("cur_stdout.txt");
          EXPECT_EQ(expectedOutput.size(), actualBuffer.size());
          ASSERT_EQ(expectedOutput, actualBuffer);
        }
        else {
#if PL_OS == PL_OS_LINUX
          bfc::Expected<bfc::Process> expectedProcess{
            bfc::Process::create(fmt::format("./{}/{}", dir, exeName), "r")};
#elif PL_OS == PL_OS_WINDOWS
          bfc::Expected<bfc::Process> expectedProcess{
            bfc::Process::create(fmt::format("./{}", exeName), "r")};
#endif
          ASSERT_TRUE(expectedProcess.has_value());
          bfc::Process&     process{expectedProcess.value()};
          const std::size_t res{std::fread(
            actualBuffer.data(), 1, actualBuffer.size(), process.file())};
          EXPECT_EQ(expectedOutput.size(), res);
          ASSERT_EQ(expectedOutput, actualBuffer);
        }
      }
    }
  }
}
