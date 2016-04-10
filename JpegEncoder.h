#ifndef JPEGENCODER
#define JPEGENCODER

#include "Jpeg.h"
#include <string>
#include <fstream>

namespace jpeg
{
    float C(int u);
    void dct(float **block);
    void quantize(float **block, BYTE quan[64]);

    float* zigzag(float **block);

    class JpegEncoder
    {
    public:
        JpegEncoder();
        JpegEncoder(int quality, std::string subsampling);
        ~JpegEncoder() {};

        void encodeImage(Pixel **matrix, int height, int width);
        int quality;
        std::string chroma_subsampling;
        int img_height, img_width;

        int mcu_hor_count, mcu_ver_count;
        int mcu_width, mcu_height;

        int y_hor_count;
        int y_ver_count;
        int y_block_count;
        int c_hor_count;
        int c_ver_count;
        int c_block_count;

        float ***y_block;
        float ***cb_block;
        float ***cr_block;

    private:
        Pixel **origin;

        void subsample();
        void dctAndQuan();
        void deltaEncoding();
    };
}

#endif // !JPEGENCODER
