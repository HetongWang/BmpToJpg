#ifndef __JPEG__
#define __JPEG__

#define SIZE(array) (sizeof((array))/sizeof((array[0])))
typedef unsigned char BYTE;
typedef unsigned short WORD;

namespace jpeg 
{
    static BYTE lum_quant0[64] = { 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 };
    static BYTE croma_quant0[64] = { 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 };
    static BYTE lum_quant1[64] = { 1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,3 ,1 ,1 ,1 ,1 ,2 ,2 ,3 ,3 ,1 ,1 ,1 ,2 ,2 ,3 ,3 ,3 ,1 ,1 ,2 ,2 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,3 ,3 ,3 ,3 ,3 };
    static BYTE croma_quant1[64] = { 1 ,1 ,1 ,2 ,3 ,3 ,3 ,3 ,1 ,1 ,1 ,2 ,3 ,3 ,3 ,3 ,1 ,1 ,2 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 };
    static BYTE lum_quant2[64] = { 16 ,11 ,10 ,16 ,24 ,40 ,51 ,61 ,12 ,12 ,14 ,19 ,26 ,58 ,60 ,55 ,14 ,13 ,16 ,24 ,40 ,57 ,69 ,56 ,14 ,17 ,22 ,29 ,51 ,87 ,80 ,62 ,18 ,22 ,37 ,56 ,68 ,109 ,103 ,77 ,24 ,35 ,55 ,64 ,81 ,104 ,113 ,92 ,49 ,64 ,78 ,87 ,103 ,121 ,120 ,101 ,72 ,92 ,95 ,98 ,112 ,100 ,103 ,99 };
    static BYTE croma_quant2[64] = { 17 ,18 ,24 ,47 ,99 ,99 ,99 ,99 ,18 ,21 ,26 ,66 ,99 ,99 ,99 ,99 ,24 ,26 ,56 ,99 ,99 ,99 ,99 ,99 ,47 ,66 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 ,99 };

    const int DC_LUM_CODES = 12;
    const int AC_LUM_CODES = 162;
    static BYTE dc_lum_bits[17] = { 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
    static BYTE dc_lum_val[DC_LUM_CODES] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static BYTE dc_chr_bits[17] = { 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
    static BYTE dc_chr_val[DC_LUM_CODES] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    static BYTE ac_lum_bits[17] = { 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
    static BYTE ac_lum_val[AC_LUM_CODES] = {
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
    };
    static BYTE ac_chr_bits[17] = { 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
    static BYTE ac_chr_val[AC_LUM_CODES] =
    {
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
        0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
        0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
        0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
        0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
        0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
    };

    // Jpeg markers defination
    static BYTE SOI[2] = { 0xff, 0xd8 };
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
    static BYTE EOI[2] = { 0xff, 0xd9 };

    struct Pixel
    {
        double v1, v2, v3;
 
        Pixel() {}
        Pixel(unsigned char val[3]);
        Pixel(double r, double g, double b);
        Pixel operator+(Pixel &rgb);
        Pixel operator/(double x);
    };

    Pixel rgb2ycc(Pixel pixel);
    Pixel ycc2rgb(Pixel pixel);

    struct BitString
    {
        WORD code, length;
    };
}

#endif // !__JPEG__
