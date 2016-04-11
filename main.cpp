#include "Jpeg.h"
#include "BmpImage.h"
#include "JpegEncoder.h"
#include "JpegDecoder.h"
#include <iostream>
#include <bitset>

using namespace jpeg;
using std::cout;

int main(int argc, char *argv) {
    BmpImage *bmp = new BmpImage();
    bmp->readBmp("1.bmp");

    JpegEncoder *encoder = new jpeg::JpegEncoder(2, "4:2:0");
    encoder->encodeImage(bmp->matrix, bmp->height, bmp->width);
    encoder->writeJpg("res");

    //JpegDecoder *decoder = new JpegDecoder();
    //decoder->decoderImage(encoder);

    //int* res = zigzagTransform(encoder->y_block[3]);
    //for (int i = 0; i < 64; i++)
    //    std::cout << res[i] << ' ';
    //std::cout << '\n';
    //std::cout << encoder->y_zigzag[3][0] << ' ';
    //for (int i = 0; i < encoder->y_ac.size(); i++)
        //std::cout << encoder->y_ac[i][0] << ' ' << encoder->y_ac[i][1] << ' ' << '\n';

    //std::bitset<16> x(numberEncoding(-63));
    //cout << x;

    int a;
    std::cin >> a;
    return 0;
}