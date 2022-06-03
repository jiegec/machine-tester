# Machine Tester


Build:

```shell
$ mkdir -p build
$ cd build
# Find BLAS automatically
$ cmake ..
# Use OpenBLAS
$ cmake .. -DBLA_VENDOR=OpenBLAS
# Use BLIS
$ cmake .. -DBLA_VENDOR=FLAME
```

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
