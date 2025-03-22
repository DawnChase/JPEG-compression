// all the headers are included here

#ifndef HEAD_H
#define HEAD_H

#include <bits/stdc++.h>
using namespace std;

#pragma pack(2) 
struct BMPFileHeader {
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserve1;
	unsigned short bfReserve2;
	unsigned int bfOffBits;
};

struct BMPInfoHeader {
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
};

struct DHTSegment {
    unsigned short marker;  // 0xFFC4
    unsigned int length;  // 2 + 4 + 1 + 16 + sum(BITS[i]) * sizeof(int) 
    unsigned char HTInformation;    // 00: L_DC, 01: L_AC, 10: C_DC, 11: C_AC
    unsigned char BITS[16]; // 码长为 i + 1 的码字数目
    int *HUFFVAL;
};

const int Qtable[2][8][8] = {
    // The Luminance Quantization Table
    {   
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 35, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}
    },
    // The Chrominance Quantization Table
    {
        {17, 18, 24, 47, 99, 99, 99, 99},
        {18, 21, 26, 66, 99, 99, 99, 99},
        {24, 26, 56, 99, 99, 99, 99, 99},
        {47, 66, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99}
    }
};

const int ZigzagOrder[64] = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

double alpha(int x);

void print(const char* FileName, double *Y, double *U, double *V, int height, int width, int flag);

void print(const char* FileName, int *Y, int *U, int *V, int height, int width, int flag);

#endif 

