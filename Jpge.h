#ifndef JPGIMAGE_H
#define JPGIMAGE_H

#include "BmpImage.h"
#include <fstream>

namespace jpge {

struct YCCPixel {
    float y, cb, cr;
};

// encoder function
YCCPixel rgb2ycc(RGBPixel);
void dct(float block[8][8]);
void quantize(float block[8][8], BYTE q[64], WORD res[64]);

class Huffman_node {
public:
    int val;
    int code;
    int depth;
    Huffman_node *left_child;
    Huffman_node *right_child;

    Huffman_node(int val);
    Huffman_node();
    ~Huffman_node();
};

struct heap_node {
    int count;
    Huffman_node *huff;
};

class Huffman_table {
public:
    int codes[256];
    BYTE code_size[256];
    BYTE bits[17];
    BYTE val[256];
    int count[256];

    void optimize(int table_len);
    void getBinary(Huffman_node *node, int code, int code_size);
    void compute(); // compute JFIF standard form
};

class JpgImage
{
public:
    void writeJPG(std::string);
    JpgImage(RGBPixel **matrix, int height, int width);
    ~JpgImage();
private:
    std::ofstream out;
    int width;
    int height;
    int block_size;
    WORD last_dc[3];
    WORD ***zigzag_res;
    Huffman_table huff_dc, huff_ac;

    WORD** compress(RGBPixel block[8][8], int count);
    void generateAcHuffman();
    void generateDcHuffman();
    void entropyCode();
    void dcEncode(WORD dc, BYTE &buff, int &buff_bits);
    void acEncode(WORD *ac, BYTE &buff, int &buff_bits);
    void writeCode(unsigned int code, int code_size, BYTE &buff, int &buff_bits);
    void writeData(BYTE &buff, int &buff_bits);
    void writeLastByte(BYTE &buff, int &buff_size);

    void makeMarker();
    void makeSOI();
    void makeAPP0();
    void makeDQT();
    void makeSOF0();
    void makeDHT();
    void makeSOS();
    void makeEOI();

};

}; // jpge namespace

#endif