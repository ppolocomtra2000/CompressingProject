//
// Created by quan on 03/12/2019.
//

#ifndef OOP_PROJECT01_FAST_DCT_8_H
#define OOP_PROJECT01_FAST_DCT_8_H

#include <SFML/Graphics.hpp>
#include "array2d.h"
using namespace sf;

namespace fast_dct_8 {
    void transform(double *input, double* output, unsigned int start, unsigned int step);
    void inverseTransform(double *input, double *output, unsigned int start, unsigned int step);
    void dct_quantize(array2d<Int16> ycc_a, array2d<Int16> ycc_y, array2d<Int16> ycc_cb, array2d<Int16> ycc_cr, int quality);
    void inverse_dct_quantize(array2d<Int16> ycc_a, array2d<Int16> ycc_y, array2d<Int16> ycc_cb, array2d<Int16> ycc_cr, int quality);
}

#endif //OOP_PROJECT01_FAST_DCT_8_H
