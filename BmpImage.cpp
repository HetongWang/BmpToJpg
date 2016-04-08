#include "BmpImage.h"
#include <iostream>
#include <fstream>
using namespace std;

namespace jpeg
{
    void BmpImage::storeRGB() {
        int i, j, offset;
        BYTE buff[3];

        matrix = new Pixel*[height];
        for (i = 0; i < height; i++) {
            matrix[i] = new Pixel[width];
            for (j = 0; j < width; j++) {
                offset = (i *(width) + j) * 3;
                buff[0] = data[offset];
                buff[1] = data[offset + 1];
                buff[2] = data[offset + 2];
                matrix[i][j] = Pixel(buff);
            }
        }
    }

    void BmpImage::readBmp(string path) {
        WORD *buff = new WORD;
        int i = 0;

        ifstream in(path, ios::binary);
        in.read((char*)buff, 2);
        if (*buff != 19778) {
            cout << "Error:" << path << "is not a bmp file";
            exit(0);
        }

        in.read((char*)&size_bmp, 4);
        in.ignore(0xC, EOF); // 12 byte
        in.read((char*)&width, 4); in.read((char*)&height, 4);
        size_img = (width) * (height) * 3;
        in.seekg(size_bmp - size_img, ios::beg); // move file point to bitmap data

        data = new BYTE[size_img];
        in.read((char*)data, size_img);

        storeRGB();
    }
}