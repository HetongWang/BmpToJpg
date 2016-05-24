#include "JpegEncoder.h"
#include <string.h>
#include <iostream>
#include <fstream>

using std::cout;
using std::vector;
using std::string;

namespace jpeg
{
    int zigzag_table[64] = {0, 1, 5, 6, 14, 15, 27, 28, 
                            2, 4, 7, 13, 16, 26, 29, 42, 
                            3, 8, 12, 17, 25, 30, 41, 43, 
                            9, 11, 18, 24, 31, 40, 44, 53, 
                            10, 19, 23, 32, 39, 45, 52, 54,
                            20, 22, 33, 38, 46, 51, 55, 60, 
                            21, 34, 37, 47, 50, 56, 59, 61, 
                            35, 36, 48, 49, 57, 58, 62, 63};
    double C(int u)
    {
        if (u == 0)
            return (1.0 / sqrt(2.0));
        else
            return 1.0;
    }

    void dct(double **block) 
    {
        double a;
        double F[8][8];
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

    void quantize(double **block, BYTE quan[64]) 
    {
        int x, y;
        for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            block[i][j] = block[i][j] / quan[zigzag_table[i * 8 + j]];
            if (block[i][j] > 2048 || block[i][j] < -2048) 
                block[i][j] > 0 ? block[i][j] = 2048 : block[i][j] = -2048;
        }
    }

    int* zigzagTransform(double **block)
    {
        int* res = new int[64];
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                res[zigzag_table[i * 8 + j]] = round(block[i][j]);
            }
        }

        return res;
    }

    int* zigzagTransform(BYTE *block)
    {
        int* res = new int[64];
        for (int i = 0; i < 64; i++)
        {
            res[zigzag_table[i]] = round(block[i]);
        }

        return res;
    }

    int numberOfSetBits(int n)
    {
        int i = 2, res = 1;
        n = abs(n);
        while (n >= i)
        {
            i *= 2;
            res++;
        }
        return res;
    }
    
    unsigned short numberEncoding(int n)
    {
        unsigned short res;
        int bits = numberOfSetBits(n);
        if (n < 0)
        {
            res = abs(n);
            unsigned short t = 0;
            for (int i = 0; i < bits; i++)
                t = (t << 1) + 1;

            res = t - res;
        }
        else
            res = n;
        return res;
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
        y_block = new double**[y_block_count];
        for (i = 0; i < y_block_count; i++)
        {
            y_block[i] = new double *[8];
            for (j = 0; j < 8; j++)
                y_block[i][j] = new double[8];
        }

        cr_block = new double **[c_block_count];
        cb_block = new double **[c_block_count];
        for (i = 0; i < c_block_count; i++)
        {
            cr_block[i] = new double *[8];
            cb_block[i] = new double *[8];
            for (j = 0; j < 8; j++)
            {
                cr_block[i][j] = new double[8];
                cb_block[i][j] = new double[8];
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
                {
                    y_block[n][ii][jj] = 0;
                    // this part is unstable
                    if (i * 8 + ii >= img_height && j * 8 + jj >= img_width)
                        y_block[n][ii][jj] = rgb2ycc(origin[img_height - 1][img_width - 1]).v1;
                    else if (i * 8 + ii >= img_height)
                        y_block[n][ii][jj] = rgb2ycc(origin[img_height - 1][j * 8 + jj]).v1;
                    else if (j * 8 + jj >= img_width)
                        y_block[n][ii][jj] = rgb2ycc(origin[i * 8 + ii][img_width - 1]).v1;
                }
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
                    sum = sum / (double)count;
                    Pixel ycc = rgb2ycc(sum);
                    cb_block[n][ii][jj] = ycc.v2;
                    cr_block[n][ii][jj] = ycc.v3;
                }
                else
                {
                    cb_block[n][ii][jj] = 0;
                    cr_block[n][ii][jj] = 0;

                    if (img_i >= img_height && img_j >= img_width)
                        cb_block[n][ii][jj] = rgb2ycc(origin[img_height - 1][img_width - 1]).v2;
                    else if (img_i >= img_height)
                        cb_block[n][ii][jj] = rgb2ycc(origin[img_height - 1][img_j]).v2;
                    else if (img_j >= img_width)
                        cb_block[n][ii][jj] = rgb2ycc(origin[img_i][img_width - 1]).v2;

                    if (img_i >= img_height && img_j >= img_width)
                        cr_block[n][ii][jj] = rgb2ycc(origin[img_height - 1][img_width - 1]).v3;
                    else if (img_i >= img_height)
                        cr_block[n][ii][jj] = rgb2ycc(origin[img_height - 1][img_j]).v3;
                    else if (img_j >= img_width)
                        cr_block[n][ii][jj] = rgb2ycc(origin[img_i][img_width - 1]).v3;
                }
            }
            n++;
        }
    }

    void JpegEncoder::dctAndQuan()
    {
        int i;
        switch (quality)
        {
        case 0:
            lum_quan = lum_quant0;
            croma_quan = croma_quant0;
            break;
        case 1:
            lum_quan = lum_quant1;
            croma_quan = croma_quant1;
            break;
        case 2:
            lum_quan = lum_quant2;
            croma_quan = croma_quant2;
            break;
        case 5:
            lum_quan = lum_quant5;
            croma_quan = croma_quant5;
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

    void JpegEncoder::zigzag()
    {
        int i;
        y_zigzag = new int*[y_block_count];
        for (i = 0; i < y_block_count; i++)
        {
            y_zigzag[i] = zigzagTransform(y_block[i]);
        }

        cr_zigzag = new int *[c_block_count];
        cb_zigzag = new int *[c_block_count];
        for (i = 0; i < c_block_count; i++)
        {
            cr_zigzag[i] = zigzagTransform(cr_block[i]);
            cb_zigzag[i] = zigzagTransform(cb_block[i]);
        }
        
    }

    void JpegEncoder::deltaEncoding()
    {
        int y_previous = y_zigzag[0][0];
        for (int i = 1; i < y_block_count; i++)
        {
            int y_current = y_zigzag[i][0];
            int y_delta = y_current - y_previous;
            if (y_delta > 32767 || y_delta < -32767) 
                y_delta > 0 ? y_delta = 32767 : y_delta = -32767;
            y_previous = y_zigzag[i][0];
            y_zigzag[i][0] = y_delta;
        }

        int cr_previous = cr_zigzag[0][0];
        int cb_previous = cb_zigzag[0][0];
        for (int i = 1; i < c_block_count; i++)
        {
            int cr_current = cr_zigzag[i][0];
            int cr_delta = cr_current - cr_previous;
            if (cr_delta > 32767 || cr_delta < -32767) 
                cr_delta > 0 ? cr_delta = 32767 : cr_delta = -32767;
            cr_previous = cr_zigzag[i][0];
            cr_zigzag[i][0] = cr_delta;

            int cb_current = cb_zigzag[i][0];
            int cb_delta = cb_current - cb_previous;
            if (cb_delta > 32767 || cb_delta < -32767) 
                cb_delta > 0 ? cb_delta = 32767 : cb_delta = -32767;
            cb_previous = cb_zigzag[i][0];
            cb_zigzag[i][0] = cb_delta;
        }
    }

    void JpegEncoder::RLEAddPair(int zero_count, int n, vector<vector<int>> &ac)
    {
        vector<int> pair;
        pair.push_back(zero_count);
        n = n > 128 ? 128 : n;
        n = n < -128 ? -128 : n;
        pair.push_back(n);
        ac.push_back(pair);
    }

    void JpegEncoder::RLE(int **zigzag, int block_count, vector<vector<int>> &ac)
    {
        for (int i = 0; i < block_count; i++)
        {
            int zero_count = 0;
            int zero_pair = 0;
            for (int j = 1; j < 64; j++)
            {
                if (zigzag[i][j] == 0)
                {
                    if (j == 63)
                    {
                        RLEAddPair(0, 0, ac);
                        continue;
                    }
                    zero_count++;
                    if (zero_count == 16)
                    {
                        zero_pair++;
                        zero_count = 0;
                    }
                }
                else
                {
                    if (zero_pair != 0)
                    {
                        for (int k = 0; k < zero_pair; k++)
                            RLEAddPair(15, 0, ac);
                        zero_pair = 0;
                    }
                    RLEAddPair(zero_count, zigzag[i][j], ac);
                    zero_count = 0;
                }
            }
        }
    }

    void JpegEncoder::encodeImage(Pixel **matrix, int height, int width)
    {
        origin = matrix;
        img_height = height;
        img_width = width;

        subsample();
        dctAndQuan();
        //for (int i = 0; i < 8; i++)
        //for (int j = 0; j < 8; j++)
        //       cout << y_block[0][i][j] << ' ';
        zigzag();
        for (int i = 0; i < 64; i++)
            cout << y_zigzag[0][i] << ' ';
        deltaEncoding();
        RLE(y_zigzag, y_block_count, y_ac);
        RLE(cr_zigzag, c_block_count, cr_ac);
        RLE(cb_zigzag, c_block_count, cb_ac);
        cout << '\n';
    }

    void JpegEncoder::computeHuffmanTable(BYTE *bits, BYTE *value, BitString *table)
    {
        BYTE n = 0;
        WORD huffman_code = 0;

        for (int i = 1; i < 16; i++)
        {
            for (int j = 0; j < bits[i]; j++)
            {
                table[value[n]].code = huffman_code;
                table[value[n]].length = i;
                n++;
                huffman_code++;
            }
            huffman_code <<= 1;
        }
    }

    void JpegEncoder::initHuffmanTable()
    {
        memset(dc_lum_table, 0, sizeof(dc_lum_table));
        computeHuffmanTable(dc_lum_bits, dc_lum_val, dc_lum_table);

        memset(dc_chr_table, 0, sizeof(dc_chr_table));
        computeHuffmanTable(dc_chr_bits, dc_chr_val, dc_chr_table);
        
        memset(ac_lum_table, 0, sizeof(ac_lum_table));
        computeHuffmanTable(ac_lum_bits, ac_lum_val, ac_lum_table);

        memset(ac_chr_table, 0, sizeof(ac_chr_table));
        computeHuffmanTable(ac_chr_bits, ac_chr_val, ac_chr_table);
    }

    void JpegEncoder::writeWord(WORD code, int length)
    {
        unsigned short mask[] = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768 };
        int code_pos = length - 1;

        while (code_pos >= 0)
        {
            if ((code & mask[code_pos]) != 0)
            {
                newByte = newByte | mask[newBytePos];
            }
            code_pos--;
            newBytePos--;
            if (newBytePos < 0)
            {
                out.write((char*)&newByte, 1);
                if (newByte == 0xff)
                    out.write("\x00", 2);

                newBytePos = 7;
                newByte = 0;
            }
        }
    }

    void JpegEncoder::writeBlock(int dc, std::vector<std::vector<int>>&ac, int &ac_index, bool isY)
    {
        BitString *ac_huffman, *dc_huffman;
        if (isY)
        {
            dc_huffman = dc_lum_table;
            ac_huffman = ac_lum_table;
        }
        else  
        {
            dc_huffman = dc_chr_table;
            ac_huffman = ac_chr_table;
        }

        WORD value = numberEncoding(dc);
        int length = numberOfSetBits(abs(dc));
        if (value == 0 && length == 1)
        {
            BitString code = dc_huffman[0];
            writeWord(code.code, code.length);
        }
        else
        {
            BitString code = dc_huffman[length];
            writeWord(code.code, code.length);
            writeWord(value, length);
        }
        
        int ac_amount = 0;
        while (ac_amount < 63)
        {
            int zero_amount = ac[ac_index][0];
            value = numberEncoding(ac[ac_index][1]);
            length = numberOfSetBits(abs(ac[ac_index][1]));
            if (zero_amount == 0 && value == 0 && length == 1)
            {
                ac_amount = 63;
                length = 0;
            }

            WORD rleCode = (zero_amount | 0xf) << 4 + (length | 0xf);
            if (rleCode == 0)
            {
                BitString code = ac_huffman[rleCode];
                writeWord(code.code, code.length);
            }
            else
            {
                BitString code = ac_huffman[rleCode];
                writeWord(code.code, code.length);
                writeWord(value, length);
            }
            ac_index++;
        }
    }

    void JpegEncoder::writeImageData()
    {
        initHuffmanTable();

        //for (int i = 0; i < cb_ac.size(); i++)
        //    std::cout << cb_ac[i][0] << ' ' << cb_ac[i][1] << ' ' << '\n';
        int y_n = 0, c_n = 0;
        int y_ac_index = 0,
            cb_ac_index = 0,
            cr_ac_index = 0;
        for (int i = 0; i < mcu_ver_count; i++)
        for (int j = 0; j < mcu_hor_count; j++)
        {
            if (chroma_subsampling == "4:4:4")
            {
                c_n = y_n = i * mcu_hor_count + j;
                writeBlock(y_zigzag[y_n][0], y_ac, y_ac_index, true);
                writeBlock(cb_zigzag[c_n][0], cb_ac, cb_ac_index, false);
                writeBlock(cr_zigzag[c_n][0], cr_ac, cr_ac_index, false);
            }
            if (chroma_subsampling == "4:2:2")
            {
                y_n = i * (mcu_hor_count * 2) + (j * 2);
                c_n = i * mcu_hor_count + j;
                writeBlock(y_zigzag[y_n][0], y_ac, y_ac_index, true);
                y_n += 1;
                writeBlock(y_zigzag[y_n][0], y_ac, y_ac_index, true);

                writeBlock(cb_zigzag[c_n][0], cb_ac, cb_ac_index, false);
                writeBlock(cr_zigzag[c_n][0], cr_ac, cr_ac_index, false);
            }
            if (chroma_subsampling == "4:2:0")
            {
                y_n = (i * 2) * (mcu_hor_count * 2) + (j * 2);
                c_n = i * mcu_hor_count + j;
                writeBlock(y_zigzag[y_n][0], y_ac, y_ac_index, true);
                y_n += 1;
                writeBlock(y_zigzag[y_n][0], y_ac, y_ac_index, true);
                y_n = y_n + (mcu_hor_count * 2) - 1;
                writeBlock(y_zigzag[y_n][0], y_ac, y_ac_index, true);
                y_n += 1;
                writeBlock(y_zigzag[y_n][0], y_ac, y_ac_index, true);

                writeBlock(cb_zigzag[c_n][0], cb_ac, cb_ac_index, false);
                writeBlock(cr_zigzag[c_n][0], cr_ac, cr_ac_index, false);
            }
        }
    }

    void JpegEncoder::writeJpg(std::string file)
    {
        out.open(file, std::ios::out | std::ios::binary);

        //SOI
        out.write((char*)SOI, sizeof(SOI));
        makeAPP0();
        makeDQT(0, lum_quan);
        makeDQT(1, croma_quan);
        makeSOF0();
        makeDHT(0x00, dc_lum_bits, dc_lum_val, DC_LUM_CODES);
        makeDHT(0x10, ac_lum_bits, ac_lum_val, AC_LUM_CODES);
        makeDHT(0x01, dc_chr_bits, dc_chr_val, DC_LUM_CODES);
        makeDHT(0x11, ac_chr_bits, ac_chr_val, AC_LUM_CODES);
        makeSOS();

        writeImageData();

        if (newBytePos != 7)
            out.write((char*)&newByte, 1);
        out.write((char*)&EOI, 2);
        out.close();
    }

    void JpegEncoder::makeAPP0() 
    {
        APP0 app0;
        app0.name[0] = 0xff; app0.name[1] = 0xe0;
        app0.length[0] = 0x00, app0.length[1] = 16;
        app0.symbol[0] = 0x4A, app0.symbol[1] = 0x46, app0.symbol[2] = 0x49, app0.symbol[3] = 0x46, app0.symbol[4] = 0x00;
        app0.version[0] = 0x01, app0.version[1] = 0x02;
        app0.density_x[0] = 0x00, app0.density_x[1] = 0x48;
        app0.density_y[0] = 0x00, app0.density_y[1] = 0x48;
        out.write((char*)&app0, sizeof(app0));
    }

    void JpegEncoder::makeDQT(int id, BYTE *table)
    {
        DQT dqt;
        dqt.name[0] = 0xff, dqt.name[1] = 0xdb;
        dqt.length[0] = 0; dqt.length[1] = 67;
        dqt.info = id;
        for (int i = 0; i < 64; i++)
        {
            dqt.table[i] = table[i];
        }
        out.write((char*)&dqt, sizeof(dqt));
    }

    void JpegEncoder::makeSOF0() 
    {
        SOF0 sof0;
        sof0.name[0] = 0xff; sof0.name[1] = 0xc0;
        sof0.length[0] = 0x00; sof0.length[1] = 0x11;
        int length = (sof0.length[0] << 8) + sof0.length[1];
        sof0.height[0] = (img_height & 0xff00) >> 8;
        sof0.height[1] = img_height & 0x00ff;
        sof0.width[0] = (img_width & 0xff00) >> 8;
        sof0.width[1] = img_width & 0x00ff;
        sof0.colorinfo[0].id = 0x01;
        sof0.colorinfo[0].sampling = 0x11;
        sof0.colorinfo[0].tableid = 0x00;
        sof0.colorinfo[1].id = 0x02;
        sof0.colorinfo[1].tableid = 0x01;
        sof0.colorinfo[2].id = 0x03;
        sof0.colorinfo[2].tableid = 0x01;
        sof0.colorinfo[1].sampling = sof0.colorinfo[2].sampling = 0x11;
        if (chroma_subsampling == "4:4:4")
            sof0.colorinfo[0].sampling = 0x11;
        if (chroma_subsampling == "4:2:2")
            sof0.colorinfo[0].sampling = 0x21;
        if (chroma_subsampling == "4:2:0")
            sof0.colorinfo[0].sampling = 0x22;
        
        out.write((char*)&sof0, length + 2);
    }

    void JpegEncoder::makeDHT(BYTE id, BYTE *bits, BYTE *vals, int len)
    {
        BYTE l[2];
        WORD length;
        this->out.write("\xff\xc4", 2);
        length = 2 + 1 + 16 + len;
        l[0] = (length & 0xff00) >> 8;
        l[1] = length & 0x00ff;
        this->out.write((char*)l, 2);
        this->out.write((char*)&id, 1);
        this->out.write((char*)&bits[1], 16);
        this->out.write((char*)vals, len);
    }

    void JpegEncoder::makeSOS()
    {
        SOS sos;
        sos.name[0] = 0xff, sos.name[1] = 0xda;
        sos.length[0] = 0x00, sos.length[1] = 12;
        int length = (sos.length[0] << 8) + sos.length[1];
        sos.color = 3;
        sos.color_huffinfo[0] = 0x01;
        sos.color_huffinfo[1] = 0x00;
        sos.color_huffinfo[2] = 0x02;
        sos.color_huffinfo[3] = 0x11;
        sos.color_huffinfo[4] = 0x03;
        sos.color_huffinfo[5] = 0x11;
        sos.data[0] = 0x00; sos.data[1] = 0x3f; sos.data[2] = 0x00;
        out.write((char*)&sos, length + 2);
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