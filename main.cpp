#include "Jpeg.h"
#include "BmpImage.h"
#include "JpegEncoder.h"
#include "JpegDecoder.h"
#include <iostream>

using namespace jpeg;

int main(int argc, char *argv) {
    BmpImage *bmp = new BmpImage();
    bmp->readBmp("1.bmp");

    JpegEncoder *encoder = new jpeg::JpegEncoder(2, "4:2:0");
    encoder->encodeImage(bmp->matrix, bmp->height, bmp->width);

    JpegDecoder *decoder = new JpegDecoder();
    decoder->decoderImage(encoder);

    int* res = zigzagTransform(encoder->y_block[3]);
    for (int i = 0; i < 64; i++)
        std::cout << res[i] << ' ';
    std::cout << '\n';
    std::cout << encoder->y_zigzag[3][0] << ' ';

    int a;
    std::cin >> a;
    return 0;
}