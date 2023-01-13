#ifndef IMAGE_PROCESSOR_HPP
#define IMAGE_PROCESSOR_HPP

#include <array>

#include  "../image/Image.hpp"
#include "../utils/Vector2D.hpp"

class ImageProcessor
{
public:

    static void negative(Image& img);
    static void convertToBW(Image& img, pcomp threshold);
    static pcomp getOtsuTheshold(Image& img);
    static std::array<int, N_COMP_VALS> histogramGray(Image& img);
    static void rotationAroundPoint(Image& img, Image& copy, double angle, Vector2D center);
    static void shear(Image& img, Image& copy, double x_shear, double y_shear);
};

#endif