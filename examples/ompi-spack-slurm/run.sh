#!/bin/bash
set -e

if [ -z "$NODES" ]
then
	NODES=2
fi
echo "Testing $NODES nodes"

ROOT=$PWD/../../
source /opt/spack/share/spack/setup-env.sh
spack load openmpi@4.0.5 ~cuda

set -x
srun -n $NODES -N $NODES $ROOT/machine_tester