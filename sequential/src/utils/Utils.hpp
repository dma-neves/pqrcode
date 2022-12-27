#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <map>
#include <algorithm>

class Utils
{
public:
    template <typename T>
    static bool contains(std::vector<T>* vec, T val)
    {
        // OMP ?? this function is called many times
        return (std::find(vec->begin(), vec->end(), val) != vec->end());
    }

    template <typename T>
    static std::vector<T> intersection(std::vector<T>* v1, std::vector<T>* v2)
    {
        std::vector<T> intersect;

        std::sort(v1->begin(), v1->end());
        std::sort(v2->begin(), v2->end());

        std::set_intersection(v1->begin(),v1->end(), v2->begin(),v2->end(), back_inserter(intersect));

        return intersect;
    }

    template <typename T>
    static std::vector<T> union_(std::vector<T>* v1, std::vector<T>* v2)
    {
        // OMP ?? this function is called many times
        std::vector<T> uni;

        std::sort(v1->begin(), v1->end());
        std::sort(v2->begin(), v2->end());

        auto it = std::set_union(v1->begin(),v1->end(), v2->begin(),v2->end(), std::back_inserter(uni));
        return uni;
    }

    template <typename T, typename U>
    static std::vector<U> getMapValues(std::map<T,U> m)
    {
        std::vector<U> v;
        for(auto entry : m) v.push_back(entry.second);

        return v;
    }

    template <typename T, int N>
    static int sumArrayValues(std::array<T, N> arr)
    {
        int sum = 0;
        for(T elem : arr)
            sum += elem;

        return sum;
    }
};

#endif