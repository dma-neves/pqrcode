#include "ConnectedComponents.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <math.h>
#include <chrono>
#include <omp.h>

#include "../utils/Utils.hpp"

void ConnectedComponents::printLabels(Labels& labels)
{
    /*
    * DEBUG
    */

    for (int y = 0; y < labels.height; y++)
    {
        for (int x = 0; x < labels.width; x++)
        {
            std::string labelStr = std::to_string(labels.labels[y][x]);

            if (labels.labels[y][x] == -1)
                labelStr = " ";

            int nwp = (4 - labelStr.length());

            for (int i = 0; i < nwp; i++)
                std::cout << " ";

           std::cout << labelStr << " ";

        }
        std::cout << std::endl;
    }

}

std::vector<std::vector<int>> getTransitiveClosure(std::vector<std::vector<int>> adjacencies)
{
    for (int i = 0; i < adjacencies.size(); i++)
    {
        auto adj_a = &adjacencies[i];
        if (adj_a->size() == 0) continue;

        for (int j = 0; j < adjacencies.size(); j++)
        {
            if (i != j)
            {
                auto adj_b = &adjacencies[j];
                if (adj_b->size() == 0) continue;

                auto uni = Utils::union_<int>(adj_a, adj_b);
                if(uni.size() < adj_a->size() + adj_b->size())
                {
                    adjacencies[i] = uni;
                    adj_b->clear();
                }
            }
        }
    }

    std::vector<std::vector<int>> transitiveClosure;
    for (int i = 0; i < adjacencies.size(); i++)
        if (adjacencies[i].size() != 0)
            transitiveClosure.push_back(adjacencies[i]);

    return transitiveClosure;
}

void applyAdjacencies(Labels& labels, std::vector<std::vector<int>> adjacencies)
{
    int label;

    // Save replacements found in the transitive closure in a map

    std::map<int, int> replacements;
    for (int i = 0; i < adjacencies.size(); i++)
    {
        auto it_min = std::min_element(adjacencies[i].begin(), adjacencies[i].end());
        int min = *it_min;
        adjacencies[i].erase(it_min);
        for(int k = 0; k < adjacencies[i].size(); k++)
        {
            label = adjacencies[i][k];
            if (replacements.count(label))
                ;// throw "repeated key while applying transitive closure";
            else
                replacements.insert({label, min});
        }
    }

    // Apply the replacements using the map

    for (int y = 0; y < labels.height; y++)
    {
        for (int x = 0; x < labels.width; x++)
        {
            label = labels.labels[y][x];
            if (label != -1 && replacements.count(label))
                labels.labels[y][x] = replacements[label];
        }
    }
}

void computeLabels(Image& img, Labels& labels, int firstLabel)
{
    int width = img.width;
    int height = img.height;

    int label = firstLabel, neighbourLabel;

    std::vector<std::vector<int>> adjacencies;

    // TODO: don't need to set initial labels -> improve performance

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if(img.pixels[y][x].r == 0)
                labels.labels[y][x] = label++;
            else
                labels.labels[y][x] = -1;
        }
    }

    // Update labels and save adjacencies

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int initialLabelValue = labels.labels[y][x];
            if (initialLabelValue != -1)
            {
                std::vector<int> candidates;
                for (int yoffset = (y == 0 ? 0 : -1); yoffset <= (y == height - 1 ? 0 : 1); yoffset++)
                {
                    for (int xoffset = (x == 0 ? 0 : -1); xoffset <= (x == width - 1 ? 0 : 1); xoffset++)
                    {
                        neighbourLabel = labels.labels[y + yoffset][x + xoffset];

                        if(neighbourLabel != -1)
                        {
                            if (neighbourLabel < initialLabelValue && !Utils::contains<int>(&candidates, neighbourLabel))
                                candidates.push_back(neighbourLabel);

                            if (neighbourLabel < labels.labels[y][x])
                                labels.labels[y][x] = neighbourLabel;
                        }
                    }
                }

                if (candidates.size() > 1)
                    adjacencies.push_back(candidates);
            }
        }
    }

    // Compute the transitive closure of the adjacencies
    std::vector<std::vector<int>> transitiveClosure = getTransitiveClosure(adjacencies);

    // Apply the adjacencies using the trasitive closure
    applyAdjacencies(labels, transitiveClosure);
}

Labels ConnectedComponents::getLabels(Image& img)
{
    int firstLabel = 1;
    Labels labels(img.width, img.height);  
    computeLabels(img, labels, firstLabel);
    return labels;
}

std::map<int, BoundingBox> ConnectedComponents::getBoundingBoxes(Labels& labels)
{
    int height = labels.height;
    int width = labels.width;
    std::map<int, BoundingBox> boundingBoxes;

    for (int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            int label = labels.labels[y][x];
            if (label != -1)
            {
                BoundingBox bbox;

                if(boundingBoxes.find(label) == boundingBoxes.end())
                {
                    bbox = (BoundingBox) { .left = x, .right = x, .top = y, .bottom = y, .center_x = -1, .center_y = -1 };
                    boundingBoxes.insert( {label, bbox} );
                }
                else
                {
                    bbox = boundingBoxes[label];

                    if (x < bbox.left) bbox.left = x;
                    if (x > bbox.right) bbox.right = x;
                    if (y < bbox.top) bbox.top = y;
                    if (y > bbox.bottom) bbox.bottom = y;

                    boundingBoxes[label] = bbox;
                }
            }
        }
    }

    std::map<int, BoundingBox> boundingBoxesWithCenter;

    for(auto entry : boundingBoxes)
    {
        BoundingBox bbox = entry.second;
        bbox.center_x = (int)round(bbox.left + (bbox.right - bbox.left) / 2.0);
        bbox.center_y = (int)round(bbox.top + (bbox.bottom - bbox.top) / 2.0);

        boundingBoxesWithCenter.insert( {entry.first, bbox} );
    }

    return boundingBoxesWithCenter;
}

Labels ConnectedComponents::getLabelsParallel(Image& img)
{
    Labels labels(img.width, img.height);

    int nthreads = omp_get_max_threads();
    int partSize = img.height / nthreads;

   #pragma omp parallel
   {
        int t = omp_get_thread_num();

        /*
            Partition labels and image
        */

        Image imgPartition;
        imgPartition.pixels = &img.pixels[t*partSize];
        imgPartition.width = img.width;
        imgPartition.height = t == nthreads-1 ? 
            img.height - (partSize * (nthreads-1)) - 1 :
            partSize;

        Labels labelsPartition;
        labelsPartition.labels = &labels.labels[t*partSize];
        labelsPartition.width = labels.width;
        labelsPartition.height = t == nthreads-1 ? 
            labels.height - (partSize * (nthreads-1)) - 1 :
            partSize;


        /*
            Run connected components on each partition
        */

        int firstLabel = t*partSize + 1;
        computeLabels(imgPartition, labelsPartition, firstLabel);

        // set fields to zero and nullptr, so that the destructors
        // don't delete the data from the main image, and labels
        imgPartition.pixels = nullptr;
        imgPartition.height = 0;
        imgPartition.width = 0;
        labelsPartition.labels = nullptr;
        labelsPartition.width = 0;
        labelsPartition.height = 0;
   }

    /*
        Join partitioned labels
    */

    // Find adjacencies between label partitions

    int prevLabel = -1;
    int prevNeighbourLabel = -1;

    std::vector<std::vector<int>> interPartAdjacencies;
    for(int y = partSize-1; y < img.height-partSize; y += partSize)
    {
        for(int x = 0; x < img.width; x++)
        {
            for(int xoffset = (x == 0 ? 0 : -1); xoffset <= (x == img.width-1 ? 0 : 1); xoffset++)
            {
                int neighbour_x = x+xoffset;
                int neighbour_y = y+1;

                int label = labels.labels[y][x];
                int neighbourLabel = labels.labels[neighbour_y][neighbour_x];

                if(label != -1 && neighbourLabel != -1 && label != prevLabel && neighbourLabel != prevNeighbourLabel)
                {
                    std::vector<int> adj;
                    adj.push_back(label);
                    adj.push_back(neighbourLabel);
                    interPartAdjacencies.push_back(adj);

                    prevLabel = label;
                    prevNeighbourLabel = neighbourLabel;
                }
            }
        }
    }

    // Compute transitive closure of interPartAdjacencies TODO: necessay?
    std::vector<std::vector<int>> transitiveClosure = getTransitiveClosure(interPartAdjacencies);

    // Apply the adjacencies using the trasitive closure
    applyAdjacencies(labels, transitiveClosure);

    return labels;
}