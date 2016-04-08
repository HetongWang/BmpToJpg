#include "JpegEncoder.h"
#include <string.h>
#include <iostream>

using std::count;
using std::string;

namespace jpeg
{
    float C(int u)
    {
        if (u == 0)
            return (1.0 / sqrt(2.0));
        else
            return 1.0;
    }

    void dct(float **block)
    {
        double a;
        float F[8][8];
        for (int u = 0; u < 8; u++)
        for (int v = 0; v < 8; v++)
        {
            a = 0.0;
            for (int x = 0; x < 8; x++)
            for (int y = 0; y < 8; y++)
                a += block[x][y] * cos((2.0*x + 1.0)*u*3.14 / 16.0) * cos((2.0*y + 1.0)*v*3.14 / 16.0);
            F[u][v] = 0.25 * C(u) * C(v) * a;
        }

        for (int u = 0; u < 8; u++)
        for (int v = 0; v < 8; v++)
            block[u][v] = F[u][v];
    }

    void quantize(float **block, BYTE quan[64]) 
    {
        int x, y;
        for (int i = 0; i < 64; i++) {
            x = i / 8;
            y = i % 8;
            block[x][y] = int(round(block[x][y] / quan[i]));
            if (block[x][y] > 65535 || block[x][y] < -65536) 
                block[x][y] > 0 ? block[x][y] = 65535 : block[x][y] = -65536;
        }
    }
    
    void  JpegEncoder::subsample()
    {
        int i, j, ii, jj;

        // chroma subsampling decide MCU
        if (chroma_subsampling == "4:4:4")
        {
            mcu_hor_count = img_width / 8; if (img_width % 8 != 0) mcu_hor_count++;
            mcu_ver_count = img_height / 8; if (img_height % 8 != 0) mcu_ver_count++;
            mcu_width = mcu_height = 8;
            y_hor_count = mcu_hor_count;
            y_ver_count = mcu_ver_count;
            y_block_count = y_hor_count * y_ver_count;
            c_hor_count = y_hor_count;
            c_ver_count = y_ver_count;
            c_block_count = y_block_count;
        }
        else if (chroma_subsampling == "4:2:2")
        {
            mcu_hor_count = img_width / 16; if (img_width % 16 != 0) mcu_hor_count++;
            mcu_ver_count = img_height / 8; if (img_height % 8 != 0) mcu_ver_count++;
            mcu_width = 16;
            mcu_height = 8;
            y_hor_count = mcu_hor_count * 2;
            y_ver_count = mcu_ver_count;
            y_block_count = y_hor_count * y_ver_count;
            c_hor_count = mcu_hor_count;
            c_ver_count = mcu_ver_count;
            c_block_count = c_hor_count * c_ver_count;
        }
        else // default 4:2:0
        {
            mcu_hor_count = img_width / 16; if (img_width % 16 != 0) mcu_hor_count++;
            mcu_ver_count = img_height / 16; if (img_height % 16 != 0) mcu_ver_count++;
            mcu_width = mcu_height = 16;
            y_hor_count = mcu_hor_count * 2;
            y_ver_count = mcu_ver_count * 2;
            y_block_count = y_hor_count * y_ver_count;
            c_hor_count = mcu_hor_count;
            c_ver_count = mcu_ver_count;
            c_block_count = c_hor_count * c_ver_count;
        }

        // init all blocks
        y_block = new float**[y_block_count];
        for (i = 0; i < y_block_count; i++)
        {
            y_block[i] = new float *[8];
            for (j = 0; j < 8; j++)
                y_block[i][j] = new float[8];
        }

        cr_block = new float **[c_block_count];
        cb_block = new float **[c_block_count];
        for (i = 0; i < c_block_count; i++)
        {
            cr_block[i] = new float *[8];
            cb_block[i] = new float *[8];
            for (j = 0; j < 8; j++)
            {
                cr_block[i][j] = new float[8];
                cb_block[i][j] = new float[8];
            }
        }

        // subsample y blocks
        int n = 0;
        for (i = 0; i < y_ver_count; i++)
        for (j = 0; j < y_hor_count; j++) 
        {
            for (ii = 0; ii < 8; ii++)
            for (jj = 0; jj < 8; jj++)
                if (i * 8 + ii < img_height && j * 8 + jj < img_width)
                {
                    Pixel ycc = rgb2ycc(origin[i * 8 + ii][j * 8 + jj]);
                    y_block[n][ii][jj] = ycc.v1;
                }
                else
                    y_block[n][ii][jj] = 0;
            n++;
        }

        // subsample cr&cb blocks
        n = 0;
        for (i = 0; i < c_ver_count; i++)
        for (j = 0; j < c_hor_count; j++) 
        {
            for (ii = 0; ii < 8; ii++)
            for (jj = 0; jj < 8; jj++)
            { 
                int img_i = i * mcu_height + ii * (mcu_height / 8);
                int img_j = j * mcu_width + jj * (mcu_width / 8);
                if (img_i < img_height && img_j < img_width)
                {
                    Pixel sum = origin[img_i][img_j];
                    int count = 0;
                    for (int x = 0; x < (mcu_height / 8); x++)
                        for (int y = 0; y < (mcu_width / 8); y++)
                            if (img_i + x < img_height && img_j + y < img_width)
                            {
                                count++;
                                sum = sum + origin[img_i + x][img_j + y];
                            }
                    sum = sum / (float)count;
                    Pixel ycc = rgb2ycc(sum);
                    cb_block[n][ii][jj] = ycc.v2;
                    cr_block[n][ii][jj] = ycc.v3;
                }
                else
                {
                    cb_block[n][ii][jj] = 0;
                    cr_block[n][ii][jj] = 0;
                }
            }
            n++;
        }
    }

    void JpegEncoder::dctAndQuan()
    {
        int i;
        BYTE *lum_quan, *croma_quan;
        switch (quality)
        {
        case 0:
            lum_quan = lum_quant0;
            croma_quan = croma_quant0;
        case 1:
            lum_quan = lum_quant1;
            croma_quan = croma_quant1;
            break;
        case 2:
            lum_quan = lum_quant2;
            croma_quan = croma_quant2;
            break;
        default:
            return;
        }

        for (i = 0; i < y_block_count; i++)
        {
            dct(y_block[i]);
            quantize(y_block[i], lum_quan);
        }
        for (i = 0; i < c_block_count; i++)
        {
            dct(cr_block[i]);
            quantize(cr_block[i], croma_quan);
            dct(cb_block[i]);
            quantize(cb_block[i], croma_quan);
        }
    }

    void JpegEncoder::encodeImage(Pixel **matrix, int height, int width)
    {
        origin = matrix;
        img_height = height;
        img_width = width;
        subsample();
        dctAndQuan();
    }

    JpegEncoder::JpegEncoder(int quality, string subsampling) :
        chroma_subsampling(subsampling),
        quality(quality)
    {

    }

    JpegEncoder::JpegEncoder()
    {
        quality = 1;
        chroma_subsampling = "4:2:0";
    }
}