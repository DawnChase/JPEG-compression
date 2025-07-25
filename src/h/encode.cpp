#include "encode.h"

void TransformRGBToYUV(unsigned char *Info, double *Y, double *U, double *V, int height, int width);
void DCT(double *Matrix, int height, int width);
void Quantize(double *Matrix, int *Q_Matrix, int height, int width, int flag, int qf);
void ZigzagScan(int *Matrix, int height, int width);
void RLE(int *Matrix, int height, int width, vector <pair<int, int> > &EC);
void DPCM(int *Matrix, int height, int width, vector <pair<int, int> > &EC);

// Compression:
// 1. RGB -> YUV
// 2. subsample
// 3. DCT
// 4. Quantization
// 5. zigzag
// 6. RLE
// 7. DPCM
// 8. Entropy Coding
void Compress(unsigned char *Info, string FileName, BMPInfoHeader *InfoHeader)
{
    int height = InfoHeader -> biHeight;
    int width = InfoHeader -> biWidth;
    int len = height * width;
    // RGB -> YUV
    printf("RGB -> YUV processing... \n");
    double *Y = (double *)malloc(sizeof(double) * len);
    double *U = (double *)malloc(sizeof(double) * len);
    double *V = (double *)malloc(sizeof(double) * len);
    TransformRGBToYUV(Info, Y, U, V, height, width);
        // print("encodeYUV.test", Y, U, V, height, width, 0);
    // subsample color (4:2:0)
    // U, V: 1/2
    double *UU = (double *)malloc(sizeof(double) * len / 4);
    double *VV = (double *)malloc(sizeof(double) * len / 4);
    for (int i = 0; i < height; i += 2)
        for (int j = 0; j < width; j += 2)
        {
            int k = i * width + j;
            UU[i * width / 4 + j / 2] = (U[k] + U[k + 1] + U[k + width] + U[k + width + 1]) / 4;
            VV[i * width / 4 + j / 2] = (V[k] + V[k + 1] + V[k + width] + V[k + width + 1]) / 4;
        }
    free(U); free(V);
    U = UU; V = VV;
        // print("encodeSubsample.test", Y, U, V, height, width, 1);
    // DCT (block = 8 * 8)
    printf("DCT processing... \n");
    DCT(Y, height, width);
    DCT(U, height / 2, width / 2);
    DCT(V, height / 2, width / 2);
        // print("encodeDCT.test", Y, U, V, height, width, 1);
    // Quantization
    printf("Quantization processing... \n");
    printf("Please input quality factor (1 ~ 100): ");
    int qf;
    scanf("%d", &qf);
    int *Q_Y = (int *)malloc(sizeof(int) * len);
    int *Q_U = (int *)malloc(sizeof(int) * len);
    int *Q_V = (int *)malloc(sizeof(int) * len);
    Quantize(Y, Q_Y, height, width, 0, qf);
    Quantize(U, Q_U, height / 2, width / 2, 1, qf);
    Quantize(V, Q_V, height / 2, width / 2, 1, qf);
        // print("encodeQuantize.test", Q_Y, Q_U, Q_V, height, width, 1);
    free(Y); free(U); free(V);
    // zigzag scan
    printf("Zigzag scan processing... \n");
    ZigzagScan(Q_Y, height, width);
    ZigzagScan(Q_U, height / 2, width / 2);
    ZigzagScan(Q_V, height / 2, width / 2);
        // print("encodeQ.test",Q_Y, Q_U, Q_V, height, width, 1);
    // RLE on AC
    printf("RLE processing... \n");
    vector <pair<int, int> > L_ACEC;    // Y
    vector <pair<int, int> > C_ACEC;    // U + V
    RLE(Q_Y, height, width, L_ACEC);
    RLE(Q_U, height / 2, width / 2, C_ACEC);
    RLE(Q_V, height / 2, width / 2, C_ACEC);
        // print("encodeRLE.test", Q_Y, Q_U, Q_V, height, width, 1);
    // DPCM on DC
    printf("DPCM processing... \n");
    vector <pair<int, int> > L_DCEC;    // Y
    vector <pair<int, int> > C_DCEC;    // U + V
    DPCM(Q_Y, height, width, L_DCEC);
    DPCM(Q_U, height / 2, width / 2, C_DCEC);
    DPCM(Q_V, height / 2, width / 2, C_DCEC);
        // print("encodeDPCM.test", Q_Y, Q_U, Q_V, height, width, 1);
    // free
    free(Q_Y); free(Q_U); free(Q_V);
    // print to file
    FILE *huffmanfp = fopen(FileName.c_str(), "wb");
    if (huffmanfp == NULL)
    {
        printf("Error: cannot open huffman codes file! \n");
        return;
    }
    // add height and width 
    fwrite(&height, sizeof(int), 1, huffmanfp);
    fwrite(&width, sizeof(int), 1, huffmanfp);
    fwrite(&qf, sizeof(int), 1, huffmanfp);
    // Entropy Coding
    printf("Entropy Coding processing... \n");
    huffman(huffmanfp, L_DCEC, 0x00, 0);
    huffman(huffmanfp, C_DCEC, 0x01, 0);
    huffman(huffmanfp, L_ACEC, 0x10, 1);
    huffman(huffmanfp, C_ACEC, 0x11, 1);
        // printf("%ld %ld %ld %ld\n", L_ACEC.size(), C_ACEC.size(), L_DCEC.size(), C_DCEC.size());
        // printf("%ld %ld %ld %ld\n", L_ACcodes.size(), C_ACcodes.size(), L_DCcodes.size(), C_DCcodes.size());
    // close file
    fclose(huffmanfp);
}

void DPCM(int *Matrix, int height, int width, vector <pair<int, int> > &EC)
{
    // (size, amplitude)
    int PreviousDC = 0, cnt = 0;
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            int CurrentDC = Matrix[i * width + j];
            int delta = CurrentDC - PreviousDC;
            // calc size
            int size = 0, absdelta = abs(delta);
            while (absdelta != 0)
            {
                size ++;
                absdelta /= 2;
            }
            EC.push_back(make_pair(size, delta));    
            PreviousDC = CurrentDC;
        }
}

void RLE(int *Matrix, int height, int width, vector <pair<int, int> > &EC)
{
    // (skip << 4 + size, value)
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            // check every 8 * 8 block
            int skipCount = 0;
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                {
                    int k = (i + u) * width + j + v;
                    if (u == 0 && v == 0)
                        continue;
                    if (Matrix[k] == 0)
                        skipCount ++;
                    else
                    {
                        int size = 0, x = abs(Matrix[k]);
                        // calc size
                        while (x != 0)
                        {
                            size ++;
                            x /= 2;
                        }
                        EC.push_back(make_pair((skipCount << 4) + size, Matrix[k]));
                        skipCount = 0;
                    }
                }
            // last (0, 0)
            if (skipCount > 0)
                EC.push_back(make_pair(0, 0));
        }
}

void ZigzagScan(int *Matrix, int height, int width)
{
    // 0 -> 1 -> 8 -> ...
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            // temp block
            int Block[N][N];
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                    Block[u][v] = Matrix[(i + u) * width + j + v];
            int k = 0;
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                {
                    Matrix[(i + u) * width + j + v] = Block[ZigzagOrder[k] / N][ZigzagOrder[k] % N];
                    k ++;
                }
        }
}

void Quantize(double *Matrix, int *Q_Matrix, int height, int width, int flag, int qf)
{
    double scalingfactor = (qf < 50) ? (50.0 / qf) : (100.0 - qf) / 50.0;
    // use standard quantization table
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                {
                    int k = (i + u) * width + j + v;
                    if (abs(scalingfactor) < 1e-7)
                        Q_Matrix[k] = round(Matrix[k]);
                    else
                        Q_Matrix[k] = round(Matrix[k] / (Qtable[flag][u][v] * scalingfactor));
                }
        }
}

void DCT(double *Matrix, int height, int width)
{
    // directly use the formula
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            // temp block
            double Block[N][N];
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                {
                    Block[u][v] = 0;
                    for (int x = 0; x < N; x ++)
                        for (int y = 0; y < N; y ++)
                        {
                            int k = (i + x) * width + j + y;
                            Block[u][v] += Matrix[k] * cos((2 * x + 1) * u * Pi / (2 * N)) * cos((2 * y + 1) * v * Pi / (2 * N));
                        }
                    Block[u][v] = alpha(u) * alpha(v) * Block[u][v];
                }
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                    Matrix[(i + u) * width + j + v] = Block[u][v];
        }
}

void TransformRGBToYUV(unsigned char *Info, double *Y, double *U, double *V, int height, int width)
{
    // use the formula
    // let Y, U, V in [-128, 127] for DCT 
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            int k = i * width + j;
            int B = Info[3 * k], G = Info[3 * k + 1], R = Info[3 * k + 2];
            Y[k] =  0.299  * R + 0.587  * G + 0.114  * B - 128;
            U[k] = -0.1687 * R - 0.3313 * G + 0.5    * B;
            V[k] =  0.500  * R - 0.4187 * G - 0.0813 * B;
        }
}

