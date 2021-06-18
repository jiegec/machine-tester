#!/bin/bash
set -e
ROOT=$PWD/../../
source /opt/spack/share/spack/setup-env.sh
spack load mvapich2@2.3.4

set -x
make -C $ROOT clean all