#include "Jpeg.h"

using namespace jpeg;

Pixel::Pixel(unsigned char val[3])
{
    this->v3 = val[0];
    this->v2 = val[1];
    this->v1 = val[2];
}

Pixel::Pixel(float r, float g, float b) :
    v1(r), v2(g), v3(b)
{
}

Pixel Pixel::operator+(Pixel &rgb)
{
    Pixel res;
    res.v1 = v1 + rgb.v1;
    res.v2 = v2 + rgb.v2;
    res.v3 = v3 + rgb.v3;
    return res;
}

Pixel Pixel::operator/(float x)
{
    Pixel res;
    res.v1 = v1 / x;
    res.v2 = v2 / x;
    res.v3 = v3 / x;
    return res;
}

Pixel jpeg::ycc2rgb(Pixel pixel) 
{
    Pixel res;
    res.v1 = int(pixel.v1 + 1.402 * (pixel.v3 - 128));
    res.v2 = int(pixel.v1 - 0.34414 * (pixel.v2 - 128) - 0.71414 * (pixel.v3 - 128));
    res.v3 = int(pixel.v1 + 1.772 * (pixel.v2 - 128));
    res.v1 = res.v1 > 255 ? 255 : res.v1;
    res.v2 = res.v1 > 255 ? 255 : res.v2;
    res.v3 = res.v1 > 255 ? 255 : res.v3;
    res.v1 = res.v1 < 0 ? 0 : res.v1;
    res.v2 = res.v2 < 0 ? 0 : res.v2;
    res.v3 = res.v3 < 0 ? 0 : res.v3;
    return res;
}

Pixel jpeg::rgb2ycc(Pixel pixel) 
{
    Pixel res;
    res.v1 = 0   + (0.299     * pixel.v1) + (0.587     * pixel.v2) + (0.114     * pixel.v3);
    res.v2 = 128 - (0.168736  * pixel.v1) - (0.331264  * pixel.v2) + (0.5       * pixel.v3);
    res.v3 = 128 + (0.5       * pixel.v1) - (0.418688  * pixel.v2) - (0.081312  * pixel.v3);
    return res;
}
