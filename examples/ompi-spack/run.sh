#!/bin/bash
set -e

if [ -z "$NODES" ]
then
	NODES=2
fi
echo "Testing $NODES nodes"

if [ -z "$HOSTS" ]
then
	echo "Please specify nodes with \$HOSTS"
	exit
fi

ROOT=$PWD/../../
source /opt/spack/share/spack/setup-env.sh
spack load openmpi@3.1.6 ~cuda

set -x
$(which mpirun) -n $NODES -host $HOSTS $ROOT/machine_tester