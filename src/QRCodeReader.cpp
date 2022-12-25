#include "QRCodeReader.hpp"

std::string QRCodeReader::read(Image* img)
{
    Image* copy = img->copy();
    Image* auxImg = img->copy();

    ImageProcessor::convertToBW(auxImg, std::min<pcomp>(100, ImageProcessor::getOtsuTheshold(auxImg)));
    
}