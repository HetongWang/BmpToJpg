#ifndef JPEGENCODER
#define JPEGENCODER

#include "Jpeg.h"
#include <string>
#include <fstream>

namespace jpeg
{
    double C(int u);
    void dct(double **block);
    void quantize(double **block, BYTE quan[64]);

    int* zigzagTransform(double **block);

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

        double ***y_block;
        double ***cb_block;
        double ***cr_block;
        int **y_zigzag;
        int **cb_zigzag;
        int **cr_zigzag;

    private:
        Pixel **origin;

        void subsample();
        void dctAndQuan();
        void zigzag();
        void deltaEncoding();
        void RLE();
    };
}

#endif // !JPEGENCODER
