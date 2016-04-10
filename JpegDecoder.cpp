#include "JpegDecoder.h"
#include <iostream>

namespace jpeg
{
    void idct(double **block)
    {
        double a;
        double f[8][8];
        for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
        {
            a = 0.0;
            for (int u = 0; u < 8; u++)
            for (int v = 0; v < 8; v++)
                a += C(u) * C(v) * block[u][v] * cos((2.0*x + 1.0)*u*3.14 / 16.0) * cos((2.0*y + 1.0)*v*3.14 / 16.0);
            f[x][y] = 0.25 * a;
        }

        for (int u = 0; u < 8; u++)
        for (int v = 0; v < 8; v++)
            block[u][v] = f[u][v];
    }

    void iquantize(double ** block, BYTE quan[64])
    {
        int x, y;
        for (int i = 0; i < 64; i++) {
            x = i / 8;
            y = i % 8;
            block[x][y] = block[x][y] * quan[i];
        }
    }

    void JpegDecoder::idctAndIquan()
    {
        int i, ii, jj;
        BYTE *lum_quan, *croma_quan;
        switch (encoder->quality)
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

        for (i = 0; i < encoder->y_block_count; i++)
        {
            int corner_x, corner_y;
            iquantize(y_block[i], lum_quan);
            idct(y_block[i]);
            corner_x = i / encoder->y_hor_count * 8;
            corner_y = i % encoder->y_hor_count * 8;
            for (ii = 0; ii < 8; ii++)
            for (jj = 0; jj < 8; jj++)
            {
                if (corner_x + ii < encoder->img_height &&
                    corner_y + jj < encoder->img_width)
                    origin[corner_x + ii][corner_y + jj].v1 = y_block[i][ii][jj];
            }
        }

        for (i = 0; i < encoder->c_block_count; i++)
        {
            int corner_x, corner_y;
            iquantize(cr_block[i], croma_quan);
            iquantize(cb_block[i], croma_quan);
            idct(cr_block[i]);
            idct(cb_block[i]);
            corner_x = i / encoder->c_hor_count * encoder->mcu_height;
            corner_y = i % encoder->c_hor_count * encoder->mcu_width;
            for (ii = 0; ii < encoder->mcu_height; ii++)
            for (jj = 0; jj < encoder->mcu_width; jj++)
            {
                if (corner_x + ii < encoder->img_height &&
                    corner_y + jj < encoder->img_width)
                {
                    int v_ratio = encoder->mcu_height / 8;
                    int h_ratio = encoder->mcu_width / 8;
                    origin[corner_x + ii][corner_y + jj].v2 = cb_block[i][ii / v_ratio][jj / h_ratio];
                    origin[corner_x + ii][corner_y + jj].v3 = cr_block[i][ii / v_ratio][jj / h_ratio];
                }
            }
        }
    }

    void JpegDecoder::decoderImage(JpegEncoder * encoder)
    {
        int i, j, k;
        this->encoder = encoder;

        // init all blocks
        y_block = new double**[encoder->y_block_count];
        for (i = 0; i < encoder->y_block_count; i++)
        {
            y_block[i] = new double *[8];
            for (j = 0; j < 8; j++)
            {
                y_block[i][j] = new double[8];
                for (k = 0; k < 8; k++)
                    y_block[i][j][k] = encoder->y_block[i][j][k];
            }
        }

        cr_block = new double **[encoder->c_block_count];
        cb_block = new double **[encoder->c_block_count];
        for (i = 0; i < encoder->c_block_count; i++)
        {
            cr_block[i] = new double *[8];
            cb_block[i] = new double *[8];
            for (j = 0; j < 8; j++)
            {
                cr_block[i][j] = new double[8];
                cb_block[i][j] = new double[8];
                for (k = 0; k < 8; k++)
                {
                    cr_block[i][j][k] = encoder->cr_block[i][j][k];
                    cb_block[i][j][k] = encoder->cb_block[i][j][k];
                }
            }
        }

        // Decoding begins
        origin = new Pixel*[encoder->img_height];
        for (i = 0; i < encoder->img_height; i++)
        {
            origin[i] = new Pixel[encoder->img_width];
            for (j = 0; j < encoder->img_width; j++)
                origin[i][j] = Pixel();
        }

        idctAndIquan();
        for (i = 0; i < encoder->img_height; i++)
            for (j = 0; j < encoder->img_width; j++)
                origin[i][j] = ycc2rgb(origin[i][j]);
    }
}
