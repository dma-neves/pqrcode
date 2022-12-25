#ifndef QRCODE_READER_HPP
#define QRCODE_READER_HPP

#include <iostream>

#include "image/Image.hpp"
#include "processing/ImageProcessor.hpp"

class QRCodeReader
{
public:
    static std::string read(Image* img);
};

#endif