#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cstring>

#define COMP_MAX_VAL 255
#define N_COMP_VALS 256

typedef unsigned char pcomp; // Pixel RGB component data type

struct Pixel
{
public:
    pcomp r,g,b;
};

class Image
{
public:
    int width;
    int height;
    Pixel** pixels;

    Image(int width, int height) : width(width), height(height)
    {
        pixels = new Pixel*[height];

        for(int y = 0; y < height; y++)
            pixels[y] = new Pixel[width];
    }

    ~Image()
    {
        for(int y = 0; y < height; y++)
            delete pixels[y];

        delete pixels;
    }

    Image* copy()
    {
        Image* imgCopy = new Image(width, height);
        //std::memcpy(imgCopy->pixels, pixels, sizeof(Pixel)*width*height);

        for(int y = 0; y < height; y++)
            for(int x = 0; x < width; x++)
            {
                imgCopy->pixels[y][x].r = pixels[y][x].r;
                imgCopy->pixels[y][x].g = pixels[y][x].g;
                imgCopy->pixels[y][x].b = pixels[y][x].b;
            }

        return imgCopy;
    }

    void copyPixels(Image* img)
    {
        for(int y = 0; y < height; y++)
            for(int x = 0; x < width; x++)
            {
                pixels[y][x].r = img->pixels[y][x].r;
                pixels[y][x].g = img->pixels[y][x].g;
                pixels[y][x].b = img->pixels[y][x].b;
            }
    }
};

#endif