#ifndef CONNECTED_COMPONENTS_HPP
#define CONNECTED_COMPONENTS_PPP

#include <map>

#include "../image/Image.hpp"

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

class Labels
{
public:
    int** __restrict__ labels;
    int width;
    int height;

    Labels()
    {}

    Labels(int width, int height) : width(width), height(height)
    {
        labels = new int*[height];
        for(int y = 0; y < height; y++)
            labels[y] = new int[width];
    }

    void destroy()
    {
        for(int y = 0; y < height; y++)
            delete labels[y];

        delete labels;
    }

    ~Labels()
    {
        // for(int y = 0; y < height; y++)
        //     delete labels[y];

        // delete labels;
    }
};


class ConnectedComponents
{
public:
    static void printLabels(Labels* labels);
    static Labels* getLabels(Image* img);
    static Labels* getLabelsParallel(Image* img);
    static std::map<int, BoundingBox> getBoundingBoxes(Labels* labels);
};

#endif