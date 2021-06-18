#!/bin/bash
set -e
ROOT=$PWD/../../

set -x
make -C $ROOT clean all
