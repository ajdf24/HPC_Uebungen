#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define N 3

int rank, size;

int tokenA = 0;
int tokenB[N];

static void writeData(void) {
  printf("%d is writing data\n", rank);
  fflush(stdout);
}


static void a(void) {
  printf("tokenA %d\n", tokenA);
  //TODO part a
  // Use token-a
  // printf("in if %d", rank);
  while(1){
    //printf("in while");
    if(tokenA == 0){
      printf("in if %d", rank);
      tokenA = 1;
      writeData();
      tokenA = 0;
      break;
    }
  }


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
