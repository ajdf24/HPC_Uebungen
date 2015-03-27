#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int rank = 0;
  int size = 0;
  
  MPI_Init (&argc, &argv);


  MPI_Finalize();
  return 0;
}
