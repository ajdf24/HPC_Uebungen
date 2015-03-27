=======
Übung 2
=======

Aufgabe 1
=========
a) ::

    cd hello-world
    make run

b) ::

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <mpi.h>

    int main(int argc, char **argv) {
      int rank = 0;
      int size = 0;

      MPI_Init (&argc, &argv);      /* starts MPI */
      MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
      MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
      printf( "Hello world from process %d of %d\n", rank, size );
      MPI_Finalize();
      return 0;
    }

c)

  Die Ausgabe ist nicht konstant, weil die Threads bei jeder Ausgabe unterschiedlich schnell sind.
