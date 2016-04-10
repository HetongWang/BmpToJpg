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

    int a;
    std::cin >> a;
    return 0;
}