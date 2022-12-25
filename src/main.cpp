#include <iostream>

#include "image/PNGHandler.hpp"
#include "processing/ImageProcessor.hpp"

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
        //ImageProcessor::negative(img);
        ImageProcessor::convertToBW(img, ImageProcessor::getOtsuTheshold(img));
        im->writeImage(img, argv[2]);
        delete img;
    }
    catch(std::string msg)
    {
        std::cerr << "Exception: " << msg << std::endl;
    }

    delete im;

    return 0;
}