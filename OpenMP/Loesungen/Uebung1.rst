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
        printf("Hola Mundo from thread %d of %d\n",
          omp_get_thread_num(), omp_get_num_threads());
        printf("Hej varlden from thread %d of %d\n",
          omp_get_thread_num(), omp_get_num_threads());
      }

      #pragma omp section
      {
        printf("Bonjour tout from thread %d of %d\n",
          omp_get_thread_num(), omp_get_num_threads());
      }

      #pragma omp section
      {
        printf("Hallo Welt from thread %d of %d\n",
          omp_get_thread_num(), omp_get_num_threads());
      }
      #pragma omp section
      {
        printf("Hello World from thread %d of %d\n",
          omp_get_thread_num(), omp_get_num_threads());
      }
    }
    return 0;
  }

Aufgabe 2
=========

a)
--

Der Fehler in error1 tritt nur auf, wenn mehr als 2 Threads verwendet werden.

Von l.36 bis l.55 wird das Programm in zwei Sections aufgeteilt welche von 2 Threads abgearbeitet werden. Wegen dem "nowait" Befehl laufen alle anderen Threads gegen die Barrier in l.58 und warten auf die Threads, welche den Sectionblock abarbeiten.

Diese Threads laufen aber gegen die Barrier in l.86 und warten dort auf die anderen Threads, welche diesen Codeteil nicht ausführen.

Für die Behebung gibt es also 2 Möglichkeiten.

  1. OMP_NUM_THREADS=2

  2. l.86 löschen

b)
--

Der Fehler tritt durch ein Deadlock auf, dies geschieht da die beiden Sections ihre Locks nicht zu Beginn setzen.

In l.45 locked der erste Thread locka und zeitgleich in l. 59 wird durch den anderen Thread lockb gelocked. Laufen nun beide Threads weiter wartet der erste Thread in l.48 auf das unlock von lockb und der andere Thread in l.62 auf das unlock von locka.

Der Fehler kann wieder durch mindesten 2 Arten gelöst werden.

  1. Der Bereich wird nicht paraellisiert sondern sequentiell hintereinander ausgeführt.

  2. Jede Section locked zu beginn all ihre locks in der gleichen Reihenfolge. ::

  ::

    #include <omp.h>
    #include <stdio.h>
    #include <stdlib.h>
    #define N 1000000
    #define PI 3.1415926535
    #define DELTA .01415926535

    int main (int argc, char *argv[])
    {
    int nthreads, tid, i;
    float a[N], b[N];
    omp_lock_t locka, lockb;

    /* Initialize the locks */
    omp_init_lock(&locka);
    omp_init_lock(&lockb);

    /* Fork a team of threads giving them their own copies of variables */
    #pragma omp parallel shared(a, b, nthreads, locka, lockb) private(tid)
      {

      /* Obtain thread number and number of threads */
      tid = omp_get_thread_num();
      #pragma omp master
        {
        nthreads = omp_get_num_threads();
        printf("Number of threads = %d\n", nthreads);
        }
      printf("Thread %d starting...\n", tid);
      #pragma omp barrier

      #pragma omp sections nowait
        {
        #pragma omp section
          {
          printf("Thread %d initializing a[]\n",tid);
          omp_set_lock(&locka);
          omp_set_lock(&lockb);
          for (i=0; i<N; i++)
            a[i] = i * DELTA;
          printf("Thread %d adding a[] to b[]\n",tid);
          for (i=0; i<N; i++)
            b[i] += a[i];
          omp_unset_lock(&lockb);
          omp_unset_lock(&locka);
          }

        #pragma omp section
          {
          printf("Thread %d initializing b[]\n",tid);
          omp_set_lock(&locka);
          omp_set_lock(&lockb);
          for (i=0; i<N; i++)
            b[i] = i * PI;
          printf("Thread %d adding b[] to a[]\n",tid);
          for (i=0; i<N; i++)
            a[i] += b[i];
          omp_unset_lock(&locka);
          omp_unset_lock(&lockb);
          }
        }  /* end of sections */
      }  /* end of parallel region */

    }


  3. Nur einen CPU Core verwenden, weil denn die Sections auch sequentiell und nicht parallel ausgeführt werden.

Aufgabe 3
=========

a)
--

::

  make run

b)
--

::

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <omp.h>

  #define TRYS 5000000

  static int throw() {
    double x, y;
    x = (double)rand() / (double)RAND_MAX;
    y = (double)rand() / (double)RAND_MAX;
    if ((x*x + y*y) <= 1.0) return 1;

    return 0;
  }

  int main(int argc, char **argv) {
    int globalCount = 0, globalSamples=TRYS, i;

    #pragma omp parallel for private(i) shared(globalCount)
    for(i = 0; i < globalSamples; ++i) {
      int add = throw();
      if (add != 0){
        #pragma omp atomic
    		globalCount += add;
      }
    }

    double pi = 4.0 * (double)globalCount / (double)(globalSamples);

    printf("pi is %.9lf\n", pi);

    return 0;
  }

c)
--

::

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <omp.h>

  #define TRYS 5000000

  static int throw() {
    double x, y;
    x = (double)rand() / (double)RAND_MAX;
    y = (double)rand() / (double)RAND_MAX;
    if ((x*x + y*y) <= 1.0) return 1;

    return 0;
  }

  int main(int argc, char **argv) {
    int globalCount = 0, globalSamples=TRYS, i;

    #pragma omp parallel for reduction(+:globalCount)
    for(i = 0; i < globalSamples; ++i) {
      int add = throw();
      if (add != 0){
    		globalCount += add;
      }
    }

    double pi = 4.0 * (double)globalCount / (double)(globalSamples);

    printf("pi is %.9lf\n", pi);

    return 0;
  }

d)
--

::

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <omp.h>

  #define TRYS 5000000

  static int throw() {
    double x, y;
    x = (double)rand() / (double)RAND_MAX;
    y = (double)rand() / (double)RAND_MAX;
    if ((x*x + y*y) <= 1.0) return 1;

    return 0;
  }

  int main(int argc, char **argv) {
    int globalCount = 0, globalSamples=TRYS, i;

    #pragma omp parallel reduction(+:globalCount)
    {
      #pragma omp for
        for(i = 0; i < globalSamples; ++i) {
          int add = throw();
          if (add != 0){
            globalCount += add;
          }
        }

      printf("thread %d: i = %d\n", omp_get_thread_num(), globalCount);
    }


    double pi = 4.0 * (double)globalCount / (double)(globalSamples);

    printf("pi is %.9lf\n", pi);

    return 0;
  }

e)
--

::

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <omp.h>

  #define TRYS 5000000

  static int throw() {
    double x, y;
    x = (double)rand() / (double)RAND_MAX;
    y = (double)rand() / (double)RAND_MAX;
    if ((x*x + y*y) <= 1.0) return 1;

    return 0;
  }

  int main(int argc, char **argv) {
    int globalCount = 0, globalSamples=TRYS, i;

    #pragma omp parallel reduction(+:globalCount) num_threads(6)
    {
      #pragma omp for
        for(i = 0; i < globalSamples; ++i) {
          int add = throw();
          if (add != 0){
            globalCount += add;
          }
        }

      printf("thread %d: i = %d\n", omp_get_thread_num(), globalCount);
    }


    double pi = 4.0 * (double)globalCount / (double)(globalSamples);

    printf("pi is %.9lf\n", pi);

    return 0;
  }

Durch das ``num_threads(6)`` wird unterbunden, dass der Benutzer die Threadanzahl verändern kann. Er könnte dies ohne diese Angabe durch setzen von  ``OMP_NUM_THREADS`` tun.
