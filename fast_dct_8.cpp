//
// Created by quan on 03/12/2019.
//



#include <cmath>
#include <SFML/Graphics.hpp>
#include "fast_dct_8.h"
using namespace sf;

// thuật toán biến đổi Discrete Cosine Transform đã được cải tiến để áp dụng lên 8 phần tử với tốc độ nhanh nhất có thể
//
//
static const double S[] = {
        0.353553390593273762200422,
        0.254897789552079584470970,
        0.270598050073098492199862,
        0.300672443467522640271861,
        0.353553390593273762200422,
        0.449988111568207852319255,
        0.653281482438188263928322,
        1.281457723870753089398043,
};

static const double A[] = {
        NAN,
        0.707106781186547524400844,
        0.541196100146196984399723,
        0.707106781186547524400844,
        1.306562964876376527856643,
        0.382683432365089771728460,
};
void fast_dct_8::transform(double* input, double* output, unsigned int start, unsigned int step) {
    const double v0 = input[start + step*0] + input[start + step*7];
    const double v1 = input[start + step*1] + input[start + step*6];
    const double v2 = input[start + step*2] + input[start + step*5];
    const double v3 = input[start + step*3] + input[start + step*4];
    const double v4 = input[start + step*3] - input[start + step*4];
    const double v5 = input[start + step*2] - input[start + step*5];
    const double v6 = input[start + step*1] - input[start + step*6];
    const double v7 = input[start + step*0] - input[start + step*7];

    const double v8 = v0 + v3;
    const double v9 = v1 + v2;
    const double v10 = v1 - v2;
    const double v11 = v0 - v3;
    const double v12 = -v4 - v5;
    const double v13 = (v5 + v6) * A[3];
    const double v14 = v6 + v7;

    const double v15 = v8 + v9;
    const double v16 = v8 - v9;
    const double v17 = (v10 + v11) * A[1];
    const double v18 = (v12 + v14) * A[5];

    const double v19 = -v12 * A[2] - v18;
    const double v20 = v14 * A[4] - v18;

    const double v21 = v17 + v11;
    const double v22 = v11 - v17;
    const double v23 = v13 + v7;
    const double v24 = v7 - v13;

    const double v25 = v19 + v24;
    const double v26 = v23 + v20;
    const double v27 = v23 - v20;
    const double v28 = v24 - v19;

    output[start + step*0] = S[0] * v15;
    output[start + step*1] = S[1] * v26;
    output[start + step*2] = S[2] * v21;
    output[start + step*3] = S[3] * v28;
    output[start + step*4] = S[4] * v16;
    output[start + step*5] = S[5] * v25;
    output[start + step*6] = S[6] * v22;
    output[start + step*7] = S[7] * v27;
}

void fast_dct_8::inverseTransform(double* input, double* output, unsigned int start, unsigned int step) {
    const double v15 = input[start + step*0] / S[0];
    const double v26 = input[start + step*1] / S[1];
    const double v21 = input[start + step*2] / S[2];
    const double v28 = input[start + step*3] / S[3];
    const double v16 = input[start + step*4] / S[4];
    const double v25 = input[start + step*5] / S[5];
    const double v22 = input[start + step*6] / S[6];
    const double v27 = input[start + step*7] / S[7];

    const double v19 = (v25 - v28) / 2;
    const double v20 = (v26 - v27) / 2;
    const double v23 = (v26 + v27) / 2;
    const double v24 = (v25 + v28) / 2;

    const double v7  = (v23 + v24) / 2;
    const double v11 = (v21 + v22) / 2;
    const double v13 = (v23 - v24) / 2;
    const double v17 = (v21 - v22) / 2;

    const double v8 = (v15 + v16) / 2;
    const double v9 = (v15 - v16) / 2;

    const double v18 = (v19 - v20) * A[5];  // Different from original
    const double v12 = (v19 * A[4] - v18) / (A[2] * A[5] - A[2] * A[4] - A[4] * A[5]);
    const double v14 = (v18 - v20 * A[2]) / (A[2] * A[5] - A[2] * A[4] - A[4] * A[5]);

    const double v6 = v14 - v7;
    const double v5 = v13 / A[3] - v6;
    const double v4 = -v5 - v12;
    const double v10 = v17 / A[1] - v11;

    const double v0 = (v8 + v11) / 2;
    const double v1 = (v9 + v10) / 2;
    const double v2 = (v9 - v10) / 2;
    const double v3 = (v8 - v11) / 2;

    output[start + step*0] = (v0 + v7) / 2;
    output[start + step*1] = (v1 + v6) / 2;
    output[start + step*2] = (v2 + v5) / 2;
    output[start + step*3] = (v3 + v4) / 2;
    output[start + step*4] = (v3 - v4) / 2;
    output[start + step*5] = (v2 - v5) / 2;
    output[start + step*6] = (v1 - v6) / 2;
    output[start + step*7] = (v0 - v7) / 2;
}
//
//
// kết thúc thuật toán DCT

// áp dụng phép biến đổi DCT lên một block (8x8) của input
// sau DCT, các giá trị sẽ ở kiểu double - ko phù hợp để nén
// ta chia các giá trị này với giá trị trong bảng quantization rồi làm tròn thành số nguyên để nén
// - input, output: gt vào và ra
// - qtable: bảng quantization được dùng
// - x, y: tọa độ của block đó
void _dct_quantize(array2d<Int16> input, array2d<Int16> output, const float* qtable, unsigned int x, unsigned int y) {
    // chép các giá trị của block qua mảng tạm
    auto temp = new double[64];
    for (unsigned int i = 0; i < 8; ++i)
        for (unsigned int j = 0; j < 8; ++j)
            temp[i+j*8] = input.try_get(i+x, j+y);
    // thực hiện biến đổi
    for (int i = 0; i < 8; ++i)
        fast_dct_8::transform(temp, temp, i*8, 1);
    for (int i = 0; i < 8; ++i)
        fast_dct_8::transform(temp, temp, i, 8);
    // chia giá trị kết quả cho giá trị trong bảng qtable và gán cho output
    for (unsigned int i = 0; i < 8; ++i)
        for (unsigned int j = 0; j < 8; ++j)
            output.try_set(i+x, j+y, (Int16)(round(temp[i+j*8] / qtable[i+j*8])));
}
// thực hiện _dct_quantize cho tất cả các block trong mảng
void _dct_quantize(array2d<Int16> input, const float* qtable) {
    for (int i = 0; i < input.get_size_x(); i+=8)
        for (int j = 0; j < input.get_size_y(); j+=8)
            _dct_quantize(input, input, qtable, i,j);
}

// thực hiện phép toán đảo ngược của _dct_quantize
void _inverse_dct_quantize(array2d<Int16> input, array2d<Int16> output, const float* qtable, unsigned int x, unsigned int y) {
    auto temp = new double[64];
    for (unsigned int i = 0; i < 8; ++i)
        for (unsigned int j = 0; j < 8; ++j)
            temp[i+j*8] = input.try_get(i+x, j+y) * qtable[i+j*8];
    for (int i = 0; i < 8; ++i)
        fast_dct_8::inverseTransform(temp, temp, i, 8);
    for (int i = 0; i < 8; ++i)
        fast_dct_8::inverseTransform(temp, temp, i*8, 1);
    for (unsigned int i = 0; i < 8; ++i)
        for (unsigned int j = 0; j < 8; ++j)
            output.try_set(i+x, j+y, (Int16)round(temp[i+j*8]));
}
void _inverse_dct_quantize(array2d<Int16> input, const float* qtable) {
    for (int i = 0; i < input.get_size_x(); i+=8)
        for (int j = 0; j < input.get_size_y(); j+=8)
            _inverse_dct_quantize(input, input, qtable, i,j);
}

// bảng quantization dùng cho các mức độ nén khác nhau
// 40: chất lượng thấp nhất, 100: cao nhất
// lum: dùng có kênh độ sáng và kênh alpha, chrom: dùng cho các kênh màu
static const float quantize_table_40_lum[] = {
        20, 14, 13, 20, 30, 50, 64, 76,
        15, 15, 18, 24, 33, 73, 75, 69,
        18, 16, 20, 30, 50, 71, 86, 70,
        18, 21, 28, 36, 64, 109, 100, 78,
        23, 28, 46, 70, 85, 136, 129, 96,
        30, 44, 69, 80, 101, 130, 141, 115,
        61, 80, 98, 109, 129, 151, 150, 126,
        90, 115, 119, 123, 140, 125, 129, 124
};
static const float quantize_table_40_chrom[] = {
        21, 23, 30, 59, 124, 124, 124, 124,
        23, 26, 33, 83, 124, 124, 124, 124,
        30, 33, 70, 124, 124, 124, 124, 124,
        59, 83, 124, 124, 124, 124, 124, 124,
        124, 124, 124, 124, 124, 124, 124, 124,
        124, 124, 124, 124, 124, 124, 124, 124,
        124, 124, 124, 124, 124, 124, 124, 124,
        124, 124, 124, 124, 124, 124, 124, 124
};
static const float quantize_table_70_lum[] = {
        10, 7, 6, 10, 14, 24, 31, 37,
        7, 7, 8, 11, 16, 35, 36, 33,
        8, 8, 10, 14, 24, 34, 41, 34,
        8, 10, 13, 17, 31, 52, 48, 37,
        11, 13, 22, 34, 41, 65, 62, 46,
        14, 21, 33, 38, 49, 62, 68, 55,
        29, 38, 47, 52, 62, 73, 72, 61,
        43, 55, 57, 59, 67, 60, 62, 59
};
static const float quantize_table_70_chrom[] = {
        10, 11, 14, 28, 59, 59, 59, 59,
        11, 13, 16, 40, 59, 59, 59, 59,
        14, 16, 34, 59, 59, 59, 59, 59,
        28, 40, 59, 59, 59, 59, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59
};
static const float quantize_table_90_chrom[] = {
        1, 1, 1, 1, 1, 1, 1, 2,
        1, 1, 1, 1, 1, 1, 1, 2,
        1, 1, 1, 1, 1, 1, 2, 2,
        1, 1, 1, 1, 1, 2, 2, 3,
        1, 1, 1, 1, 2, 2, 3, 3,
        1, 1, 1, 2, 2, 3, 3, 3,
        1, 1, 2, 2, 3, 3, 3, 3,
        2, 2, 2, 3, 3, 3, 3, 3
};

static const float quantize_table_90_lum[] = {
        1, 1, 1, 2, 3, 3, 3, 3,
        1, 1, 1, 2, 3, 3, 3, 3,
        1, 1, 2, 3, 3, 3, 3, 3,
        2, 2, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3
};
static const float quantize_table_100_chrom[] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
};

static const float quantize_table_100_lum[] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
};

static const float* qtable_lum[] = {quantize_table_40_lum, quantize_table_70_lum, quantize_table_90_lum, quantize_table_100_lum};
static const float* qtable_chrom[] = {quantize_table_40_chrom, quantize_table_70_chrom, quantize_table_90_chrom, quantize_table_100_chrom};

// thực hiện _dct_quantize, tự chọn bảng qtable ứng với chất lượng cho trước
void fast_dct_8::dct_quantize(array2d<Int16> ycc_a, array2d<Int16> ycc_y, array2d<Int16> ycc_cb, array2d<Int16> ycc_cr, int quality) {
    _dct_quantize(ycc_a, qtable_lum[quality]);
    _dct_quantize(ycc_y, qtable_lum[quality]);
    _dct_quantize(ycc_cb, qtable_chrom[quality]);
    _dct_quantize(ycc_cr, qtable_chrom[quality]);
}

void fast_dct_8::inverse_dct_quantize(array2d<Int16> ycc_a, array2d<Int16> ycc_y, array2d<Int16> ycc_cb, array2d<Int16> ycc_cr, int quality) {
    _inverse_dct_quantize(ycc_a, qtable_lum[quality]);
    _inverse_dct_quantize(ycc_y, qtable_lum[quality]);
    _inverse_dct_quantize(ycc_cb, qtable_chrom[quality]);
    _inverse_dct_quantize(ycc_cr, qtable_chrom[quality]);
}
