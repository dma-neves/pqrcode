#ifndef QRCODE_READER_HPP
#define QRCODE_READER_HPP

#include <iostream>
#include <string>

#include "image/Image.hpp"
#include "processing/ImageProcessor.hpp"

#ifndef COMPONENT_CENTER_DIST_MARGIN
#define COMPONENT_CENTER_DIST_MARGIN 2.5
#endif

#define POSITIONING_BLOCKS_DIST_MARGIN 2.0

#define MORE_THAN_THREE_POSITIONING_BLOCKS_EXCPETION "found more than 3 positioning blocks"
#define LESS_THAN_THREE_POSITIONING_BLOCKS_EXCPETION "found less than 3 positioning blocks"
#define MAX_RECURSION_DEPTH_EXCEPTION "reached maximum recursion depth"

class QRCodeReader
{
public:
    static std::string read(Image& img);
};

#endif