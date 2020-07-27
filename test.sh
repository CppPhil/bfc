#!/bin/bash

catch_errors() {
  printf "\ntest.sh failed!\n" >&2
  exit 1
}

trap catch_errors ERR;

# Directory containing this bash script
readonly DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

readonly PREV_DIR=$(pwd)

cd "$DIR"

if [ ! -d build ]; then
  printf "build directory does not exist, exiting!\n" >&2
  exit 1
fi 

cd build

ctest . --verbose

cd ../external/rdebath_brainfuck/
rm -rf testing
git checkout testing

cd "$PREV_DIR"
exit 0

