#!/usr/bin/env bash

cd "$(dirname "${BASH_SOURCE[0]}")/build"
make && ./test/testall
cd -