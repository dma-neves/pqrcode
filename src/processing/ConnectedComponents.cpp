#include "ConnectedComponents.hpp"

#include <list>

#include "../utils/Utils.hpp"

Labels ConnectedComponents::getLabels(Image* img)
{
    int x, y;
    int width = img->width;
    int height = img->height;

    int label = 1, neighbourLabel;
    Labels labels(width, height);

    std::list<std::list<int>> adjacencies;

    // TODO: don't need to set initial labels -> improve performance

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if(img->pixels[y][x].r == 0)
                labels.labels[y][x] = label++;
            else
                labels.labels[y][x] = -1;
        }
    }

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            int initialLabelValue = labels.labels[y][x];
            if (initialLabelValue != -1)
            {
                std::list<int> candidates;
                for (int yoffset = (y == 0 ? 0 : -1); yoffset <= (y == height - 1 ? 0 : 1); yoffset++)
                {
                    for (int xoffset = (x == 0 ? 0 : -1); xoffset <= (x == width - 1 ? 0 : 1); xoffset++)
                    {
                        neighbourLabel = labels.labels[y + yoffset][x + xoffset];

                        if(neighbourLabel != -1)
                        {
                            if (neighbourLabel < initialLabelValue && !Utils::contains(candidates, neighbourLabel))
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

    // Compute transitive closure

    std::list<std::list<int>>::iterator it;

    for(it = adjacencies.begin(); it != adjacencies.end(); ++it)
    {
        auto adj_a = *it;
        if (adj_a.size() == 0) continue;

        std::list<std::list<int>>::iterator inner_it;
        for(inner_it = it.; inner_it != adjacencies.end(); ++inner_it)
        {

        }
    }

    for (int i = 0; i < adjacencies.size(); i++)
    {
        std::list<int> adj_a = adjacencies.get(i);
        if (adj_a.size() == 0) continue;

        for (int j = 0; j < adjacencies.size(); j++)
        {
            if (i != j)
            {
                var adj_b = adjacencies[j];
                if (adj_b.size() == 0) continue;

                if (adj_a.Intersect(adj_b).Any())
                {
                    adj_a = adj_a.Union(adj_b).ToList();
                    adj_b = new List<int>();
                    adjacencies[j] = adj_b;
                }
            }
        }

        adjacencies[i] = adj_a;
    }

    List<List<int>> transitiveClosure = new List<List<int>>();
    for (int i = 0; i < adjacencies.size(); i++)
        if (adjacencies[i].size() != 0)
            transitiveClosure.Add(adjacencies[i]);

    // Save replacements found in the transitive closure in a dictionary

    Dictionary<int, int> replacements = new Dictionary<int, int>();
    for (int i = 0; i < transitiveClosure.size(); i++)
    {
        int min = transitiveClosure[i].Min();
        transitiveClosure[i].Remove(min);
        for(int k = 0; k < transitiveClosure[i].size(); k++)
        {
            label = transitiveClosure[i][k];
            if (replacements.ContainsKey(label))
                ;// throw new Exception("Repeated key while applying transitive closure: " + label);
            else
                replacements.Add(label, min);
        }
    }

    // Apply replacements using the dictionary

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            label = labels.labels[y][x];
            if (label != -1 && replacements.ContainsKey(label))
                labels.labels[y][x] = replacements[label];
        }
    }

    watch.Stop();
    //Console.WriteLine($"Execution Time: {watch.ElapsedMilliseconds} ms");

    return labels;
}

std::map<int, BoundingBox> ConnectedComponents::getBoundingBoxes(Labels labels)
{

}