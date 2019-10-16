#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ISIZE 2000
#define JSIZE 2000

int main(int argc, char **argv) {
    double *a;
    int i, j;

    int rank, size;

    MPI_Init (&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    a = (double*)malloc(ISIZE * JSIZE * sizeof(double));

    FILE *ff;
    double start = MPI_Wtime();

    for (i=0; i<ISIZE; i++)
    {
        for (j=0; j<JSIZE; j++)
        {
            *(a + ISIZE * i + j) = 10 * i + j;
        }
    }

    for (i=1; i<ISIZE-1; i++)
    {
        for (j = 6; j < JSIZE-1; j++)
        {
            *(a + ISIZE * i + j) = sin(0.00001 * (*(a + ISIZE * (i + 1) + j - 6)));
        }
    }

    double end = MPI_Wtime();
    ff = fopen("result.txt","w");
    for(i=0; i < ISIZE; i++)
    {
        for (j=0; j < JSIZE; j++)
        {
            fprintf(ff,"%f ", *(a + ISIZE * i + j));
        }
        fprintf(ff,"\n");
    }

    fclose(ff);
    printf("Time is %f for 1 slot\n", end - start);
    MPI_Finalize();
}