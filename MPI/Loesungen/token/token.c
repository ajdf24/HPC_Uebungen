#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define N 3

int rank, size;

int tokenA = 0;
int tokenB[N] = {0,0,0};

static void writeData(void) {
  printf("%d is writing data\n", rank);
  fflush(stdout);
}


static void a(void) {
  //TODO part a
  // Use token-a
  while(1){
    //printf("in while");
    if(tokenA == 0){
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
  while(1){
    //printf("in while");
    int i;
    int wasWritten = 0;
    for(i = 0; i < N; i++){
      if(tokenB[i] == 0){
        tokenB[i] = 1;
        writeData();
        wasWritten = 1;
        tokenB[i] = 0;
        break;
      }
    }
    if(wasWritten)
      break;

  }

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
