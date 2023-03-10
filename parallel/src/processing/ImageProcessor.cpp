#include "ImageProcessor.hpp"
#include "../utils/Utils.hpp"

#include <math.h>
#include <omp.h>

void ImageProcessor::negative(Image& img)
{
    #pragma omp parallel for
    for(int y = 0; y < img.height; y++)
    {
        for(int x = 0; x < img.width; x++)
        {
            img.pixels[y][x].r = 255 - img.pixels[y][x].r;
            img.pixels[y][x].g = 255 - img.pixels[y][x].g;
            img.pixels[y][x].b = 255 - img.pixels[y][x].b;
        }
    }
}

void ImageProcessor::convertToBW(Image&img, pcomp threshold)
{
    #pragma omp parallel for
    for(int y = 0; y < img.height; y++)
    {
        for(int x = 0; x < img.width; x++)
        {
            pcomp gray = round( (img.pixels[y][x].r + img.pixels[y][x].g + img.pixels[y][x].b) / 3.0 );

            if(gray <= threshold)
            {
                img.pixels[y][x].r = 0;
                img.pixels[y][x].g = 0;
                img.pixels[y][x].b = 0;
            }
            else
            {
                img.pixels[y][x].r = 255;
                img.pixels[y][x].g = 255;
                img.pixels[y][x].b = 255;
            }
        }
    }
}

pcomp ImageProcessor::getOtsuTheshold(Image&img)
{
    std::array<int, N_COMP_VALS> hist = histogramGray(img);
    int total_sum = Utils::sumArrayValues<int, N_COMP_VALS>(hist);

    int best_t = 0;
    double best_var = 0;

    #pragma omp parallel
    {
        double best_var_priv = 0;
        int best_t_priv = 0;

        #pragma omp for
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
            if (t == 0 || var > best_var_priv)
            {
                best_t_priv = t;
                best_var_priv = var;
            }
        }

        #pragma omp critical
        {
            if(best_var_priv > best_var)
            {
                best_var = best_var_priv;
                best_t = best_t_priv;
            }
        }
    }


    return best_var;
}

std::array<int, N_COMP_VALS> ImageProcessor::histogramGray(Image& img)
{
    std::array<int, N_COMP_VALS> hist;
    hist.fill(0);

    #pragma omp parallel
    {
        std::array<int, N_COMP_VALS> hist_priv;
        hist_priv.fill(0);

        #pragma omp for
        for(int y = 0; y < img.height; y++)
        {
            for(int x = 0; x < img.width; x++)
            {
                pcomp val = round( (img.pixels[y][x].r + img.pixels[y][x].g + img.pixels[y][x].b) / 3.0 );
                hist_priv[val]++;
            }
        }

        #pragma omp critical
        {
            for(int i = 0; i < N_COMP_VALS; i++)
                hist[i] += hist_priv[i];
        }
    }

    return hist;
}

void ImageProcessor::rotationAroundPoint(Image& img, Image& copy, double angle, Vector2D center)
{
    int origin_x, origin_y;
    double cos_theta = cos(angle);
    double sin_theta = sin(angle);

    #pragma omp parallel for private(origin_x,origin_y)
    for(int y = 0; y < img.height; y++)
    {
        for(int x = 0; x < img.width; x++)
        {
            origin_x = (int)round((x - center.x) * cos_theta - (center.y - y) * sin_theta + center.x);
            origin_y = (int)round(center.y - (x - center.x) * sin_theta - (center.y - y) * cos_theta);

            if (origin_x >= 0 && origin_x < img.width && origin_y >= 0 && origin_y < img.height)
            {
                img.pixels[y][x].r = copy.pixels[origin_y][origin_x].r;
                img.pixels[y][x].g = copy.pixels[origin_y][origin_x].g;
                img.pixels[y][x].b = copy.pixels[origin_y][origin_x].b;
            }
            else
            {
                img.pixels[y][x].r = 0;
                img.pixels[y][x].g = 0;
                img.pixels[y][x].b = 0;
            }
        }
    }
}

void ImageProcessor::shear(Image& img, Image& copy, double x_shear, double y_shear)
{
    int origin_x, origin_y;

    #pragma omp parallel for private(origin_x,origin_y)
    for(int y = 0; y < img.height; y++)
    {
        for(int x = 0; x < img.width; x++)
        {
            origin_x = (int)round(x - x_shear*y);
            origin_y = (int)round(y - y_shear*x);

            if (origin_x >= 0 && origin_x < img.width && origin_y >= 0 && origin_y < img.height)
            {
                img.pixels[y][x].r = copy.pixels[origin_y][origin_x].r;
                img.pixels[y][x].g = copy.pixels[origin_y][origin_x].g;
                img.pixels[y][x].b = copy.pixels[origin_y][origin_x].b;

            }
            else
            {
                img.pixels[y][x].r = 0;
                img.pixels[y][x].g = 0;
                img.pixels[y][x].b = 0;
            }
        }
    }
}