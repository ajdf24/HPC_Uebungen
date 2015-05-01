=======
Übung 5
=======

Aufgabe 1
=========
a)
-- 
  ::

    cd hello-world
    make run

b)
--
  ::

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
--

  Die Ausgabe ist nicht konstant, weil die Threads bei jeder Ausgabe unterschiedlich schnell sind.

Aufgabe 2
=========
a)
--

  ::

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

b)
--

  ::

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

c)
--

  ::

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

  Aufgabe gelöst über MPI Reduce.

d)
--

  ::

    double sumPi;
    double avageragePi;
    int length = 1;
    int root = 0;

    MPI_Allreduce(&pi, &sumPi, length, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

    avageragePi = sumPi/size;
    if (rank == root){
      printf("pi from Allreduce is %.9lf\n", avageragePi);
    }

  Aufgabe gelößt über MPI ReduceAll.

e+f)
----

.. image:: ../../source/bilder/Aufgabe2_e+f_Austausch.jpg

.. image:: ../../source/bilder/Aufgabe2_e+f_Broadcast.jpg

.. image:: ../../source/bilder/Aufgabe2_e+f_Ring.jpg

.. image:: ../../source/bilder/Aufgabe2_e+f_AllReduce.jpg

Aufgabe 3
=========
a)
--

  ::

    while(1){
      //printf("in while");
      if(tokenA == 0){
        tokenA = 1;
        writeData();
        tokenA = 0;
        break;
      }
    }

  In einer Schleife wird gewartet, bis das Token frei ist. Ist dies der Fall, locked der Thread das Token und schreibt.
  Nach dem schreiben gibt er das Token wieder frei. In der Zeit, wo das Token blockiert ist, kann kein anderer Thread schreiben.


b)
--

  ::

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

  Wie Aufgabe a, so verhält sich auch Aufgabe b, nur das hier drei Token zur Verfügung stehen, welche gleichzeitig allokiert werden können.

c)
--

Da die maximalen Datenraten von Festplatten und Netzwerktechniken begränzt sind, muss der I/O gebremst werden.
Würden extrem viele Prozesse auf einmal auf ein Laufwerke zugreifen, wäre eine Überlastung vorprogrammiert!
