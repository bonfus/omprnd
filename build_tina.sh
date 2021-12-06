#!/bin/bash
VER=4.24

# https://stackoverflow.com/a/9107028
SCRIPT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/"

wget https://github.com/rabauke/trng4/archive/refs/tags/v$VER.tar.gz
tar xzf v4.24.tar.gz
cd trng4-$VER
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$SCRIPT_PATH/tina  CXXFLAGS='-fPIC' ..
make && make install
