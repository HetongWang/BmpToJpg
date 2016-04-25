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
    bmp->readBmp("3.bmp");

    JpegEncoder *encoder = new jpeg::JpegEncoder(0, "4:2:0");
    encoder->encodeImage(bmp->matrix, bmp->height, bmp->width);
    encoder->writeJpg("res");

    //JpegDecoder *decoder = new JpegDecoder();
    //decoder->decoderImage(encoder);

    for (int i = 0; i < 4; i++)
        std::cout << encoder->y_zigzag[i][0] << '\n';
    std::cout << encoder->cb_zigzag[0][0] << '\n';
    std::cout << encoder->cr_zigzag[0][0] << '\n';
    for (int i = 0; i < encoder->y_ac.size(); i++)
        std::cout << encoder->y_ac[i][0] << ' ' << encoder->y_ac[i][1] << ' ' << '\n';

    //std::bitset<16> x(numberEncoding(-63));
    //cout << x;

    int a;
    std::cin >> a;
    return 0;
}