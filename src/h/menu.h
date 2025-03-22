// print menu

#ifndef MENU_H
#define MENU_H

#include "head.h"

void PrintMenu()
{
    printf("1. Read BMP file.\n");
    printf("2. Print BMP file.\n");
    printf("3. Compress BMP file by DCT.\n");
    printf("4. Decode BMP file from DCT and Print it.\n");
    printf("5. Compress BMP file by WT.\n");
    printf("6. Decode BMP file from WT and Print it.\n");
    printf("7. Quit.\n");
}

#endif