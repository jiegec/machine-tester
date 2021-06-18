#!/bin/bash
set -e
ROOT=$PWD/../../
source /opt/spack/share/spack/setup-env.sh
spack load openmpi@4.0.5 ~cuda

set -x
make -C $ROOT clean all