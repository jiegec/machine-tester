#!/bin/bash
set -e
ROOT=$PWD/../../
source /opt/spack/share/spack/setup-env.sh
spack load openmpi@3.1.6 ~cuda

set -x
make -C $ROOT clean all