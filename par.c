#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


#define ISIZE 5000
#define JSIZE 5000

int min(int x, int y)
{
    if(x<y)
        return x;
    return y;
}

int main(int argc, char **argv) {

    int rank, size;

    MPI_Init (&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Request requestSend;

    double *mainBuf, *recBuf, *sendBuf, *localBuf;
    int i, j;
    int batchSize = 0;
    FILE *ff;

    batchSize = ISIZE / size;


    localBuf = (double*)malloc(batchSize * JSIZE * sizeof(double));
    mainBuf = (double*)malloc(ISIZE * JSIZE * sizeof(double));

    recBuf = (double*)malloc(JSIZE * sizeof(double));
    sendBuf = (double*)malloc(ISIZE * sizeof(double));


    double start = MPI_Wtime();
    for (i = rank * batchSize; i < min(ISIZE, (rank + 1) * batchSize); i++) {
        for (j = 0; j < JSIZE; j++) {

            *(localBuf + (i - rank * batchSize) * ISIZE + j) = 10 * i + j;

            if (rank != 0 && i == rank * batchSize) {
                sendBuf[j] = 10 * i + j;
            }

        }
    }

    if(size > 1) {
        if (rank != 0)
            MPI_Isend(sendBuf, JSIZE, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requestSend);


        if (rank != size - 1)
            MPI_Recv(recBuf, JSIZE, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


        if (rank == 0) {
            for (i = 1; i < batchSize; i++) {
                for (j = 6; j < JSIZE - 1; j++) {
                    if (i == batchSize - 1) {
                        *(localBuf + ISIZE * i + j) = sin(0.00001 * recBuf[j - 6]);
                    } else
                        *(localBuf + ISIZE * i + j) = sin(0.00001 * (*(localBuf + ISIZE * (i + 1) + j - 6)));

                }
            }
        } else {
            if (rank == size - 1) {
                for (i = 0; i < batchSize - 1; i++) {
                    for (j = 6; j < JSIZE - 1; j++) {
                        *(localBuf + ISIZE * i + j) = sin(0.00001 * (*(localBuf + ISIZE * (i + 1) + j - 6)));

                    }
                }
            } else
                for (i = 0; i < batchSize; i++) {
                    for (j = 6; j < JSIZE - 1; j++) {
                        if (i == batchSize - 1) {
                            *(localBuf + ISIZE * i + j) = sin(0.00001 * recBuf[j - 6]);
                        } else
                            *(localBuf + ISIZE * i + j) = sin(0.00001 * (*(localBuf + ISIZE * (i + 1) + j - 6)));

                    }
                }
        }

        MPI_Gather(localBuf, batchSize * JSIZE, MPI_DOUBLE, mainBuf, batchSize * JSIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    } else{
        for (i = 1; i < batchSize - 1; i++) {
            for (j = 6; j < JSIZE - 1; j++) {
                *(localBuf + ISIZE * i + j) = sin(0.00001 * (*(localBuf + ISIZE * (i + 1) + j - 6)));

            }

        }
    }

    double end = MPI_Wtime();
    if(rank == 0) {
        ff = fopen("result_par1.txt", "w");

        for (i = 0; i < ISIZE; i++) {
            for (j = 0; j < JSIZE; j++) {
                if(size > 1)
                    fprintf(ff, "%f ", *(mainBuf + ISIZE * i + j));
                else
                    fprintf(ff, "%f ", *(localBuf + ISIZE * i + j));
//                if(i == ISIZE - 1)
//                    printf("%f ",mainBuf[i][j]);

            }
            fprintf(ff, "\n");

        }

        fclose(ff);
        printf("Time is %f for %d slots\n", end - start, size);
    }

    free(localBuf);
    free(mainBuf);

    free(recBuf);
    free(sendBuf);

    MPI_Finalize();
    return 0;
}