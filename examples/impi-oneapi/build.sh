#!/bin/bash
set -e
ROOT=$PWD/../../
source /opt/intel/oneapi/setvars.sh

set -x
make -C $ROOT clean all