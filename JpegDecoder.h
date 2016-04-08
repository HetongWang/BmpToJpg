#ifndef JPEGDECODER
#define JPEGDECODER

#include "Jpeg.h"
#include "JpegEncoder.h"

namespace jpeg
{
    void idct(float **block);
    void iquantize(float **block, BYTE quan[64]);

    class JpegDecoder
    {
    public:
        JpegDecoder() {};
        ~JpegDecoder() {};
        void decoderImage(JpegEncoder *encoder);
        Pixel **origin;
    private:
        JpegEncoder *encoder;
        float ***y_block;
        float ***cb_block;
        float ***cr_block;
        void idctAndIquan();
    };
}


#endif