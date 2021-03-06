cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(bfc_project VERSION 1.0.0)

enable_testing()

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    string(APPEND CMAKE_CXX_FLAGS "/W4")
  endif()

  string(
    APPEND
    CMAKE_CXX_FLAGS
    " /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS /D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING /DNOMINMAX /DUNICODE"
  )

  string(APPEND CMAKE_CXX_FLAGS_DEBUG " /Od /DDEBUG_MODE")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE " /O2 /DNDEBUG /DRELEASE_MODE")
else()
  string(
    APPEND
    CMAKE_CXX_FLAGS
    " -Werror -Wall -Wcast-align -Wcast-qual -Wmissing-field-initializers -Wmissing-include-dirs -Wpointer-arith -Wstrict-aliasing=2 -Wunreachable-code"
  )

  string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g -O0 -DDEBUG_MODE")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE
         " -g -O3 -funroll-loops -DNDEBUG -DRELEASE_MODE")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_subdirectory(external/philslib)
add_subdirectory(external/fmtlib)
add_subdirectory(external/googletest)

# STATIC LIBRARY
set(LIB_NAME bfc_lib)

set(LIB_HEADERS
    include/error.hpp
    include/unreachable.hpp
    include/command_line_arguments.hpp
    include/transpile.hpp
    include/compile.hpp
    include/directory_listing.hpp
    include/process.hpp
    include/file_size.hpp)

set(LIB_SOURCES
    src/error.cpp
    src/command_line_arguments.cpp
    src/transpile.cpp
    src/compile.cpp
    src/directory_listing.cpp
    src/process.cpp
    src/file_size.cpp)

add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})

target_link_libraries(${LIB_NAME} PUBLIC Threads::Threads CppPhil::philslib
                                         fmt::fmt)

if(WIN32)
  target_link_libraries(${LIB_NAME} PUBLIC Kernel32.lib)
endif()

target_include_directories(
  ${LIB_NAME} PUBLIC ${bfc_project_SOURCE_DIR}/include
                     ${bfc_project_SOURCE_DIR}/external/expected/include)

# APPLICATION
set(APP_NAME bfc)
add_executable(${APP_NAME} src/main.cpp)
target_link_libraries(${APP_NAME} PRIVATE ${LIB_NAME})

add_subdirectory(test)
