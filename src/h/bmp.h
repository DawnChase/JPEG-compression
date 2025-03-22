// read and write a bmp

#ifndef BMP_H
#define BMP_H

#include "head.h"

//读入BMP文件
unsigned char *ReadBMP(const char* FileName, BMPInfoHeader *InfoHeader);

//print BMP file
void PrintBMP(const char *FileName, unsigned char *Info, BMPInfoHeader *InfoHeader);

#endif