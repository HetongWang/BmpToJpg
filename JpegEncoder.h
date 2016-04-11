#ifndef JPEGENCODER
#define JPEGENCODER

#include "Jpeg.h"
#include <string>
#include <fstream>
#include <vector>

namespace jpeg
{
    double C(int u);
    void dct(double **block);
    void quantize(double **block, BYTE quan[64]);

    int* zigzagTransform(double **block);
    int numberOfSetBits(int i);
    unsigned short numberEncoding(int n);

    class JpegEncoder
    {
    public:
        JpegEncoder();
        JpegEncoder(int quality, std::string subsampling);
        ~JpegEncoder() {};
        void writeJpg(std::string file);

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

        BYTE *lum_quan, *croma_quan;
        double ***y_block;
        double ***cb_block;
        double ***cr_block;
        int **y_zigzag;
        int **cb_zigzag;
        int **cr_zigzag;
        std::vector<std::vector<int>> y_ac;
        std::vector<std::vector<int>> cr_ac;
        std::vector<std::vector<int>> cb_ac;

    private:
        Pixel **origin;
        std::ofstream out;

        void subsample();
        void dctAndQuan();
        void zigzag();
        void deltaEncoding();
        void RLE(int **zigzag, int block_count, std::vector<std::vector<int>>&ac);
        void RLEAddPair(int zero_count, int n, std::vector<std::vector<int>> &ac);

        void JpegEncoder::makeAPP0();
        void JpegEncoder::makeDQT(int id, BYTE *table);
        void JpegEncoder::makeSOF0();
        void JpegEncoder::makeDHT(BYTE id, BYTE *bits, BYTE *vals, int len);
    };
}

#endif // !JPEGENCODER
