#include <iostream>
#include <chrono>

#include "image/PNGHandler.hpp"
#include "processing/ImageProcessor.hpp"
#include "processing/ConnectedComponents.hpp"
#include "QRCodeReader.hpp"

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
        Image img = im->readImage(argv[1]);

        #ifdef TIME
            auto start = std::chrono::high_resolution_clock::now();
            QRCodeReader::read(img);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            std::cout << duration.count() << std::endl;
        #else
            std::cout << QRCodeReader::read(img) << std::endl;
        #endif

        im->writeImage(img, argv[2]);
    }
    catch(std::string msg)
    {
        std::cerr << "Exception: " << msg << std::endl;
    }
    catch(char const* msg)
    {
        std::cerr << "Exception: " << msg << std::endl;
    }

    delete im;

    return 0;
}