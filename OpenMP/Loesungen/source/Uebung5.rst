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

Aufgabe 2
=========
a)::

  static void a(double pi) {
    double receivedPi;
    int length = 1;
    MPI_Status status;
    //Ring
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


  Jeder Prozess, sendet und empfängt eine Nachricht. Somit werden n Sende- und Empfangsschritte benötigt, wobei n die Anzahl der Prozesse darstellt.

b)::

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

  Jeder Prozess, sendet und empfängt zwei Nachrichten. Somit werden n*2 Sende- und Empfangsschritte benötigt, wobei n die Anzahl der Prozesse darstellt.

c)::


FEHLT NOCH!!!!

d)::


FEHLT NOCH!!!

e+f)::

.. image:: ../../source/bilder/Aufgabe2_e+f_Austausch.jpg

.. image:: ../../source/bilder/Aufgabe2_e+f_Broadcast.jpg

.. image:: ../../source/bilder/Aufgabe2_e+f_Ring.jpg

Aufgabe 2
=========
a)::

FEHLT NOCH!!!!

b)::


FEHLT NOCH!!!

c)::

Da die maximalen Datenraten von Festplatten und Netzwerktechniken begränzt sind, muss der I/O gebremst werden.
Würden extrem viele Prozesse auf einmal auf ein Laufwerke zugreifen, wäre eine Überlastung vorprogrammiert!
