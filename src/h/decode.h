// decompress BMP file

#ifndef DECODE_H
#define DECODE_H

#include "head.h"
#include "bmp.h"
#include "huffman.h"
#define N 8
#define Pi 3.14159265358979323846

// Decompress JPEG file to BMP
void Decompress(string FileName, string OutputFileName);

#endif

