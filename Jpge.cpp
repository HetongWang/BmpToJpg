#include "Jpge.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
using std::cout;

namespace jpge {

// Jpeg markers defination
    BYTE SOI[2] = { 0xff, 0xd8 };
struct APP0 {
    BYTE name[2];
    BYTE length[2];
    BYTE symbol[5];
    BYTE version[2];
    BYTE density = 0x02;
    BYTE density_x[2];
    BYTE density_y[2];
    BYTE thum_x = 0;
    BYTE thum_y = 0;
};

struct DQT {
    BYTE name[2];
    BYTE length[2];
    BYTE info;
    BYTE table[64];
};

struct colorInfo {
    BYTE id;
    BYTE sampling = 0x11;
    BYTE tableid;
};

struct SOF0 {
    BYTE name[2];
    BYTE length[2];
    BYTE accuracy = 8;
    BYTE height[2];
    BYTE width[2];
    BYTE color = 3;
    colorInfo colorinfo[3];
};

struct DHT {
    BYTE name[2];
    BYTE length[2];
    BYTE id;
    BYTE count[16];
};

struct SOS {
    BYTE name[2];
    BYTE length[2];
    BYTE color = 3;
    BYTE color_huffinfo[6];
    BYTE data[3];
};
BYTE EOI[2] = { 0xff, 0xd9 };

const int MAX_HUFF_SYMBOLS = 257;
const int MAX_BITS = 17;
const int DC_LUM_CODES = 12;
const int AC_LUM_CODES = 162;
static BYTE lum_quant[64] = { 16, 11, 12, 14, 12, 10, 16, 14, 13, 14, 18, 17, 16, 19, 24, 40, 26, 24, 22, 22, 24, 49, 35, 37, 29, 40, 58, 51, 61, 60, 57, 51, 56, 55, 64, 72, 92, 78, 64, 68, 87, 69, 55, 56, 80, 109, 81, 87, 95, 98, 103, 104, 103, 62, 77, 113, 121, 112, 100, 120, 92, 101, 103, 99 };
static BYTE croma_quant[64] = { 17, 18, 18, 24, 21, 24, 47, 26, 26, 47, 99, 66, 56, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 };
static BYTE zigzag_table[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };
static BYTE dc_lum_bits[17] = { 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
static BYTE dc_lum_val[DC_LUM_CODES] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static BYTE ac_lum_bits[17] = { 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
static BYTE ac_lum_val[AC_LUM_CODES] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa
};

float C(int u)
{
    if (u == 0)
        return (1.0 / sqrt(2.0));
    else
        return 1.0;
}

void dct(float block[8][8])
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

void quantize(float block[8][8], BYTE q[64], WORD res[64]) {
    int a, x, y;
    for (int i = 0; i < 64; i++) {
        x = zigzag_table[i] / 8;
        y = i - x * 8;
        a = block[x][y] / q[i];
        if (a > 65535 || a < -65536) a>0 ? a = 65535 : a = -65536;
        res[i] = a;
    }
}

YCCPixel rgb2ycc(RGBPixel pixel) {
    YCCPixel res;
    res.y = 0 + (0.299     * pixel.r) + (0.587     * pixel.g) + (0.114     * pixel.b);
    res.cb = 128 - (0.168736  * pixel.r) - (0.331264  * pixel.g) + (0.5       * pixel.b);
    res.cr = 128 + (0.5       * pixel.r) - (0.418688  * pixel.g) - (0.081312  * pixel.b);
    return res;
}

WORD** JpgImage::compress(RGBPixel block[8][8], int count) {
    int i, j, k;
    YCCPixel y_block[8][8];
    float y[8][8], cr[8][8], cb[8][8];
    WORD **zigzag;

    zigzag = new WORD*[3];
    for (j = 0; j < 3; j++)
        zigzag[j] = new WORD[64];

    if (count == 109)
        i = 0;

    // RGB to YCrCb
    for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++) {
        y_block[i][j] = rgb2ycc(block[i][j]);
        y[i][j] = y_block[i][j].y;
        cb[i][j] = y_block[i][j].cb;
        cr[i][j] = y_block[i][j].cr;
    }

    dct(y); dct(cr); dct(cb);
    quantize(y, lum_quant, zigzag[0]);
    quantize(cr, croma_quant, zigzag[1]);
    quantize(cb, croma_quant, zigzag[2]);

    this->last_dc[0] = zigzag[0][0];
    this->last_dc[1] = zigzag[1][0];
    this->last_dc[2] = zigzag[2][0];

    if (count != 0) {
        zigzag[0][0] = zigzag[0][0] - this->last_dc[0];
        zigzag[1][0] = zigzag[1][0] - this->last_dc[1];
        zigzag[2][0] = zigzag[2][0] - this->last_dc[2];
    }

    return zigzag;
}

void Huffman_table::compute() {
    int last_p, si;
    BYTE huff_size[257];
    unsigned int huff_code[257];
    unsigned int code;

    int p = 0;
    for (char l = 1; l <= 16; l++)
    for (int i = 1; i <= this->bits[l]; i++)
        huff_size[p++] = l;

    huff_size[p] = 0; last_p = p; // write sentinel

    code = 0; si = huff_size[0]; p = 0;

    while (huff_size[p]) {
        while (huff_size[p] == si)
            huff_code[p++] = code++;
        code <<= 1;
        si++;
    }

    memset(this->codes, 0, sizeof(this->codes[0]) * 256);
    memset(this->code_size, 0, sizeof(this->code_size[0]) * 256);
    for (p = 0; p < last_p; p++) {
        this->codes[this->val[p]] = huff_code[p];
        this->code_size[this->val[p]] = huff_size[p];
    }
}
inline void swapHuffNode(heap_node &a, heap_node &b) {
    heap_node temp;
    temp = a; a = b; b = temp;
}

heap_node deleteMin(heap_node node[MAX_HUFF_SYMBOLS], int &n) {
    heap_node min = node[1], last = node[n--];
    int i, child;

    for (i = 1; i < n; i = child) {
        node[i * 2].count < node[i * 2 + 1].count ? child = i * 2 : child = i * 2 + 1;
        if (last.count > node[child].count)
            node[i] = node[child];
        else
            break;
    }
    node[i] = last;
    return min;
}

Huffman_node::Huffman_node(int val) {
    this->left_child = NULL;
    this->right_child = NULL;
    this->depth = 2;
    this->val = val;
}

Huffman_node::Huffman_node() {
    this->left_child = NULL;
    this->right_child = NULL;
    this->val = -1;
}

Huffman_node::~Huffman_node() {
    if (this->left_child) delete this->left_child;
    if (this->right_child) delete this->right_child;
}

void Huffman_table::getBinary(Huffman_node *node, int code, int code_size) {
    node->code = (node->code << 1) + code;

    if (node->val > 0) {
        this->codes[node->val] = node->code;
        this->code_size[node->val] = code_size;
    }
    else {
        if (node->left_child) {
            node->left_child->code = node->code;
            getBinary(node->left_child, 0, code_size + 1);
        }
        if (node->right_child) {
            node->right_child->code = node->code;
            getBinary(node->right_child, 1, code_size + 1);
        }
    }
}

void Huffman_table::optimize(int table_len) {
    int i, j, k, n = 0, remain;
    heap_node node[MAX_HUFF_SYMBOLS], min1, min2;

    node[0].count = 0;
    for (i = 0; i < table_len; i++)
    if (this->count[i]) {
        node[++n].count = this->count[i];
        node[n].huff = new Huffman_node(i);
    }

    // build priority queue
    for (i = n / 2; i > 0; i--) {
        if (node[i].count > node[i * 2 ].count) swapHuffNode(node[i], node[i * 2]);
        if (node[i].count > node[i * 2 + 1].count) swapHuffNode(node[i], node[i * 2 + 1]);
    }

    // build huffman tree
    remain = n;
    while (remain-- > 1) {
        min1 = deleteMin(node, n);
        min2 = deleteMin(node, n);
        node[++n].count = min1.count + min2.count;
        node[n].huff = new Huffman_node;
        if (min1.huff->depth < min2.huff->depth) {
            node[n].huff->left_child = min1.huff;
            node[n].huff->right_child = min2.huff;
            node[n].huff->depth = min1.huff->depth + 1;
        }
        else {
            node[n].huff->left_child = min2.huff;
            node[n].huff->right_child = min1.huff;
            node[n].huff->depth = min2.huff->depth + 1;
        }

        // insert node
        for (i = n; node[i / 2].count > node[i].count; i /= 2)
            swapHuffNode(node[i], node[i / 2]);
    }
    // thus node[1] contain the whole huffman tree

    // search the huffman tree and store its binary data 
    node[1].huff->code = 0;
    this->getBinary(node[1].huff, 0, 1);
    delete node[1].huff;
}

void JpgImage::generateAcHuffman() {
    int i, j, k, run_len, nbits, temp;
    int *ac_count = this->huff_ac.count;
    memset(ac_count, 0, sizeof(ac_count[0]) * 256);

    nbits = 0;
    for (i = 0; i < this->block_size; i++)
    for (j = 0; j < 3; j++) {
        for (run_len = 0, k = 1; k < 64; k++) {
            temp = this->zigzag_res[i][j][k];
            if (temp == 0) 
                run_len++;
            else {
                while (run_len >= 16) {
                    ac_count[0xf0]++;
                    run_len -= 16;
                }
                nbits = 1;
                while (temp >>= 1) nbits++;
                ac_count[(run_len << 8) + nbits]++;
                run_len = 0;
            }
        }
    }
    if (run_len) ac_count[0]++;
    //this->huff_ac.optimize(256);

    memcpy(this->huff_ac.bits, ac_lum_bits, 17);
    memcpy(this->huff_ac.val, ac_lum_val, 256);
    this->huff_ac.compute();
}

void JpgImage::generateDcHuffman() {
    int i, j, k, nbits;
    int *dc_count = this->huff_dc.count;
    memset(dc_count, 0, sizeof(dc_count[0])* 256);

    for (i = 0; i < this->block_size; i++)
    for (j = 0; j < 3; j++) {
        k = this->zigzag_res[i][j][0];
        nbits = 0;  // there is possible that dc coffient is equal to zero
        while (k) {
            nbits++;
            k >>= 1;
        }
        dc_count[nbits]++;
    }

    // I can't write it by myself in such limited time.. So I use standard huffman table.. 
    //this->huff_dc.optimize(MAX_BITS);
    memcpy(this->huff_dc.bits, dc_lum_bits, 17);
    memcpy(this->huff_dc.val, dc_lum_val, 12);
    this->huff_dc.compute();
}

void JpgImage::makeSOI() {
    this->out.write((char*)SOI, 2);
}

void JpgImage::makeAPP0() {
    APP0 app0;
    app0.name[0] = 0xff; app0.name[1] = 0xe0;
    app0.length[0] = 0x00, app0.length[1] = 16;
    app0.symbol[0] = 0x4A, app0.symbol[1] = 0x46, app0.symbol[2] = 0x49, app0.symbol[3] = 0x46, app0.symbol[4] = 0x00;
    app0.version[0] = 0x01, app0.version[1] = 0x02;
    app0.density_x[0] = 0x00, app0.density_x[1] = 0x60;
    app0.density_y[0] = 0x00, app0.density_y[1] = 0x60;
    int length = (app0.length[0] << 8) + app0.length[1];
    this->out.write((char*)&app0, length + 2);
}

void JpgImage::makeDQT() {
    DQT dqt;
    WORD length;
    dqt.name[0] = 0xff, dqt.name[1] = 0xdb;
    dqt.info = 0x00;
    memcpy(dqt.table, lum_quant, 64);
    length = sizeof(dqt.info) + sizeof(dqt.table) + 2;
    dqt.length[0] = (length & 0xff00) >> 8;
    dqt.length[1] = length & 0x00ff;

    this->out.write((char*)&dqt, length + 2);

    dqt.info = 0x01;
    memcpy(dqt.table, croma_quant, 64);
    this->out.write((char*)&dqt, length + 2);
}

void JpgImage::makeSOF0() {
    SOF0 sof0;
    sof0.name[0] = 0xff, sof0.name[1] = 0xc0;
    sof0.length[0] = 0x00, sof0.length[1] = 0x11;
    int length = (sof0.length[0] << 8) + sof0.length[1];
    sof0.height[0] = (this->height & 0xff00) >> 8;
    sof0.height[1] = this->height & 0x00ff;
    sof0.width[0] = (this->width & 0xff00) >> 8;
    sof0.width[1] = this->width & 0x00ff;
    sof0.colorinfo[0].id = 0x01;
    sof0.colorinfo[0].sampling = sof0.colorinfo[1].sampling = sof0.colorinfo[2].sampling = 0x11;
    sof0.colorinfo[0].tableid = 0x00;
    sof0.colorinfo[1].id = 0x02;
    sof0.colorinfo[1].tableid = sof0.colorinfo[2].tableid = 0x01;
    sof0.colorinfo[2].id = 0x03;
    this->out.write((char*)&sof0, length + 2);
}

void JpgImage::makeDHT() {
    BYTE len[2];
    WORD length;
    this->out.write("\xff\xc4", 2);
    length = 2 + 1 + 17 + DC_LUM_CODES;
    len[0] = (length & 0xff00) >> 8;
    len[1] = length & 0x00ff;
    this->out.write((char*)len, 2);
    this->out.write("\x00" ,1);
    this->out.write((char*)this->huff_dc.bits, MAX_BITS);
    this->out.write((char*)dc_lum_val, DC_LUM_CODES);

    this->out.write("\xff\xc4", 2);
    length = 2 + 1 + 17 + AC_LUM_CODES;
    len[0] = (length & 0xff00) >> 8;
    len[1] = length & 0x00ff;
    this->out.write((char*)len, 2);
    this->out.write("\x10", 1);
    this->out.write((char*)this->huff_ac.bits, MAX_BITS);
    this->out.write((char*)ac_lum_val, AC_LUM_CODES);
}

void JpgImage::makeSOS() {
    SOS sos;
    sos.length[0] = 0x00, sos.length[1] = 12;
    int length = (sos.length[0] << 8) + sos.length[1];
    sos.name[0] = 0xff, sos.name[1] = 0xda;
    sos.color_huffinfo[0] = 0x01;
    sos.color_huffinfo[2] = 0x02;
    sos.color_huffinfo[4] = 0x03;
    sos.color_huffinfo[1] = 0x00;
    sos.color_huffinfo[3] = 0x10;
    sos.color_huffinfo[5] = 0x10;
    sos.data[0] = 0x00; sos.data[1] = 0x3f; sos.data[2] = 0x00;
    this->out.write((char*)&sos, length + 2);
}

void JpgImage::makeEOI() {
    this->out.write((char*)&EOI, 2);
    this->out.close();
}

void JpgImage::makeMarker() {
    this->makeSOI();
    this->makeAPP0();
    this->makeDQT();
    this->makeSOF0();
    this->makeDHT();
    this->makeSOS();
}

void JpgImage::writeLastByte(BYTE &buff, int &buff_bits) {
    //for (int i = 0; i < 8 - buff_bits; i++) {
    //    buff <<= 1;
    //    buff += 1;
    //}
    buff <<= (8 - buff_bits);
    this->out.write((char*)&buff, 1);
    buff = 0;
    buff_bits = 0;
}

void JpgImage::writeData(BYTE &buff, int &buff_bits) {
    this->out.write((char*)&buff, 1);
    if (buff == 0xFF) this->out.write("\x00", 1);
    buff = 0;
    buff_bits = 0;
}

void JpgImage::writeCode(unsigned int code, int code_size, BYTE &buff, int &buff_bits) {
    BYTE b = 0;
    unsigned int temp;
    code <<= (32 - code_size);
    if (code_size == 0) code_size++;
    while (code_size) {
        temp = code & 0x80000000;
        if (temp != 0) b = 1; else b = 0;
        code <<= 1; code_size--;
        buff <<= 1;
        buff = buff + b;
        buff_bits++;
        if (buff_bits == 8) this->writeData(buff, buff_bits);
    }
}

void JpgImage::dcEncode(WORD dc, BYTE &buff, int &buff_bits) {
    int nbits = 0, code_size, temp = dc;
    unsigned int code;
    while (temp) {
        nbits++; 
        temp >>= 1;
    }
    code = this->huff_dc.codes[nbits];
    code_size = this->huff_dc.code_size[nbits];
    writeCode(code, code_size, buff, buff_bits);

    if (dc != 0) writeCode(dc, nbits, buff, buff_bits);
}

void JpgImage::acEncode(WORD *ac, BYTE &buff, int &buff_bits) {
    int i, j, k, run_len, code_size, temp, nbits;
    unsigned int code;
    for (run_len = 0, k = 1; k < 64; k++) {
        temp = ac[k];
        if (temp == 0)
            run_len++;
        else {
            while (run_len >= 16) {
                code = this->huff_ac.codes[0xf0];
                code_size = this->huff_ac.code_size[0xf0];
                this->writeCode(code, code_size, buff, buff_bits);

                code = this->huff_ac.codes[0];
                code_size = this->huff_ac.code_size[0];
                this->writeCode(code, code_size, buff, buff_bits);
                run_len -= 16;
            }
            nbits = 1;
            while (temp >>= 1) nbits++;
            i = (run_len << 8) + nbits;
            code = this->huff_ac.codes[i];
            code_size = this->huff_ac.code_size[i];
            this->writeCode(code, code_size, buff, buff_bits);

            code = this->huff_ac.codes[ac[k]];
            code_size = this->huff_ac.code_size[ac[k]];
            this->writeCode(code, code_size, buff, buff_bits);
            run_len = 0;
        }
    }
    if (run_len) {
        code = this->huff_ac.codes[0];
        code_size = this->huff_ac.code_size[0];
        this->writeCode(code, code_size, buff, buff_bits);
    }
}

void JpgImage::entropyCode() {
    BYTE buff = 0;
    int i, j, k, buff_size = 0;
    for (i = 0; i < this->block_size; i++)
    for (j = 0; j < 3; j++) {
        this->dcEncode(this->zigzag_res[i][j][0], buff, buff_size);
        this->acEncode(this->zigzag_res[i][j], buff, buff_size);
    }
    if (buff_size) this->writeLastByte(buff, buff_size);
}

JpgImage::JpgImage(RGBPixel **matrix, int height, int width) {
    int x_8, y_8, i, j, ii, jj, n = 0;
    RGBPixel block[8][8];

    this->height = height;
    this->width = width;

    x_8 = this->width / 8; if (this->width % 8 != 0) x_8++;
    y_8 = this->height / 8; if (this->height % 8 != 0) y_8++;
    this->block_size = x_8 * y_8;

    this->zigzag_res = new WORD**[this->block_size];

    for (i = 0; i < x_8; i++)
    for (j = 0; j < y_8; j++) {
        for (ii = 0; ii < 8; ii++)
        for (jj = 0; jj < 8; jj++)
        if (i * 8 + ii < height&&j * 8 + jj < width)
            block[ii][jj] = matrix[i * 8 + ii][j * 8 + jj];
        else {
            block[ii][jj].b = 0;
            block[ii][jj].r = 0;
            block[ii][jj].g = 0;
        }

        this->zigzag_res[n] = this->compress(block, n);
        n++;
    }
}

void JpgImage::writeJPG(std::string file) {
    this->out.open(file, std::ios::out | std::ios::binary);

    this->generateDcHuffman();
    this->generateAcHuffman();

    this->makeMarker();
    this->entropyCode();
    //out.write("\x00", 1);
    this->makeEOI();
    out.close();
}

}; // jpge namespace