=======
Übung 6
=======

a)
==

::

  make run

b)
==

::

  int rank, size, coord[1];
  MPI_Comm card_comm;

  …

  //init MPI
  MPI_Status status;
  int dim[1], periodic [1];
  periodic[0] = 0;
  int reorder = 1;

  MPI_Init(&c, &v);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  dim[0] = size;

  //decompose Domain and proesses with 1D domain decompositon
  MPI_Cart_create(MPI_COMM_WORLD, 1, dim, periodic, reorder, &card_comm);

  MPI_Cart_coords(card_comm,rank,1,coord);

c)
==

Die Ghostboundarys dürfen nicht mit in die Datei geschrieben werden, da diese ja von dem jeweiligen Prozess nicht berechnet wurden, sondern nur von seinen Nachbarprozessen.

d)
==

e)
==

f)
==

::

  int changes = evolve(currentfield, newfield, w, h);

  //exit loop if all processes report no more changes
  int allchanges;
  MPI_Allreduce(&changes, &allchanges, size, MPI_INT, MPI_SUM, card_comm);
  if (allchanges == 0) break;
