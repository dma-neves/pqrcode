#ifndef IMAGE_HANDLER_HPP
#define IMAGE_HANDLER_HPP

#include "Image.hpp"

#define UNEXPECTED_FILE_TYPE_EXCEPTION "unexpected file type"
#define UNABLE_TO_OPEN_FILE_EXCEPTION "unable to open file"
#define FAILED_TO_READ_FILE_EXCEPTION "failed to read file"
#define INVALID_COLOR_FORMAT_EXCEPTION "invalid color format"
#define FAILED_TO_WRITE_FILE_EXCEPTION "failed to write file"

class ImageHandler
{
public:
    virtual Image readImage(char* file) = 0;
    virtual void writeImage(Image& image, char* file) = 0;
};

#endif