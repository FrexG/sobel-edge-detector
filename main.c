#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raylib.h>

#define WINDOW_H 640 
#define WINDOW_W 640 

Color *toGrayScale(Color *pixels, size_t img_w, size_t img_h)
{
  Color *grayscale = calloc(img_w * img_h, sizeof(Color));

  for (int i = 0; i < img_h; ++i)
  {
    for (int j = 0; j < img_w; ++j)
    {

      unsigned char r = pixels[img_w * i + j].r;
      unsigned char g = pixels[img_w * i + j].g;
      unsigned char b = pixels[img_w * i + j].b;
      unsigned char a = pixels[img_w * i + j].a;

      unsigned char v = (r + g + b) / 3;

      Color c = {.r = v, .g = v, .b = v, .a = 255};

      grayscale[img_w * i + j] = c;
    }
  }

  return grayscale;
}

void gaussianBlur(Color *pixels,size_t img_w, size_t img_h){
  float filter[3][3] = {{0.07511361,0.1238414,0.07511361},{0.1238414,0.20417996 ,0.1238414 }, {0.07511361, 0.1238414,  0.07511361}};
  Color *blured= calloc(img_w * img_h, sizeof(Color));

  for (size_t i = 0; i < img_h; ++i)
  {
    for (size_t j = 0; j < img_w; ++j)
    {
      float val = 0;
      // loop through the nine neighbouring pixels
      for (int k = -1; k <= 1; ++k)
      {
        for (int l = -1; l <= 1; ++l)
        {
          if (i + k < 0 || i + k >= img_h)
            continue;
          if (j + l < 0 || j + l >= img_w)
            continue;
          val += (pixels[img_w * (i + k) + (j + l)].r) * filter[1 + k][1 + l];
        }
      }
      blured[img_w * i + j].r = (unsigned char)fmin(fmax(val, 0), 255);
      blured[img_w * i + j].g = blured[img_w * i + j].r;
      blured[img_w * i + j].b = blured[img_w * i + j].r;
      blured[img_w * i + j].a = pixels[img_w * i + j].a; // Preserve the original alpha
    }
  }
  memcpy(pixels,blured,sizeof(Color));
  free(blured);

}

Color *sobel(Color *pixels, size_t img_w, size_t img_h)
{
  Color *y = calloc(img_w * img_h, sizeof(Color));

   float gx[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};
   float gy[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};

  for (size_t i = 0; i < img_h; ++i)
  {
    for (size_t j = 0; j < img_w; ++j)
    {
      float xgrad = 0;
      float ygrad = 0;
      // loop through the nine neighbouring pixels
      for (int k = -1; k <= 1; ++k)
      {
        for (int l = -1; l <= 1; ++l)
        {
          if (i + k < 0 || i + k >= img_h)
            continue;
          if (j + l < 0 || j + l >= img_w)
            continue;
          xgrad += (pixels[img_w * (i + k) + (j + l)].r) * gx[1 + k][1 + l];
          ygrad += (pixels[img_w * (i + k) + (j + l)].r) * gy[1 + k][1 + l];
        }
      }
      double mag = sqrt(pow(xgrad,2) + pow(ygrad,2));
      y[img_w * i + j].r = (unsigned char)fmin(fmax(mag, 0), 255);
      y[img_w * i + j].g = y[img_w * i + j].r;
      y[img_w * i + j].b = y[img_w * i + j].r;
      y[img_w * i + j].a = pixels[img_w * i + j].a; // Preserve the original alpha
    }
  }
  return y;
}

int main(int argc, char **argv)
{
  InitWindow(WINDOW_W, WINDOW_H, "Sobel Operator");
  SetTargetFPS(30);

  if(argc < 2){
    printf("Please provide an image\n");

    return 1;
  }

  char *image_path = argv[1];

  Image image = LoadImage(image_path);

  Color *pixels = LoadImageColors(image);
  Texture2D input_texture = LoadTextureFromImage(image);

  size_t img_w = input_texture.width;
  size_t img_h = input_texture.height;

  Color *gray_pixels = toGrayScale(pixels, img_w, img_h);
  gaussianBlur(gray_pixels,img_w,img_h);
  Color *edge_data = sobel(gray_pixels, img_w, img_h);
  // Create an Image from the Color array
  Image edge_image = {
      .data = edge_data,                          // Pointer to pixel data (array of Color)
      .width = img_w,                          // Image width
      .height = img_h,                         // Image height
      .mipmaps = 1,                               // Number of mipmaps (1 for no mipmaps)
      .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 // Pixel format (32-bit RGBA)
  };

  Texture2D texture = LoadTextureFromImage(edge_image);

  while (!WindowShouldClose())
  {
    BeginDrawing();
    // Draw the texture
    DrawTexture(texture, WINDOW_W / 2 - texture.width / 2, WINDOW_H / 2 - texture.height / 2, WHITE);
    EndDrawing();
  }

  UnloadImage(image);
  UnloadImage(edge_image);
  UnloadImageColors(pixels);
  UnloadTexture(input_texture);
  UnloadTexture(texture);
  CloseWindow();

  return 0;
}
