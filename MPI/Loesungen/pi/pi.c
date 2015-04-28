#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define TRYS 5000000

int rank, size;

static int throw() {
  double x, y;
  x = (double)rand() / (double)RAND_MAX;
  y = (double)rand() / (double)RAND_MAX;
  if ((x*x + y*y) <= 1.0) return 1;

  return 0;
}

static void a(double pi) {
  double receivedPi;
  int length = 1;
  MPI_Status status;
  //Ring
  //TODO part a
  if(rank == 0){
    MPI_Send(&pi, length, MPI_DOUBLE, rank + 1, 99, MPI_COMM_WORLD);
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, size - 1, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    printf("pi from Ring is %.9lf\n", pi);
  }else if(rank < size - 1){
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, rank - 1, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    MPI_Send(&pi, length, MPI_DOUBLE, rank + 1, 99, MPI_COMM_WORLD);
  }else{
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, rank - 1, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    MPI_Send(&pi, length, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
  }
}

static void b(double pi) {
  //Nachbar austausch
  double receivedPi;
  int length = 1;
  MPI_Status status;
  if(rank == 0){
    MPI_Send(&pi, length, MPI_DOUBLE, rank + 1, 99, MPI_COMM_WORLD);
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, size - 1, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    MPI_Send(&pi, length, MPI_DOUBLE, size -1 , 99, MPI_COMM_WORLD);
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, rank + 1, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    printf("pi from Austausch is %.9lf\n", pi);
  }else if(rank < size - 1){
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, rank - 1, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    MPI_Send(&pi, length, MPI_DOUBLE, rank + 1, 99, MPI_COMM_WORLD);
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, rank + 1, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    MPI_Send(&pi, length, MPI_DOUBLE, rank - 1, 99, MPI_COMM_WORLD);
  }else{
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, rank - 1, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    MPI_Send(&pi, length, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
    MPI_Recv(&receivedPi, 1, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, &status);
    pi = (pi + receivedPi)/2;
    MPI_Send(&pi, length, MPI_DOUBLE, rank - 1, 99, MPI_COMM_WORLD);
  }

}

static void c(double pi) {
  //Collective Pi I

  double sumPi;
  double avageragePi;
  int length = 1;
  int root = 0;

  MPI_Reduce(&pi, &sumPi, length, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD );

  if (rank == root){
    avageragePi = sumPi/size;
    printf("pi from Reduce&BroadCast is %.9lf\n", avageragePi);

    MPI_Bcast(&avageragePi, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
  } else {
    MPI_Bcast(&avageragePi, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
  }
}

static void d(double pi) {
  //Collective Pi II
  double sumPi;
  double avageragePi;
  int length = 1;
  int root = 0;

  MPI_Allreduce(&pi, &sumPi, length, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

  avageragePi = sumPi/size;
  if (rank == root){
    printf("pi from Allreduce is %.9lf\n", avageragePi);
  }
}

int main(int argc, char **argv) {

  long globalCount = 0;
  long globalSamples = TRYS;

  MPI_Init (&argc, &argv);	/* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

  srand(rank);
  //Calc pi
  for(int i = 0; i < globalSamples; ++i)
  {
    globalCount += throw();
  }
  double pi = 4.0 * (double)globalCount / (double)(globalSamples);

  printf("pi from rank %d  is %.9lf\n",rank, pi);

  a(pi);

  b(pi);

  c(pi);

  d(pi);

  MPI_Finalize();
  return 0;
}
