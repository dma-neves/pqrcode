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
    int** labels;
    int width;
    int height;

    Labels()
    {
        width = 0;
        height = 0;
        labels = nullptr;
    };

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

    Labels(Labels&& labels)
    {
        destroy_labels();
        this->labels = labels.labels;
        width = labels.width;
        height = labels.height;

        labels.width = 0;
        labels.height = 0;
        labels.labels = nullptr;
    }

    Labels(Labels& labels)
    {
        destroy_labels();
        allocate_labels(labels.width, labels.height);
        copy_labels(labels);
    }

    Labels& operator=(Labels& labels)
    {
        destroy_labels();
        this->labels = labels.labels;
        width = labels.width;
        height = labels.height;

        labels.width = 0;
        labels.height = 0;
        labels.labels = nullptr;

        return *this;
    }

    Labels& operator=(Labels&& labels)
    {
        destroy_labels();
        allocate_labels(labels.width, labels.height);
        copy_labels(labels);

        return *this;
    }

private:

    void copy_labels(Labels& labels)
    {
        for(int y = 0; y < height; y++)
            for(int x = 0; x < width; x++)
                this->labels[y][x] = labels.labels[y][x];
    }

    void destroy_labels()
    {
        for(int y = 0; y < height; y++)
            delete labels[y];

        delete labels;
    }

    void allocate_labels(int width, int height)
    {
        this->width = width;
        this->height = height;

        labels = new int*[height];

        for(int y = 0; y < height; y++)
            labels[y] = new int[width];
    }
};


class ConnectedComponents
{
public:
    static void printLabels(Labels& labels);
    static Labels getLabels(Image& img);
    static Labels getLabelsParallel(Image& img);
    static std::map<int, BoundingBox> getBoundingBoxes(Labels& labels);
};

#endif