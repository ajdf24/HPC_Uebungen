#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>
#include <mpi.h>

#define calcIndex(width, x,y)  ((y)*(width) + (x))

int rank, size, coord[1];
MPI_Comm card_comm;

void show(unsigned* currentfield, int w, int h) {
  printf("\033[H");
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) printf(currentfield[calcIndex(w, x,y)] ? "\033[07m  \033[m" : "  ");
    printf("\033[E");
  }
  fflush(stdout);
}


float convert2BigEndian( const float inFloat )
{
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat    = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}

void writeVTK(unsigned* currentfield, int w, int h, int t, char* prefix) {
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
}

int coutLifingsPeriodic(unsigned* currentfield, int x , int y, int w, int h) {
  int n = 0;
  for (int y1 = y - 1; y1 <= y + 1; y1++) {
    for (int x1 = x - 1; x1 <= x + 1; x1++) {
      if (currentfield[calcIndex(w, (x1 + w) % w, (y1 + h) % h)]) {
        n++;
      }
    }
  }
  return n;
}

int evolve(unsigned* currentfield, unsigned* newfield, int w, int h) {
int changes = 0;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int n = coutLifingsPeriodic(currentfield, x , y, w, h);
      if (currentfield[calcIndex(w, x,y)]) n--;
      newfield[calcIndex(w, x,y)] = (n == 3 || (n == 2 && currentfield[calcIndex(w, x,y)]));

      if (newfield[calcIndex(w, x,y)] - currentfield[calcIndex(w, x,y)] != 0) {
        changes++;
      }
    }
  }
  return changes;
}

void filling(unsigned* currentfield, int w, int h) {
  for (int i = 0; i < h*w; i++) {
    currentfield[i] = (rand() < RAND_MAX / 10) ? 1 : 0; ///< init domain randomly
  }
}

void game(int w, int h, int timesteps) {
  unsigned *currentfield = calloc(w*h, sizeof(unsigned));
  unsigned *newfield     = calloc(w*h, sizeof(unsigned));

  filling(currentfield, w, h);
  for (int t = 0; t < timesteps; t++) {

   //output
   //show(currentfield, w, h);
   writeVTK(currentfield, w, h, t, "output");
   int changes = evolve(currentfield, newfield, w, h);

   //exit loop if all processes report no more changes
   int allchanges;
   MPI_Allreduce(&changes, &allchanges, size, MPI_INT, MPI_SUM, card_comm);
   if (allchanges == 0) break;

   //TODO exchange boundary


   // usleep(200000);

    //SWAP
    unsigned *temp = currentfield;
    currentfield = newfield;
    newfield = temp;
  }

  free(currentfield);
  free(newfield);
}

int main(int c, char **v) {
   int w = 30, h = 30, timesteps = 10;
  if (c > 1) w = atoi(v[1]); ///< read width
  if (c > 2) h = atoi(v[2]); ///< read height
  if (c > 3) timesteps = atoi(v[3]);

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

  //game(w, h, timesteps);

  //deinit MPI
  MPI_Finalize();

  return 0;
}
