#ifndef BMPIMAGE_H
#define BMPIMAGE_H

#include "Jpeg.h"
#include <string>

namespace jpeg
{
    class BmpImage
    {
    public:
        BmpImage() {};
        ~BmpImage();
        Pixel **matrix;
        int height;
        int width;
        void readBmp(std::string);
    private:
        BYTE *data;
        int size_bmp;
        int size_img;
        void storeRGB();
    };

    inline BmpImage::~BmpImage() {
        delete[]data;
    }
}

#endif