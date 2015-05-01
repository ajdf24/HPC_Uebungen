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
  int dim[1], periodic [1];
  periodic[0] = 1;
  int reorder = 1;

  MPI_Init(&c, &v);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  dim[0] = size;

  //decompose Domain and proesses with 1D domain decompositon
  MPI_Cart_create(MPI_COMM_WORLD, 1, dim, periodic, reorder, &card_comm);

  //get reorderd cart_rank and neighbours ranks
  MPI_Cart_coords(card_comm,rank,1,coord);
  int cart_rank;
  MPI_Cart_rank(card_comm, coord, &cart_rank);

  int left_neighbour_rank, right_neighbour_rank, rank_source;
  MPI_Cart_shift(card_comm, 1, -1, &rank_source, &left_neighbour_rank);
  MPI_Cart_shift(card_comm, 1, 1, &rank_source, &right_neighbour_rank);

  printf("Rank ID: %d, Left Neighbour rank: %d , Right Neighbour rank: %d \n", cart_rank, left_neighbour_rank, right_neighbour_rank);

c)
==

::

  char name[1024] = "\0";
  sprintf(name, "%s_%d_%d.vtk", prefix, coord[0], t);
  FILE* outfile = fopen(name, "w");

  /*Write vtk header */
  fprintf(outfile,"# vtk DataFile Version 3.0\n");
  fprintf(outfile,"frame %d\n", t);
  fprintf(outfile,"BINARY\n");
  fprintf(outfile,"DATASET STRUCTURED_POINTS\n");
  fprintf(outfile,"DIMENSIONS %d %d %d \n", w, h, 1);
  fprintf(outfile,"SPACING 1.0 1.0 1.0\n");//or ASPECT_RATIO
  fprintf(outfile,"ORIGIN 0 0 0\n");
  fprintf(outfile,"POINT_DATA %d\n", h*w);
  fprintf(outfile,"SCALARS data float 1\n");
  fprintf(outfile,"LOOKUP_TABLE default\n");

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      float value = currentfield[calcIndex(w, x,y)]; // != 0.0 ? 1.0:0.0;
      value = convert2BigEndian(value);
      fwrite(&value, 1, sizeof(float), outfile);
    }
  }
  fclose(outfile);

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
