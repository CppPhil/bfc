#include <istream>
#include <ostream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "transpile.hpp"

namespace bfc {
namespace {
bool writePrologue(std::ostream& out)
{
  constexpr char code[] = R"(#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 51200

static char buffer[BUFFER_SIZE];
static char* ptr = &buffer[0] + (BUFFER_SIZE / 2);

static void right()
{
  if (ptr == (&buffer[0] + BUFFER_SIZE - 1)) {
    fprintf(stderr, "Buffer overrun on right operation!\n");
    exit(1);
  }

  ++ptr;
}

static void left()
{
  if (ptr == &buffer[0]) {
    fprintf(stderr, "Buffer overrun on left operation!\n");
    exit(2);
  }

  --ptr;
}

int main(void)
{)";

  return static_cast<bool>(out << code);
}

bool writeEpilogue(std::ostream& out)
{
  return static_cast<bool>(out << "}\n");
}
} // namespace

Expected<void> transpile(std::istream& in, std::ostream& out)
{
  if (!in) {
    return BFC_UNEXPECTED(
      Error::Stream, "Input stream was not in a good state.");
  }

  if (!out) {
    return BFC_UNEXPECTED(
      Error::Stream, "Output stream was not in a good state.");
  }

  if (!writePrologue(out)) {
    return BFC_UNEXPECTED(
      Error::Stream, "Couldn't write prologue to output stream");
  }

  std::istream::char_type ch;

  while (in.get(ch)) {
    switch (ch) {
    case '>': fmt::print(out, "  right();\n"); break;
    case '<': fmt::print(out, "  left();\n"); break;
    case '+': fmt::print(out, "  ++*ptr;\n"); break;
    case '-': fmt::print(out, "  --*ptr;\n"); break;
    case '[': fmt::print(out, "  while (*ptr) {\n"); break;
    case ']': fmt::print(out, "  }\n"); break;
    case '.': fmt::print(out, "  putchar(*ptr);\n"); break;
    case ',': fmt::print(out, "  *ptr = getchar();\n"); break;
    default: (void)0; break;
    }
  }

  if (!writeEpilogue(out)) {
    return BFC_UNEXPECTED(
      Error::Stream, "Couldn't write epilogue to output stream.");
  }

  return {};
}
} // namespace bfc
