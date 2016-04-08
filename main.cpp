#include "Jpeg.h"
#include "BmpImage.h"
#include "JpegEncoder.h"
#include "JpegDecoder.h"
#include <iostream>

using namespace jpeg;

int main(int argc, char *argv) {
    BmpImage *bmp = new BmpImage();
    bmp->readBmp("1");

    JpegEncoder *encoder = new jpeg::JpegEncoder(1, "4:4:4");
    encoder->encodeImage(bmp->matrix, bmp->height, bmp->width);

    JpegDecoder *decoder = new JpegDecoder();
    decoder->decoderImage(encoder);

    Pixel p = Pixel(255, 255, 255);
    p = rgb2ycc(p);
    p = ycc2rgb(p);
    std::cout << p.v1 << " " << p.v2 << " " << p.v3 << " " << std::endl;

    int a;
    std::cin >> a;
    return 0;
}