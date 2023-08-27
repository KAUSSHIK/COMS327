#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/*
 --------------------------------------- RUNNING INSTRUCTIONS ---------------------------------------
  TO RUN: make run
  TO COMPILE: make
  TO CLEAN: make clean
  ---------------------------------------------------------------------------------------------------
*/

/* Do not modify write_pgm() or read_pgm() */
int write_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *o;

  if (!(o = fopen(file, "w"))) {
    perror(file);

    return -1;
  }

  fprintf(o, "P5\n%u %u\n255\n", x, y);

  /* Assume input data is correctly formatted. *
   * There's no way to handle it, otherwise.   */

  if (fwrite(image, 1, x * y, o) != (x * y)) {
    perror("fwrite");
    fclose(o);

    return -1;
  }

  fclose(o);

  return 0;
}

/* A better implementation of this function would read the image dimensions *
 * from the input and allocate the storage, setting x and y so that the     *
 * user can determine the size of the file at runtime.  In order to         *
 * minimize complication, I've written this version to require the user to  *
 * know the size of the image in advance.                                   */
int read_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *f;
  char s[80];
  unsigned i, j;

  if (!(f = fopen(file, "r"))) {
    perror(file);

    return -1;
  }

  if (!fgets(s, 80, f) || strncmp(s, "P5", 2)) {
    fprintf(stderr, "Expected P6\n");

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (sscanf(s, "%u %u", &i, &j) != 2 || i != x || j != y) {
    fprintf(stderr, "Expected x and y dimensions %u %u\n", x, y);
    fclose(f);

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (strncmp(s, "255", 3)) {
    fprintf(stderr, "Expected 255\n");
    fclose(f);

    return -1;
  }

  if (fread(image, 1, x * y, f) != x * y) {
    perror("fread");
    fclose(f);

    return -1;
  }

  fclose(f);

  return 0;
}

void edge_detection(uint8_t image[1024][1024], uint8_t out[1024][1024]){
  int8_t sobelX[3][3] = 
      {{-1, 0, 1},
       {-2, 0, 2},
       {-1, 0, 1}};

  int8_t sobelY[3][3] = 
      {{-1, -2, -1},
       {0, 0, 0},
       {1, 2, 1}};

  int width = 1024;
  int height = 1024;
  int n = 3;
  int x,y,i,j;
  for (x = 1; x < width - 1; x++){
    for(y = 1; y < height - 1; y++){
      int8_t accumX = 0;
      int8_t accumY = 0;
      for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
          
          int posX = x - (i - (int) ceil(n/2));
          int posY = y - (j - (int) ceil(n/2));

          if (posX >= 0 && posX < width && posY >= 0 && posY < height) {
            accumX += image[posX][posY] * sobelX[i][j];
            accumY += image[posX][posY] * sobelY[i][j];
          }
        }
      }
      int magnitude = sqrt(accumX * accumX + accumY * accumY);
      // Normalize and map to 0-255 range
      //out[x][y] = (uint8_t) (255.0 * magnitude / (255.0 * sqrt(2.0)));
      out[x][y] = (uint8_t) magnitude;
    }

  }
}

int main(int argc, char *argv[])
{
  uint8_t image1[1024][1024];
  uint8_t out1[1024][1024];

  uint8_t image2[1024][1024];
  uint8_t out2[1024][1024];
  
  /* Example usage of PGM functions */
  /* This assumes that motorcycle.pgm is a pgm image of size 1024x1024 */
  read_pgm("bigger_digger.pgm", image1, 1024, 1024);
  edge_detection(image1, out1);
  write_pgm("bigger_digger.edge.pgm", out1, 1024, 1024);

  read_pgm("motorcycle.pgm", image2, 1024, 1024);
  edge_detection(image2, out2);
  write_pgm("motorcycle.edge.pgm", out2, 1024, 1024);
  /* After processing the image and storing your output in "out", write *
   * to motorcycle.edge.pgm.                                            */
  
  return 0;
}
