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

  printf("Rank ID: %d, Left Neighbour rank: %d , Right Neighbour rank: %d \n", cart_rank,
    left_neighbour_rank, right_neighbour_rank);

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

::

  int fieldsize = w*((h/size)+2);
  unsigned *currentfield = calloc(fieldsize, sizeof(unsigned));
  unsigned *newfield     = calloc(fieldsize, sizeof(unsigned));

  printf("Rank ID: %d, Coord: %d , Fieldsize: %d \n", cart_rank, coord[0], fieldsize);

  filling(currentfield, w, h/size);

Das Feld wird vertikal durch die Anzahl der Prozesse geteilt. Bei 5 Prozessen und 30 Felder berechnet also jeder Prozess 6 Reihen. Zusätzlich bekommt jedes Feld noch 2 Reihen für den Ghostboundary.

e)
==

::

  //exchange boundary
    int data_to_left[w], data_to_right[w], data_from_left[w], data_from_right[w];

    for(int i = 0; i < w; ++i){
     data_to_left[i] = currentfield[calcIndex(w, i, 1)];
     data_to_right[i] = currentfield[calcIndex(w, i, gridh-1)];
    }

    //send to left neighbour
    MPI_Request to_left;
    MPI_Isend(&data_to_left,  w, MPI_INT, left_neighbour_rank, 0, card_comm, &to_left);

    //send to right neighbour
    MPI_Request to_right;
    MPI_Isend(&data_to_right, w, MPI_INT, right_neighbour_rank, 0, card_comm, &to_right);

    //recive from left neighbour
    MPI_Request from_left;
    MPI_Irecv(&data_from_left, w, MPI_INT, left_neighbour_rank, 0, card_comm, &from_left);

    //recive from right neighbour
    MPI_Request from_right;
    MPI_Irecv(&data_from_right, w, MPI_INT, right_neighbour_rank, 0, card_comm, &from_right);

    //wait
    MPI_Status status_left;
    MPI_Status status_right;
    MPI_Wait(&from_left, &status_left);
    MPI_Wait(&from_right, &status_right);

    //update boundaries
    for(int i = 0; i < w; ++i){
     currentfield[calcIndex(w, i, 0)] = data_from_left[i];
     currentfield[calcIndex(w, i, gridh)] = data_from_right[i];
    }

Bei einer 1D-Gebietszerlegung wird nur in eine Richtung kommuniziert, da sich die andere Richtung komplett im eigenen Gebiet befindet und deshalb nicht, ausgetauscht werden muss. In diesem Fall wurden die Reihen augetauscht, weil dies Speichertechnisch sinnvoller ist.

Bei der Buffervariante ist die Übertragung simpler, weil kein Datentype definitiert werden muss. Dafür kann man sich bei der Datentype-Variante auf den Datentype verlassen.

f)
==

::

  //exit loop if all processes report no more changes
  int allchanges;
  MPI_Allreduce(&changes, &allchanges, size, MPI_INT, MPI_SUM, card_comm);
  if (allchanges == 0) break;

g)
==

  Bei der 2D Zerlegung muss der Randaustausch in beide Richtungen durchgeführt werden. Was ihn komplizierter macht.

  Außerdem muss die Anzahl der Prozesse sich auf das Gebiet aufteilen lassen. Also ``w%y=0`` und ``h%x=0`` sowie ``y*w=n``, wo bei x= Prozesse in Dimension1, y = Prozesse in Dimension 2 und n=Anzahl der Prozesse.
