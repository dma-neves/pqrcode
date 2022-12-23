#include <iostream>

#include "image/PNGHandler.hpp"

void processImage(Image* img)
{
    for(int y = 0; y < img->height; y++)
    {
        for(int x = 0; x < img->width; x++)
        {
            img->pixels[y][x].r = 255 - img->pixels[y][x].r;
            img->pixels[y][x].g = 255 - img->pixels[y][x].g;
            img->pixels[y][x].b = 255 - img->pixels[y][x].b;
        }
    }
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        std::cout << "usage: pqrcode <file_in> <file_out>" << std::endl;
        return 0;
    }

    ImageHandler* im = new PNGHandler();

    try
    {
        Image* img = im->readImage(argv[1]);
        processImage(img);
        im->writeImage(img, argv[2]);
        delete img;
    }
    catch(std::string msg)
    {
        std::cerr << "Exception " << msg << std::endl;
    }

    delete im;

    return 0;
}