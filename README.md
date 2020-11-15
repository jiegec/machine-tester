# Machine Tester

Usage:

```shell
# Slurm
srun -N $NNODES ./machine_tester
# OpenMPI
mpirun --hostfile -n $NNODES -N 1 ./machine_tester
# Run w/o TCP BTL
mpirun -mca btl ^tcp ...
# Run w/ UCX PML
mpirun -mca pml ucx ...
```
