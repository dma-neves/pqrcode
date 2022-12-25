#ifndef CONNECTED_COMPONENTS_HPP
#define CONNECTED_COMPONENTS_PPP

#include <map>

#include "image/Image.hpp"

struct BoundingBox
{
public:
    int left;
    int right;
    int top;
    int bottom;

    int center_x;
    int center_y;
};

struct Labels
{
public:
    int** labels;
    int width;
    int height;

    Labels(int width, int height) : width(width), height(height)
    {
        labels = new int*[height];
        for(int y = 0; y < height; y++)
            labels[y] = new int[width];
    }

    ~Labels()
    {
        for(int y = 0; y < height; y++)
            delete labels[y];

        delete labels;
    }
};


class ConnectedComponents
{
public:
    static Labels getLabels(Image* img);
    static std::map<int, BoundingBox> getBoundingBoxes(Labels labels);
};

#endif