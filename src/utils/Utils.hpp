#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <algorithm>

class Utils
{
public:
    static bool contains(std::list<int> list, int val)
    {
        return (std::find(list.begin(), list.end(),val) != list.end());
    }
};

#endif