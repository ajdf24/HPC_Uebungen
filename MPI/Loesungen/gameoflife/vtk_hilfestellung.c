void writeVTK(unsigned* currentfield, int t, int startX, int startY, int endX, int endY, int totalW, int totalH, char* prefix) {
  char name[1024] = "\0";
  sprintf(name, "%s_%d.vtk", prefix, t);
  FILE* outfile = fopen(name, "w");

  int w = endX-startX;
  int h = endY-startY;

  /*Write vtk header */
  fprintf(outfile,"# vtk DataFile Version 3.0\n");
  fprintf(outfile,"frame %d\n", t);
  fprintf(outfile,"BINARY\n");
  fprintf(outfile,"DATASET STRUCTURED_POINTS\n");
  fprintf(outfile,"DIMENSIONS %d %d %d \n", w, h, 1);
  fprintf(outfile,"SPACING 1.0 1.0 1.0\n");//or ASPECT_RATIO
  fprintf(outfile,"ORIGIN %d %d 0\n", startX, startY);
  fprintf(outfile,"POINT_DATA %d\n", w*h);
  fprintf(outfile,"SCALARS data float 1\n");
  fprintf(outfile,"LOOKUP_TABLE default\n");

  for (int y = startY; y < endY; y++) {
    for (int x = startX; x < endX; x++) {
      float value = currentfield[calcIndex(totalW, x,y)]; // != 0.0 ? 1.0:0.0;

      value = convert2BigEndian(value);
      fwrite(&value, 1, sizeof(float), outfile);
    }
  }
  fclose(outfile);
}
