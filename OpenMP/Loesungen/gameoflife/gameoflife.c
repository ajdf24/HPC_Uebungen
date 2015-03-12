#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>
#include <omp.h>
#include <math.h>

int calcIndex(int width, int heigth, int x, int y) {
  if(x > width){
    x = 0;
  }else if(x < 0){
    x = width;
  }
  if(y > heigth){
    y = 0;
  }else if(y < 0){
    y = heigth;
  }
  return y * width + x;
}

void show(unsigned* currentfield, int w, int h) {
  printf("\033[H");
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) printf(currentfield[calcIndex(w,h,x,y)] ? "\033[07m  \033[m" : "  ");
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
  sprintf(name, "%s_%d.vtk", prefix, t);
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
      float value = currentfield[calcIndex(w, h, x,y)]; // != 0.0 ? 1.0:0.0;
      value = convert2BigEndian(value);
      fwrite(&value, 1, sizeof(float), outfile);
    }
  }
  fclose(outfile);
}


int evolve(unsigned* currentfield, unsigned* newfield, int w, int h) {
int changes = 0;

  for (int y = 1; y < h-1; y++) {
    for (int x = 1; x < w-1; x++) {
      int sum = 0;
      sum += currentfield[calcIndex(w,h,x+1,y)];
      sum += currentfield[calcIndex(w,h,x+1,y+1)];
      sum += currentfield[calcIndex(w,h,x+1,y-1)];

      sum += currentfield[calcIndex(w,h,x-1,y)];
      sum += currentfield[calcIndex(w,h,x-1,y+1)];
      sum += currentfield[calcIndex(w,h,x-1,y-1)];

      sum += currentfield[calcIndex(w,h,x,y-1)];
      sum += currentfield[calcIndex(w,h,x,y+1)];
      sum += currentfield[calcIndex(w,h,x,y)];

      if(sum == 2 || sum == 3){
        newfield[calcIndex(w,h,x,y)] = 1;
      } else {
        newfield[calcIndex(w,h,x,y)] = 0;
      }

      if(changes==0 && currentfield[calcIndex(w,h,x,y)] != newfield[calcIndex(w,h,x,y)]){
        changes = 1;
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
    int num_threads = omp_get_num_threads();

    int subwidth = w / sqrt(num_threads);
    int subheight = h / sqrt(num_threads);

    #pragma omp parallel {
      int tid = omp_get_thread_num();
      unsigned *subfield = calloc((2+subwidth)*(2+subheight), sizeof(unsigned));
      unsigned *newsubfield = calloc(subwidth*subheight, sizeof(unsigned));

      int row = 0;
      for(int i = ((w/num_threads)*pid)-1; i < (w*h); i = i + w){
        for(int j = -1; j < (subwidth + 1); ++j){
          subfield[j+1 + row*(subwidth + 2)] = currentfield[calcIndex(w, h, i, j)];
        }
        row++;
      }

      int changes = evolve(subfield, newsubfield, subwidth+2, subheight+2);
      writeVTK(subfield, w, h, t, "output_"+ pid);

      row=0;
      for(int i = (w/num_threads)*pid ; i < w*h; i = i + w ){
        for(int j = 0; j < subwidth ; j++){
          newfield[i+j] = newsubfield[row*subwidth +j];
        }
        row++;
      }
    }

    usleep(100000);

    //SWAP
    unsigned *temp = currentfield;
    currentfield = newfield;
    newfield = temp;
  }

  free(currentfield);
  free(newfield);
}

int main(int c, char **v) {

  int w = 0, h = 0, timesteps = 10;
  if (c > 1) w = atoi(v[1]); ///< read width
  if (c > 2) h = atoi(v[2]); ///< read height
  if (c > 3) timesteps = atoi(v[3]);
  if (w <= 0) w = 30; ///< default width
  if (h <= 0) h = 30; ///< default height
  game(w, h, timesteps);
}
