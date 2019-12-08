//
// Created by quan on 03/12/2019.
//

#ifndef OOP_PROJECT01_ARRAY2D_H
#define OOP_PROJECT01_ARRAY2D_H

#include <cassert>
#include <iostream>
template<typename T>
class array2d {
    unsigned int size_x, size_y;
public:
    T* arr;

    array2d() : arr(nullptr), size_x(0), size_y(0) {}
    array2d(unsigned int size_x, unsigned int size_y) : size_x(size_x), size_y(size_y) {
        arr = new T[size_x*size_y];
    }

    static array2d create8rounded(unsigned int size_x, unsigned int size_y) {
        return array2d(ceil(size_x/8.0)*8, ceil(size_y/8.0)*8);
    }

    T& _get(unsigned int index_x, unsigned int index_y) {
        return arr[index_x + size_x*index_y];
    }
    T& operator()(unsigned int index_x, unsigned int index_y) const {
        assert(index_x < size_x);
        assert(index_y < size_y);
        return arr[index_x + size_x*index_y];
    }
    T try_get(unsigned int index_x, unsigned int index_y) {
        if(index_x >= size_x)
            index_x = size_x-1;
        if(index_y >= size_y)
            index_y = size_y-1;
        return arr[index_x + size_x*index_y];
    }
    void try_set(unsigned int index_x, unsigned int index_y, T value) {
        if(index_x < size_x && index_y < size_y)
            arr[index_x + size_x*index_y] = value;
    }

    [[nodiscard]] unsigned int get_size_x() const {
        return size_x;
    }

    [[nodiscard]] unsigned int get_size_y() const {
        return size_y;
    }
    void expand_value (unsigned int small_x, unsigned int small_y) {
        std::cout << _get(small_x-1,small_y-1);
        for (unsigned int x = 0; x < size_x; ++x)
            for (unsigned int y = small_y; y < size_y; ++y)
                _get(x,y) = _get(x,small_y-1);
        for (unsigned int x = small_x; x < size_x; ++x)
            for (unsigned int y = 0; y < size_y; ++y)
                _get(x,y) = _get(small_x-1,y);
        for (unsigned int x = small_x; x < size_x; ++x)
            for (unsigned int y = small_y; y < size_y; ++y)
                _get(x,y) = _get(small_x-1,small_y-1);
    }

    void print() {
        for (int i = 0; i < size_y; ++i) {
            for (int j = 0; j < size_x; ++j) {
                std::cout.width(6);
                std::cout << std::to_string(arr[j + size_x*i]) << "\t";
            }
            std::cout << "\n";
        }
    }

};
#endif //OOP_PROJECT01_ARRAY2D_H
