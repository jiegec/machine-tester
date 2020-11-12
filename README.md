# Machine Tester

Usage:

```shell
# Slurm
srun -N $NNODES ./machine_tester
# OpenMPI
mpirun --hostfile -n $NNODES -N 1 ./machine_tester
```