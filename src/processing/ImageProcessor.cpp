#include "ImageProcessor.hpp"

#include <math.h>

void ImageProcessor::negative(Image* img)
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

void ImageProcessor::convertToBW(Image *img, pcomp threshold)
{
    for(int y = 0; y < img->height; y++)
    {
        for(int x = 0; x < img->width; x++)
        {
            pcomp gray = round( (img->pixels[y][x].r + img->pixels[y][x].g + img->pixels[y][x].b) / 3.0 );

            if(gray <= threshold)
            {
                img->pixels[y][x].r = 0;
                img->pixels[y][x].g = 0;
                img->pixels[y][x].b = 0;
            }
            else
            {
                img->pixels[y][x].r = 255;
                img->pixels[y][x].g = 255;
                img->pixels[y][x].b = 255;
            }
        }
    }
}

pcomp ImageProcessor::getOtsuTheshold(Image *img)
{
    std::array<int, N_COMP_VALS> hist = histogramGray(img);
    int total_sum = hist.size();

    int best_t = 0;
    double best_var = 0;
    for (int t = 0; t < N_COMP_VALS; t++)
    {
        double q1 = 0;
        for (int i = 0; i <= t; i++)
            q1 += hist[i];
        q1 /= total_sum;

        double q2 = 0;
        for (int i = t + 1; i < N_COMP_VALS; i++)
            q2 += hist[i];
        q2 /= total_sum;

        double u1 = 0;
        for (int i = 0; i <= t; i++)
            u1 += i * hist[i];
        u1 /= total_sum;
        u1 = q1 == 0 ? 0 : u1 / q1;

        double u2 = 0;
        for (int i = t + 1; i < N_COMP_VALS; i++)
            u2 += i * hist[i];
        u2 /= total_sum;
        u2 = q2 == 0 ? 0 : u2 / q2;


        double var = q1 * q2 * (u1 - u2) * (u1 - u2);
        if (t == 0 || var > best_var)
        {
            best_t = t;
            best_var = var;
        }
    }

    return best_var;
}

std::array<int, N_COMP_VALS> ImageProcessor::histogramGray(Image* img)
{
    std::array<int, N_COMP_VALS> hist;
    hist.fill(0);
    
    for(int y = 0; y < img->height; y++)
    {
        for(int x = 0; x < img->width; x++)
        {
            pcomp val = round( (img->pixels[y][x].r + img->pixels[y][x].g + img->pixels[y][x].b) / 3.0 );
            hist[val]++;
        }
    }

    return hist;
}