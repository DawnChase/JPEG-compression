#include "head.h"

#define N 8

double alpha(int x)
{
    return (x == 0 ? sqrt(1.0 / N) : sqrt(2.0 / N));
}

void PrintMenu()
{
    printf("1. Read BMP file.\n");
    printf("2. Compress BMP file by JPEG(DCT).\n");
    printf("3. Decode BMP file from JPEG(DCT) and Print it.\n");
    printf("4. Quit.\n");
}

void print(const char* FileName, double *Y, double *U, double *V, int height, int width, int flag)
{
    FILE *fp = fopen(FileName, "w");
    for (int i = 0; i < height; i ++)
    {
        for (int j = 0; j < width; j ++)
            fprintf(fp, "%lf ", Y[i * width + j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
    if (flag) 
        height /= 2, width /= 2;
    for (int i = 0; i < height; i ++)
    {
        for (int j = 0; j < width; j ++)
            fprintf(fp, "%lf ", U[i * width + j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
    for (int i = 0; i < height; i ++)
    {
        for (int j = 0; j < width; j ++)
            fprintf(fp, "%lf ", V[i * width + j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
    fclose(fp);
}

void print(const char* FileName, int *Y, int *U, int *V, int height, int width, int flag)
{
    FILE *fp = fopen(FileName, "w");
    for (int i = 0; i < height; i ++)
    {
        for (int j = 0; j < width; j ++)
            fprintf(fp, "%d ", Y[i * width + j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n\n");
    if (flag) 
        height /= 2, width /= 2;
    for (int i = 0; i < height; i ++)
    {
        for (int j = 0; j < width; j ++)
            fprintf(fp, "%d ", U[i * width + j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n\n");
    for (int i = 0; i < height; i ++)
    {
        for (int j = 0; j < width; j ++)
            fprintf(fp, "%d ", V[i * width + j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n\n");
    fclose(fp);
}