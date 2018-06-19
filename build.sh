#!/usr/bin/env bash

# Build.
cd "$(dirname "${BASH_SOURCE[0]}")/build"
cmake ..
make
cd -

# Run.
"$(dirname "${BASH_SOURCE[0]}")/build/src/mmgc"