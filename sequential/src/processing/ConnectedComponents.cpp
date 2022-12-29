#include "ConnectedComponents.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <math.h>

#include "../utils/Utils.hpp"

void ConnectedComponents::printLabels(Labels* labels)
{
    /*
    * DEBUG
    */

    for (int y = 0; y < labels->height; y++)
    {
        for (int x = 0; x < labels->width; x++)
        {
            std::string labelStr = std::to_string(labels->labels[y][x]);

            if (labels->labels[y][x] == -1)
                labelStr = " ";

            int nwp = (4 - labelStr.length());

            for (int i = 0; i < nwp; i++)
                std::cout << " ";

           std::cout << labelStr << " ";

        }
        std::cout << std::endl;
    }

}

Labels* ConnectedComponents::getLabels(Image* img)
{
    int x, y;
    int width = img->width;
    int height = img->height;

    int label = 1, neighbourLabel;
    Labels* labels = new Labels(width, height);

    std::vector<std::vector<int>> adjacencies;

    // TODO: don't need to set initial labels -> improve performance

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if(img->pixels[y][x].r == 0)
                labels->labels[y][x] = label++;
            else
                labels->labels[y][x] = -1;
        }
    }

    // Update labels and save adjacencies

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            int initialLabelValue = labels->labels[y][x];
            if (initialLabelValue != -1)
            {
                std::vector<int> candidates;
                for (int yoffset = (y == 0 ? 0 : -1); yoffset <= (y == height - 1 ? 0 : 1); yoffset++)
                {
                    for (int xoffset = (x == 0 ? 0 : -1); xoffset <= (x == width - 1 ? 0 : 1); xoffset++)
                    {
                        neighbourLabel = labels->labels[y + yoffset][x + xoffset];

                        if(neighbourLabel != -1)
                        {
                            if (neighbourLabel < initialLabelValue && !Utils::contains<int>(&candidates, neighbourLabel))
                                candidates.push_back(neighbourLabel);

                            if (neighbourLabel < labels->labels[y][x])
                                labels->labels[y][x] = neighbourLabel;
                        }
                    }
                }

                if (candidates.size() > 1)
                    adjacencies.push_back(candidates);
            }
        }
    }

    // Compute the transitive closure of the adjacencies

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

                // if(Utils::intersection<int>(adj_a, adj_b).size() != 0)
                // {
                //     adjacencies[i] = Utils::union_<int>(adj_a, adj_b);
                //     adj_b->clear();
                // }

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

    // Save replacements found in the transitive closure in a map

    std::map<int, int> replacements;
    for (int i = 0; i < transitiveClosure.size(); i++)
    {
        auto it_min = std::min_element(transitiveClosure[i].begin(), transitiveClosure[i].end());
        int min = *it_min;
        transitiveClosure[i].erase(it_min);
        for(int k = 0; k < transitiveClosure[i].size(); k++)
        {
            label = transitiveClosure[i][k];
            if (replacements.count(label))
                ;// throw "repeated key while applying transitive closure";
            else
                replacements.insert({label, min});
        }
    }

    // Apply the replacements using the map

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            label = labels->labels[y][x];
            if (label != -1 && replacements.count(label))
                labels->labels[y][x] = replacements[label];
        }
    }

    return labels;
}

std::map<int, BoundingBox> ConnectedComponents::getBoundingBoxes(Labels* labels)
{
    int height = labels->height;
    int width = labels->width;
    std::map<int, BoundingBox> boundingBoxes;

    for (int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            int label = labels->labels[y][x];
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