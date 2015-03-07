=======
Übung 2
=======

Aufgabe 1
=========

a)
  Bei der Ausführung kann beobachtet werden, das ein Philosoph immer mehrmals hintereinander denkt und isst.

  Nach einer Weile erfolgt ein Wechsel und ein anderer Philosoph isst bzw. denkt.
  Diese Beobachtung wiederholt sich endlos.

b)
  Unsere Philosophen sind höflich, nachdem sie geggessen haben, denken sie ersteinmal wieder eine weile nach, dies verhindert Deadlocks, da ihre Kolegen, welche essen wollen in der Zwischenzeit sich die Gabel nehmen/locken können.

  ::

    #include <omp.h>
    #include <stdio.h>
    #include <stdlib.h>

    // number of philosophers
    #define N 5
    // left neighbour
    #define LEFT  (id)
    // right neighbour
    #define RIGHT ((id + 1) % num_threads)

    #define TRUE  1
    #define FALSE 0

    // Global variables
    int num_threads;
    omp_lock_t forks[N];

    void think(int philosopher) {
      printf("%d is thinking.\n", philosopher);
    }
    void eat(int philosopher) {
      printf("%d is eating.\n", philosopher);
    }

    void philosopher(int id) {
      while(TRUE) {
        think(id);
        sleep(1);

        omp_set_lock(&forks[LEFT]);
        omp_set_lock(&forks[RIGHT]);
        eat(id);
        omp_unset_lock(&forks[LEFT]);
        omp_unset_lock(&forks[RIGHT]);
      }
    }

    int main (int argc, char *argv[]) {
      int i;
      int id;

      for (i = 0; i < N; i++){
        omp_init_lock(&forks[i]);
      }

      omp_set_num_threads(N);
      #pragma omp parallel private(id) shared(num_threads, forks)
      {
        id = omp_get_thread_num();
        num_threads = omp_get_num_threads();

        philosopher(id);
      }

      for (i = 0; i < N; i++){
        omp_destroy_lock(&forks[i]);
      }
      return 0;
    }

Aufgabe 2
=========

a)
  Ohne weitere Ändungerung am Programm passiert nichts außer das 2 Threads mit jeweils 100% Auslastung starten.

b) ::

    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <omp.h>

    #define NUMITER 26

    #define TRUE 1
    #define FALSE 0

    typedef struct parallelstack {
      omp_lock_t stacklock;  //lock for accessing the stack
      int cancel;            //flag that indicates if threads should stop working
      char *buffer;          //stack elements
      int size;              //size of the stack
      int count;             //current position in the stack
    } ParallelStack;

    static inline ParallelStack* newParallelStack() {
      return calloc(1, sizeof(ParallelStack));
    }

    static inline ParallelStack* ParallelStack_init(ParallelStack* pq, int size) {
      omp_init_lock(&pq[0].stacklock);

      return pq;
    }

    static inline ParallelStack* ParallelStack_deinit(ParallelStack* pq) {

      omp_destroy_lock(&pq[0].stacklock);
      return pq;
    }

    static inline ParallelStack* freeParallelStack(ParallelStack* pq) {
      free(pq);
      return pq;
    }

    static int ParallelStack_put(ParallelStack* pq, char item) {
      int writtenChars = FALSE; // TRUE if the stack was abel to put the data, FALSE if the stack is full, the data will be rejected
      omp_set_lock(&pq[0].stacklock);

      omp_unset_lock(&pq[0].stacklock);
      return writtenChars;
    }

    int ParallelStack_get(ParallelStack* pq, char *c) {
      int numReadedChars = 0; // TRUE if the stack was abel to get the data, FALSE if the stack is empty
      omp_set_lock(&pq[0].stacklock);

      omp_unset_lock(&pq[0].stacklock);
      return numReadedChars;
    }

    void ParallelStack_setCanceled(ParallelStack* pq) {
      omp_set_lock(&pq[0].stacklock);

      omp_unset_lock(&pq[0].stacklock);
    }


    int ParallelStack_isCanceled(ParallelStack* pq) {
      int canceled = FALSE;
      omp_set_lock(&pq[0].stacklock);

      omp_unset_lock(&pq[0].stacklock);
      return canceled;
    }

    /////////////////////////////////////////
    // DO NOT EDIT BEYOND THIS LINE !!!!
    /////////////////////////////////////////

    void producer(int tid, ParallelStack* pq) {
      int i = 0;
      char item;
      while( i < NUMITER) {
        item = 'A' + (i % 26);

        if ( ParallelStack_put(pq, item) == 1) {
          i++;
          printf("->Thread %d is Producing %c ...\n",tid, item);
        }
        //sleep(1);
       }
       ParallelStack_setCanceled(pq);
    }


    void consumer(int tid, ParallelStack* pq)
    {
      char item;
      while( ParallelStack_isCanceled(pq) == FALSE) {

        if (ParallelStack_get(pq, &item) == 1) {
          printf("<-Thread %d is Consuming %c\n",tid, item);
        }
        sleep(2);
      }
    }

    int main()
    {
        int tid;
        ParallelStack* pq = ParallelStack_init(newParallelStack(), 5);

        #pragma omp parallel private(tid) num_threads(4)
        {
           tid=omp_get_thread_num();

           if(tid==1)
           {
             producer(tid, pq);
           } else
           {
             consumer(tid, pq);
           }
        }

        freeParallelStack(ParallelStack_deinit(pq));

        return 0;
    }

c)

d)

e)

f)
  Wenn bei einer Queue nur ein Lock verwendet wird, dann kann ein hinzufügen eines Elements zu der Queue den Zugriff auf das vorderste Elemet der Queue blockieren. Im Gegensatz zum Stack sind diese beiden Operationen nicht immer von einander abhängig. Sie blockieren sich nur, wenn die Queue leer ist.
