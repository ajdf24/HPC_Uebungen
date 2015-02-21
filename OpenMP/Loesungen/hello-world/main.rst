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
  #include <omp.h>

  int main(int argc, char **argv) {

    #pragma omp parallel num_threads(4)
    {
      int num_threads = omp_get_num_threads();
      int this_num = omp_get_thread_num();
      printf("Hello World %d von %d\n", this_num, num_threads);
    }
    return 0;
  }

c)
--

Die Ausgabe ist nicht konstant, weil die Threads bei jeder Ausgabe unterschiedlich schnell sind.

d)
--

::

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <omp.h>

  int main(int argc, char **argv) {
    #pragma omp parallel sections num_threads(4)
    {
      #pragma omp section
      {
        printf("Hola Mundo from thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
        printf("Hej varlden from thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
      }

      #pragma omp section
      {
        printf("Bonjour tout from thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
      }

      #pragma omp section
      {
        printf("Hallo Welt from thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
      }
      #pragma omp section
      {
        printf("Hello World from thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
      }
    }
    return 0;
  }
