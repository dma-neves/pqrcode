#ifndef IMAGE_HPP
#define IMAGE_HPP

struct Pixel
{
public:
    unsigned char r,g,b;
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
};

#endif