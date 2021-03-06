#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define N 3

int rank, size;

int token-a;
int token-b[N];

static void writeData(void) {
  printf("%d is writing data\n", rank);
  fflush(stdout);
}


static void a(void) {

  //TODO part a
  // Use token-a
  writeData();

}


static void b(void) {

  //TODO part b
  //use token-b[]
  writeData();

}




int main(int argc, char **argv) {

  MPI_Init (&argc, &argv);                      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

  a();

  b();

  MPI_Finalize();
  return 0;
}
