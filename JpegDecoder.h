#ifndef JPEGDECODER
#define JPEGDECODER

#include "Jpeg.h"
#include "JpegEncoder.h"

namespace jpeg
{
    void idct(double **block);
    void iquantize(double **block, BYTE quan[64]);

    class JpegDecoder
    {
    public:
        JpegDecoder() {};
        ~JpegDecoder() {};
        void decoderImage(JpegEncoder *encoder);
        Pixel **origin;
    private:
        JpegEncoder *encoder;
        double ***y_block;
        double ***cb_block;
        double ***cr_block;
        void idctAndIquan();
    };
}


#endif