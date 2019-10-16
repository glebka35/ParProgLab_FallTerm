# Paralell Programming Laboratory work in FallTerm
MPI is used here. The following functions are used:
```bash
MPI_Isend, MPI_Recv, MPI_Gather
```

## Compilation
To compile you may do the following:
``` bash
mpicc par.c
```

## Running
To run:
```bash
mpirun -np $NUM_of_Threads$ ./a.out
```

## P.s.
This implementation doesn't claim to be the best.
