//
// Created by quan on 03/12/2019.
//

#pragma  once

#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "array2d.h"
#include "fast_dct_8.h"
using namespace std;
using namespace sf;

void compress(const string &input, const string& output, int quality);
void decompress(const string& input, const string& output);