//
// Created by quan on 03/12/2019.
//

#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "array2d.h"
#include "fast_dct_8.h"
using namespace std;
using namespace sf;

//#define DEBUG

// Chuyển màu từ dạng RGB sang dạng YCC
// - c: chứa màu RGB cần chuyển
// - a, y, cr, cb: kênh màu alpha và các kênh màu của YCC
void rgb2ycc(Color c, Int16& a, Int16& y, Int16& cb, Int16& cr) {
    a = round(c.a - 128);
    y = round(0.299*c.r + 0.587*c.g + 0.114*c.b - 128);
    cb = round(- 0.168736*c.r - 0.331246*c.g + 0.5*c.b);
    cr = round(+ 0.5*c.r - 0.418688*c.g - 0.081312*c.b);
}
// Chuyển màu từ dạng YCC trở về dạng RGB
// - c: chứa màu RGB sau khi chuyển
// - a, y, cr, cb: kênh alpha và các kênh của YCC
void ycc2rgb(Color& c, Int16 a, Int16 y, Int16 cb, Int16 cr) {
    c.a = round(clamp(a + 128, 0, 255));
    c.r = round(clamp(y + 128 + 1.402*cr, 0.0, 255.0));
    c.g = round(clamp(y + 128 - 0.344136*cb - 0.714136*cr, 0.0, 255.0));
    c.b = round(clamp(y + 128 + 1.772*cb, 0.0, 255.0));
}
// Tạo các mảng chứa các kênh khi chuyển đổi sao cho kích thước của chúng chia hết cho 8 (là block size của thuật toán)
// - size: kích thước ảnh, quyết định kích thước các mảng
// - scale: mức thu nhỏ của các kênh màu (kênh cb vả cr)
//     mắt người nhìn các kênh màu không tốt bằng các kênh kia, nên thu nhỏ kênh màu không ảnh hưởng nhiều đến chất lượng ảnh
// - ycc_XX: được gán mảng cho kênh XX
void create_arrays(Vector2u size, int scale, array2d<Int16>& ycc_a, array2d<Int16>& ycc_y, array2d<Int16>& ycc_cb, array2d<Int16>& ycc_cr) {
    ycc_a = array2d<Int16>::create8rounded(size.x, size.y);
    ycc_y = array2d<Int16>::create8rounded(size.x, size.y);
    ycc_cb = array2d<Int16>::create8rounded(size.x/scale, size.y/scale);
    ycc_cr = array2d<Int16>::create8rounded(size.x/scale, size.y/scale);
}
// Chuyển một bức ảnh thành các kênh YCC của nó
// - scale: mức thu nhỏ của các kênh màu (kênh cb vả cr)
// - skip_transparency: được gán true nếu kiểm tra thấy kênh alpha không cần thiết
// - img: hình ảnh ban đầu
// - ycc_XX: được gán kênh XX sau khi chuyển
void img2ycc(int scale, bool& skip_transparency, const Image& img, array2d<Int16>& ycc_a, array2d<Int16>& ycc_y, array2d<Int16>& ycc_cb, array2d<Int16>& ycc_cr) {
    // tạo mảng cho các kênh
    auto size = img.getSize();
    create_arrays(size, scale, ycc_a, ycc_y, ycc_cb, ycc_cr);
    // biến để nhận các giá trị rác
    Int16 temp_cb, temp_cr;
    // xét tất cả các pixel
    for(unsigned int x = 0; x < size.x; x++)
        for(unsigned int y = 0; y < size.y; y++) {
            // chuyển mảu RGB thành YCC
            // chỉ lưu kênh màu cr và cb khi 2 tọa độ nó chia hết cho scale (thu nhỏ scale lần)
            if(x%scale == 0 && y%scale == 0)
                rgb2ycc(img.getPixel(x, y), ycc_a(x, y), ycc_y(x, y), ycc_cb(x/scale, y/scale),  ycc_cr(x/scale, y/scale));
            else rgb2ycc(img.getPixel(x, y), ycc_a(x, y), ycc_y(x, y), temp_cb, temp_cr);
        }
    // xác định có dùng kênh alpha không
    // nếu tất cả các pixel đều opague thì không cần dùng
    skip_transparency = true;
    for (int i = 0; i < ycc_a.get_size_x()*ycc_a.get_size_y(); ++i)
        if(ycc_a.arr[i] < 126) {
            skip_transparency = false;
            break;
        }
    // điền các giá trị ngoài lề của các kênh
    ycc_a.expand_value(size.x, size.y);
    ycc_y.expand_value(size.x, size.y);
    ycc_cb.expand_value(size.x/scale, size.y/scale);
    ycc_cr.expand_value(size.x/scale, size.y/scale);
}
// Chuyển các kênh YCC thành bức ảnh hoàn chỉnh
// - scale: mức thu nhỏ của các kênh màu (kênh cb vả cr)
// - skip_transparency: gán tất cả pixel thành opague hay sử dụng giá trị của kênh alpha
// - img: hình ảnh hoàn chỉnh
// - ycc_XX: kênh XX
void ycc2img(int scale, bool skip_transparency, Image& img, const array2d<Int16>& ycc_a, const array2d<Int16>& ycc_y, const array2d<Int16>& ycc_cb, const array2d<Int16>& ycc_cr) {
    auto size = img.getSize();
    for (unsigned int y = 0; y < size.y; y++) {
        for(unsigned int x = 0; x < size.x; x++) {
            Color color;
            // chuyển màu YCC thành RGB
            // nếu skip_transparency thì sử dụng giá trị tối đa cho kênh alpha (127)
            ycc2rgb(color, skip_transparency ? 127 : ycc_a(x, y), ycc_y(x, y), ycc_cb(x / scale, y / scale), ycc_cr(x / scale, y / scale));
            img.setPixel(x, y, color);
        }
    }
}
// Viết số theo kiểu VLQ vào file
// bit 128 cho biết số đã hết hay chưa
// 7 bit còn lại cho biết giá trị của số
void write_variable_length(ostream& stream, Uint16 value) {
    while(value >= 128) {
        stream << (char)(value % 128);
        value /= 128;
    }
    stream << (char)(value %128+128);
}
// Đọc theo kiểu Variable-length quantity (VLQ) từ file
// nếu byte đọc vào có bit 128 là 1 thì ngừng
void read_variable_length(istream& stream, Uint16& value) {
    unsigned char temp;
    long int multiplier = 1;
    value = 0;
    stream.read(reinterpret_cast<char *>(&temp), 1);
    while (temp < 128) {
        value += multiplier*temp;
        multiplier *= 128;
        stream.read(reinterpret_cast<char *>(&temp), 1);
    }
    value += multiplier*(temp - 128);
}
void read_variable_length(istream& stream, Int16& value) {
    unsigned char temp;
    long int multiplier = 1;
    value = 0;
    stream.read(reinterpret_cast<char *>(&temp), 1);
    while (temp < 128) {
        value += multiplier*temp;
        multiplier *= 128;
        stream.read(reinterpret_cast<char *>(&temp), 1);
    }
    value += multiplier*(temp - 128);
}
// sử dụng VLQ để đọc và viết tất cả các số trong file
void write_value(ostream& stream, Int16 value) {
    write_variable_length(stream, value);
//    stream.write(reinterpret_cast<char *>(&value), 2);
}
void read_value(istream& stream, Int16& value) {
    read_variable_length(stream, value);
//    stream.read(reinterpret_cast<char *>(&value), 2);
}
void write_value(ostream& stream, Uint16 value) {
    stream.write(reinterpret_cast<char *>(&value), 2);
}
void read_value(istream& stream, Uint16& value) {
    stream.read(reinterpret_cast<char *>(&value), 2);
}
void write_rle_zero_coding_ending(ostream& stream, Uint16& count, bool& prev_zero, streampos& count_location) {
    if(prev_zero) {
        write_value(stream, count);
        count_location = stream.tellp();
        write_value(stream, count);
    } else {
        streampos temp = stream.tellp();
        stream.seekp(count_location);
        write_value(stream, count);
        stream.seekp(temp);
    }
}
// Mã hóa các số viết vào file theo một kiểu giống Run-length encoding
// mỗi block bao gồm 2 byte cho biết số số 0, 2 byte cho biết số số khác 0, phần còn lại chứa các số khác 0
// - stream: file được viết vào
// - count: số chữ số 0 hoặc khác 0 liên tiếp
// - prev_zero: phía trước có phải là số 0 hay không
void write_rle_zero_coding (ostream& stream, Uint16& count, bool& prev_zero, streampos& count_location, Int16 value) {
    bool is_zero = value == 0;
    if(is_zero == prev_zero)
        count++;
    else {
        write_rle_zero_coding_ending(stream, count, prev_zero, count_location);
        prev_zero = is_zero;
        count = 1;
    }
    if(!is_zero)
        write_value(stream, value);
}
// Đọc mã của hàm write_rle_zero_coding
void read_rle_zero_coding (istream& stream, Uint16& count, bool& prev_zero, Int16& value) {
    while(count == 0) {
        read_value(stream, count);
        prev_zero = !prev_zero;
    }
    count --;
    if(prev_zero)
        value = 0;
    else {
        read_value(stream, value);
    }
}
// Tọa độ của các vị trí trên đường zig zag qua một block đã qua Discrete Cosine Transform (DCT) (8x8)
// các tọa độ này được sắp xếp giảm dần theo độ quan trọng của giá trị chứa trong ô đó
// khi nén ta sẽ bỏ bớt các giá trị không quan trọng
int zig_zag_x[] = {
        1, 0, 0, 1, 2, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 0,
        1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7,
        7, 6, 5, 4, 3, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 5, 6, 7, 7, 6, 7
};
int zig_zag_y[] = {
        0, 1, 2, 1, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6,
        5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1,
        2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 4, 5, 6, 7, 7, 6, 5, 6, 7, 7
};

// Các giá trị nén ứng với các mức độ nén của ảnh
// scale cho biết mức thu nhỏ của các kênh màu
static const int scales[] = {4, 4, 2, 1};
// chrom_skips cho biết mức chất lượng của màu sắc
static const int chrom_skips[] = {4, 8, 12, 63};
// lum_skíp cho biết mức chất lượng của kênh alpha và độ sáng
static const int lum_skips[] = {8, 16, 24, 63};

// viết các kênh vào file
void encode(ostream& stream, bool skip_transparency, int quality, const Image& img, const array2d<Int16>& ycc_a, const array2d<Int16>& ycc_y, const array2d<Int16>& ycc_cb, const array2d<Int16>& ycc_cr) {
    auto size = img.getSize();
    int scale = scales[quality], chrom_skip = chrom_skips[quality], lum_skip = lum_skips[quality];
    // viết kích thước ảnh
    stream.write(reinterpret_cast<char *>(&size.x), 4);
    stream.write(reinterpret_cast<char *>(&size.y), 4);
    // viết các tham sô chất lượng và skip_transparency
    stream.write(reinterpret_cast<char *>(&quality), 4);
    stream.write(reinterpret_cast<char *>(&skip_transparency), 1);
    // viết các giá trị trung bình của mỗi block
    for (int x = 0; x < ycc_a.get_size_x(); x+=8)
        for (int y = 0; y < ycc_a.get_size_y(); y+=8){
            if (!skip_transparency)
                write_value(stream, ycc_a(x,y));
            write_value(stream, ycc_y(x,y));
        }
    for (int x = 0; x < ycc_cr.get_size_x(); x+=8)
        for (int y = 0; y < ycc_cr.get_size_y(); y+=8){
            write_value(stream, ycc_cb(x,y));
            write_value(stream, ycc_cr(x,y));
        }
    // viết các tham số khác của phép biến đổi DCT
    Uint16 count = 0;
    bool prev_zero = true;
    streampos count_location = stream.tellp();
    for (int i = 0; i < 63; i++) {
        if(i>lum_skip)
            continue;
        for (int x = 0; x < ycc_a.get_size_x(); x+=8)
            for (int y = 0; y < ycc_a.get_size_y(); y+=8){
                if (!skip_transparency)
                    write_rle_zero_coding(stream, count, prev_zero, count_location, ycc_a(x+zig_zag_x[i], y+zig_zag_y[i]));
                write_rle_zero_coding(stream, count, prev_zero, count_location, ycc_y(x+zig_zag_x[i], y+zig_zag_y[i]));
            }
        if(i>chrom_skip)
            continue;
        for (int x = 0; x < ycc_cb.get_size_x(); x+=8)
            for (int y = 0; y < ycc_cb.get_size_y(); y+=8){
                write_rle_zero_coding(stream, count, prev_zero, count_location, ycc_cb(x+zig_zag_x[i], y+zig_zag_y[i]));
                write_rle_zero_coding(stream, count, prev_zero, count_location, ycc_cr(x+zig_zag_x[i], y+zig_zag_y[i]));
            }
    }
    write_rle_zero_coding_ending(stream, count, prev_zero, count_location);
    cout << "done encode";
}
// giải nén các kênh sang hình ảnh
void decode(istream& stream, bool& skip_transparency, int& quality, Image& img, array2d<Int16>& ycc_a, array2d<Int16>& ycc_y, array2d<Int16>& ycc_cb, array2d<Int16>& ycc_cr) {
    Vector2u size;
    // đọc kích thước ảnh
    stream.read(reinterpret_cast<char *>(&size.x), 4);
    stream.read(reinterpret_cast<char *>(&size.y), 4);
    // đọc các giá trị trung bình của mỗi block
    stream.read(reinterpret_cast<char *>(&quality), 4);
    stream.read(reinterpret_cast<char *>(&skip_transparency), 1);
    int scale = scales[quality], chrom_skip = chrom_skips[quality], lum_skip = lum_skips[quality];
    img.create(size.x, size.y);
    create_arrays(size, scale, ycc_a, ycc_y, ycc_cb, ycc_cr);
    // đọc các giá trị trung bình của mỗi block
    for (int x = 0; x < ycc_a.get_size_x(); x+=8)
        for (int y = 0; y < ycc_a.get_size_y(); y+=8){
            if (!skip_transparency)
                read_value(stream, ycc_a(x,y));
            read_value(stream, ycc_y(x,y));
        }
    for (int x = 0; x < ycc_cr.get_size_x(); x+=8)
        for (int y = 0; y < ycc_cr.get_size_y(); y+=8) {
            read_value(stream, ycc_cb(x,y));
            read_value(stream, ycc_cr(x,y));
        }
    // đọc các tham số khác của phép biến đổi DCT
    Uint16 count = 0;
    bool prev_zero = false;
    for (int i = 0; i < 63; i++) {
        for (int x = 0; x < ycc_a.get_size_x(); x+=8)
            for (int y = 0; y < ycc_a.get_size_y(); y+=8){
                if(i>lum_skip){
                    if (!skip_transparency)
                        ycc_a(x+zig_zag_x[i], y+zig_zag_y[i]) = 0;
                    ycc_y(x+zig_zag_x[i], y+zig_zag_y[i]) = 0;
                } else {
                    if (!skip_transparency)
                        read_rle_zero_coding(stream, count, prev_zero, ycc_a(x + zig_zag_x[i], y + zig_zag_y[i]));
                    read_rle_zero_coding(stream, count, prev_zero, ycc_y(x + zig_zag_x[i], y + zig_zag_y[i]));
                }
            }
        for (int x = 0; x < ycc_cb.get_size_x(); x+=8)
            for (int y = 0; y < ycc_cb.get_size_y(); y+=8){
                if(i>chrom_skip){
                    ycc_cb(x+zig_zag_x[i], y+zig_zag_y[i]) = 0;
                    ycc_cr(x+zig_zag_x[i], y+zig_zag_y[i]) = 0;
                } else {
                    read_rle_zero_coding(stream, count, prev_zero, ycc_cb(x+zig_zag_x[i], y+zig_zag_y[i]));
                    read_rle_zero_coding(stream, count, prev_zero, ycc_cr(x+zig_zag_x[i], y+zig_zag_y[i]));
                }
            }
    }
}
// thực hiện nén ảnh và xuất ra file
void compress(const string &input, const string& output, int quality) {
    auto img = new Image;
    img->loadFromFile(input);
    array2d<Int16> ycc_a, ycc_y, ycc_cb, ycc_cr;
    bool skip_transparency;
    img2ycc(scales[quality], skip_transparency, *img, ycc_a, ycc_y, ycc_cb, ycc_cr);
#ifdef DEBUG
    std::cout << "ycc a:\n";
    ycc_a.print();
    std::cout << "ycc y:\n";
    ycc_y.print();
    std::cout << "ycc cb:\n";
    ycc_cb.print();
    std::cout << "ycc cr:\n";
    ycc_cr.print();
#endif
    fast_dct_8::dct_quantize(ycc_a, ycc_y, ycc_cb, ycc_cr, quality);
#ifdef DEBUG
    std::cout << "after: \n";
    std::cout << "ycc a:\n";
    ycc_a.print();
    std::cout << "ycc y:\n";
    ycc_y.print();
    std::cout << "ycc cb:\n";
    ycc_cb.print();
    std::cout << "ycc cr:\n";
    ycc_cr.print();
#endif
    ofstream ostr;
    ostr.open(output);
    encode(ostr, skip_transparency, quality, *img, ycc_a, ycc_y, ycc_cb, ycc_cr);
    ostr.close();
}
// giải nén ảnh từ file và xuất ra file ảnh dạng png
void decompress(const string& input, const string& output) {
    auto img = new Image;
    array2d<Int16> ycc_a, ycc_y, ycc_cb, ycc_cr;
    bool skip_transparency;
    int quality;
    ifstream istr;
    istr.open(input);
    decode(istr, skip_transparency, quality, *img, ycc_a, ycc_y, ycc_cb, ycc_cr);
    istr.close();
#ifdef DEBUG
    std::cout << "pre-output: \n";
    std::cout << "ycc a:\n";
    ycc_a.print();
    std::cout << "ycc y:\n";
    ycc_y.print();
    std::cout << "ycc cb:\n";
    ycc_cb.print();
    std::cout << "ycc cr:\n";
    ycc_cr.print();
#endif
    fast_dct_8::inverse_dct_quantize(ycc_a, ycc_y, ycc_cb, ycc_cr, quality);
    ycc2img(scales[quality], skip_transparency, *img, ycc_a, ycc_y, ycc_cb, ycc_cr);
    img->saveToFile(output);
}
