# Machine Tester

Usage:

```shell
# Slurm
srun -N $NNODES ./machine_tester
# OpenMPI
mpirun --hostfile -n $NNODES -N 1 ./machine_tester
# Run w/o TCP
OMPI_MCA_btl=^tcp mpirun ...
# Run w/o OpenIB
OMPI_MCA_btl=^openib mpirun ...
```