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

    Image()
    {
        width = 0;
        height = 0;
        pixels = nullptr;
    };

    Image(int width, int height) : width(width), height(height)
    {
        allocate_pixels(width, height);
    }

    Image(Image&& image)
    {
        pixels = image.pixels;
        width = image.width;
        height = image.height;

        image.width = 0;
        image.height = 0;
        image.pixels = nullptr;
    }

    Image(Image& image)
    {
        allocate_pixels(image.width, image.height);
        copy_pixels(image);
    }

    Image& operator=(Image&& image)
    {
        destroy_pixels();
        pixels = image.pixels;
        width = image.width;
        height = image.height;

        image.width = 0;
        image.height = 0;
        image.pixels = nullptr;

        return *this;
    }

    Image& operator=(Image& image)
    {
        destroy_pixels();
        allocate_pixels(image.width, image.height);
        copy_pixels(image);

        return *this;
    }

    ~Image()
    {
        destroy_pixels();
    }

private:

    void copy_pixels(Image& image)
    {
        for(int y = 0; y < height; y++)
            for(int x = 0; x < width; x++)
                pixels[y][x] = image.pixels[y][x];
    }

    void destroy_pixels()
    {
        for(int y = 0; y < height; y++)
            delete pixels[y];

        delete pixels;
    }

    void allocate_pixels(int width, int height)
    {
        this->width = width;
        this->height = height;

        pixels = new Pixel*[height];

        for(int y = 0; y < height; y++)
            pixels[y] = new Pixel[width];
    }
};

#endif