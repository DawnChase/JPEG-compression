#include "decode.h"

int  readDHT(FILE *fp, DHTSegment *dht);
void ReverseHuffman(FILE *fp, vector <pair<int, int> > &M, int flag);
void ReverseRLE(vector <pair<int, int> > &RLE, int *AC, int height, int width);
void ReverseDPCM(vector <pair<int, int> > &DPCM, int *DC, int height, int width);
void CopyQ(int *AC, int *DC, int *Q, int height, int width);
void ReverseZigzagScan(int *Matrix, int height, int width);
void TransformYUVToRGB(unsigned char *Info, double *Y, double *U, double *V, int height, int width);
void Dequantize(int *Q_Matrix, double *Matrix, int height, int width, int flag);
void IDCT(double *Matrix, int height, int width);
// Decompression:
// 1. ~Entropy Decoding
// 2. ~DPCM
// 3. ~RLE
// 4. ~zigzag
// 5. Dequantize
// 6. IDCT
// 7. upsample
// 8. YUV -> RGB
void Decompress(const char *FileName, const char *OutputFileName, BMPInfoHeader *InfoHeader)
{
    // open input file
    int height, width;
    FILE *fp = fopen(FileName, "rb");
    if (fp == NULL)
    {
        printf("Error: cannot open input file! \n");
        return;
    }
    fread(&height, sizeof(int), 1, fp);
    fread(&width, sizeof(int), 1, fp);
    printf("Height: %d\n", height);
    printf("Width: %d\n", width);
    // read DHT + ~Huffman
    vector <pair<int, int> > DPCM_Y, DPCM_UV;
    vector <pair<int, int> > RLE_Y, RLE_UV;
    ReverseHuffman(fp, DPCM_Y, 0);
    ReverseHuffman(fp, DPCM_UV, 0);
    ReverseHuffman(fp, RLE_Y, 1);
    ReverseHuffman(fp, RLE_UV, 1);
    // ~RLE
    printf("~RLE processing... \n");
    int *AC_Y = (int *)malloc(sizeof(int) * height * width);
    int *AC_U = (int *)malloc(sizeof(int) * height * width / 4);
    int *AC_V = (int *)malloc(sizeof(int) * height * width / 4);
    ReverseRLE(RLE_Y, AC_Y, height, width);
    ReverseRLE(RLE_UV, AC_U, height / 2, width / 2);
    ReverseRLE(RLE_UV, AC_V, height / 2, width / 2);
        print("decode~RLE.test", AC_Y, AC_U, AC_V, height, width, 1);
    // ~DPCM
    printf("~DPCM processing... \n");
    int *DC_Y = (int *)malloc(sizeof(int) * height * width / (N * N));
    int *DC_U = (int *)malloc(sizeof(int) * height * width / (N * N) / 4);
    int *DC_V = (int *)malloc(sizeof(int) * height * width / (N * N) / 4);
    ReverseDPCM(DPCM_Y, DC_Y, height, width);
    ReverseDPCM(DPCM_UV, DC_U, height / 2, width / 2);
    ReverseDPCM(DPCM_UV, DC_V, height / 2, width / 2);
        print("decode~DPCM.test", DC_Y, DC_U, DC_V, height / N, width / N, 1);
    // AC + DC -> Q
    printf("AC + DC -> Q processing... \n");
    int *Q_Y = (int *)malloc(sizeof(int) * height * width);
    int *Q_U = (int *)malloc(sizeof(int) * height * width / 4);
    int *Q_V = (int *)malloc(sizeof(int) * height * width / 4);
    CopyQ(AC_Y, DC_Y, Q_Y, height, width);
    CopyQ(AC_U, DC_U, Q_U, height / 2, width / 2);
    CopyQ(AC_V, DC_V, Q_V, height / 2, width / 2);
    free(DC_Y); free(AC_Y);
    free(DC_U); free(AC_U);
    free(DC_V); free(AC_V);
        print("decodeQ.test", Q_Y, Q_U, Q_V, height, width, 1);
    // ~zigzag
    printf("~Zigzag scan processing... \n");
    ReverseZigzagScan(Q_Y, height, width);
    ReverseZigzagScan(Q_U, height / 2, width / 2);
    ReverseZigzagScan(Q_V, height / 2, width / 2);
        // print("decodeZigzag.test", Q_Y, Q_U, Q_V, height, width, 1);
    // Dequantize
    printf("Dequantization processing... \n");
    double *Y = (double *)malloc(sizeof(double) * height * width);
    double *U = (double *)malloc(sizeof(double) * height * width / 4);
    double *V = (double *)malloc(sizeof(double) * height * width / 4);
    Dequantize(Q_Y, Y, height, width, 0);
    Dequantize(Q_U, U, height / 2, width / 2, 1);
    Dequantize(Q_V, V, height / 2, width / 2, 1);
        // print("decodeDequantize.test", Y, U, V, height, width, 1);
    free(Q_Y); free(Q_U); free(Q_V);
    // IDCT
    printf("IDCT processing... \n");
    IDCT(Y, height, width);
    IDCT(U, height / 2, width / 2);
    IDCT(V, height / 2, width / 2);
        // print("decodeIDCT.test", Y, U, V, height, width, 1);
    // upsample color (4:2:0)
    // U, V: 1/2
    printf("Upsample processing... \n");
    double *UU = (double *)malloc(sizeof(double) * height * width);
    double *VV = (double *)malloc(sizeof(double) * height * width);
    for (int i = 0; i < height; i += 2)
        for (int j = 0; j < width; j += 2)
        {
            int k = i * width + j;
            UU[k] = UU[k + 1] = UU[k + width] = UU[k + width + 1] = U[i * width / 4 + j / 2];
            VV[k] = VV[k + 1] = VV[k + width] = VV[k + width + 1] = V[i * width / 4 + j / 2];
        }
    free(U); free(V);
    U = UU; V = VV;
        // print("decodeUpsample.test", Y, U, V, height, width, 0);
    // YUV -> RGB
    printf("YUV -> RGB processing... \n");
    unsigned char *Info = (unsigned char *)malloc(sizeof(unsigned char) * height * width * 3);
    TransformYUVToRGB(Info, Y, U, V, height, width);
    // write to output file
    PrintBMP(OutputFileName, Info, InfoHeader);
    free(Y); free(U); free(V);
    free(Info);
    fclose(fp);
}

int readDHT(FILE *fp, DHTSegment *dht)
{
    fread(&dht -> marker, sizeof(unsigned short), 1, fp);
    fread(&dht -> length, sizeof(int), 1, fp);
    fread(&dht -> HTInformation, sizeof(unsigned char), 1, fp);
    fread(dht -> BITS, sizeof(unsigned char), 16, fp);
    // printf("%d %d %d\n", dht -> marker, dht -> length, dht -> HTInformation);
    int cnt = 0;
    for (int i = 0; i < 16; i ++)
    {
        cnt += dht -> BITS[i];
        // printf("%d ", dht -> BITS[i]);
    }
    // printf("\n");
    dht -> HUFFVAL = (int *)malloc(sizeof(int) * cnt);
    fread(dht -> HUFFVAL, sizeof(int), cnt, fp);
    return cnt;
}

void ReverseHuffman(FILE *fp, vector <pair<int, int> > &M, int flag)
{
    // read DHT
    printf("Read DHT... \n");
    DHTSegment *dht = (DHTSegment *)malloc(sizeof(DHTSegment));
    int cnt = readDHT(fp, dht);
    // reconstruct Huffman tree
    printf("Reconstruct Huffman tree... \n");
    HuffmanNode *root = new HuffmanNode(-1, 0);
    reconstructHuffman(root, dht -> BITS, dht -> HUFFVAL);
    // decode
    // printf("%d\n", dht -> length);
    int bytes = (dht -> length - 2 - 4 - 1 - 16 - sizeof(int) * cnt) ;
    // printf("%d\n", bytes);
    int i = 0, pos = 7;
    unsigned char bit;
    fread(&bit, sizeof(unsigned char), 1, fp);
    while (i < bytes)
    {
        int size = 0, amplitude = 0, haltFlag = 0;
        HuffmanNode *p = root;
        // check every bit on huffman tree
        while (p -> symbol == -1)
        {
            if (bit & (1 << pos))
                p = p -> right;
            else
                p = p -> left;
            pos --;
            if (pos < 0)
            {
                if (i == bytes - 1)
                {
                    haltFlag = 1;
                    break;
                }
                fread(&bit, sizeof(unsigned char), 1, fp);
                i ++; pos = 7;
            }
        }
        size = p -> symbol;
        if (flag == 1)          // RLE
            size = size & 15;
        // get amplitude ("size" bits)
        for (int j = 0; j < size; j ++)
        {
            amplitude = amplitude << 1;
            if (bit & (1 << pos))
                amplitude ++;
            pos --;
            if (pos < 0)
            {
                if (i == bytes - 1)
                {
                    haltFlag = 1;
                    break;
                }
                fread(&bit, sizeof(unsigned char), 1, fp);
                i ++; pos = 7;
            }
        }
        if (haltFlag)
            break;
        // get the right amplitude (maybe negative)
        if (amplitude < (1 << (size - 1)))
            amplitude = amplitude - (1 << size) + 1;
        // if (i <= 10)
        //     printf("(%d,%d)\n", size, amplitude);
        M.push_back(make_pair(p -> symbol, amplitude));
    }
    // printf("%ld\n", M.size());
}

void ReverseRLE(vector <pair<int, int> > &RLE, int *AC, int height, int width)
{
    // ((skip, size), value)
    int LastZero = 1;
    vector <pair<int, int> >::iterator it = RLE.begin();
    // printf("%ld\n", RLE.size());
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            int skip, size, value;
            if (LastZero == 1)
            {
                skip = (it -> first) >> 4, size = (it -> first) & 15, value = it -> second;
                it ++;
            }
            int k = i * width + j, x = 0, y = 0;
            AC[k] = 0;    // DC
            while ((skip != 0 || value != 0) && (x != N - 1 || y != N - 1))
            {
                for (int t = 0; t < skip; t ++)
                {
                    if (y == N - 1) x ++, y = 0;
                    else y ++;
                    AC[k + x * width + y] = 0;
                }
                if (y == N - 1) x ++, y = 0;
                else y ++;
                AC[k + x * width + y] = value;
                skip = (it -> first) >> 4, size = (it -> first) & 15, value = it -> second;
                it ++;
            }
            if (x == N - 1 && y == N - 1)
                LastZero = 0;
            while (x != N - 1 || y != N - 1)
            {
                if (y == N - 1) x ++, y = 0;
                else y ++;
                AC[k + x * width + y] = 0;
            }
        }
    RLE.erase(RLE.begin(), it);
}

void ReverseDPCM(vector <pair<int, int> > &DPCM, int *DC, int height, int width)
{
    // (size, amplitude)
    int k = 0;
    vector <pair<int, int> >::iterator it = DPCM.begin();
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            int size = it -> first, amplitude = it -> second;
            it ++;
            if (k == 0)
                DC[k] = amplitude;
            else
                DC[k] = amplitude + DC[k - 1];
            k ++;
        }
    DPCM.erase(DPCM.begin(), it);
}

void CopyQ(int *AC, int *DC, int *Q, int height, int width)
{
    int cnt = 0;
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            int k = i * width + j;
            Q[k] = DC[cnt ++];
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                {
                    if (u == 0 && v == 0)
                        continue;
                    int t = (i + u) * width + j + v;
                    Q[t] = AC[t];
                }
        }
}

void ReverseZigzagScan(int *Matrix, int height, int width)
{
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            int Block[N][N];
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                    Block[u][v] = Matrix[(i + u) * width + j + v];
            int k = 0;
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                {
                    Matrix[(i + ZigzagOrder[k] / N) * width + j + ZigzagOrder[k] % N] = Block[u][v];
                    k ++;
                }
        }
}

void IDCT(double *Matrix, int height, int width)
{
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            double Block[N][N];
            for (int x = 0; x < N; x ++)
                for (int y = 0; y < N; y ++)
                {
                    Block[x][y] = 0;
                    for (int u = 0; u < N; u ++)
                        for (int v = 0; v < N; v ++)
                        {
                            int k = (i + u) * width + j + v;
                            Block[x][y] += alpha(u) * alpha(v) * Matrix[k] * cos((2 * x + 1) * u * Pi / (2 * N)) * cos((2 * y + 1) * v * Pi / (2 * N));
                        }
                }
            for (int x = 0; x < N; x ++)
                for (int y = 0; y < N; y ++)
                    Matrix[(i + x) * width + j + y] = Block[x][y];
        }
}

void Dequantize(int *Q_Matrix, double *Matrix, int height, int width, int flag)
{
    for (int i = 0; i < height; i += N)
        for (int j = 0; j < width; j += N)
        {
            for (int u = 0; u < N; u ++)
                for (int v = 0; v < N; v ++)
                {
                    int k = (i + u) * width + j + v;
                    Matrix[k] = Q_Matrix[k] * Qtable[flag][u][v];
                }
        }
}

double min(double a, double b){return a < b ? a : b;}
double max(double a, double b){return a > b ? a : b;}

void TransformYUVToRGB(unsigned char *Info, double *Y, double *U, double *V, int height, int width)
{
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            int k = i * width + j;
            // printf("YUV: %lf %lf %lf RGB: %lf %lf %lf\n", Y[k], U[k], V[k], 
            //         Y[k] + 1.772   * (U[k] - 128),
            //         Y[k] - 0.34414 * (U[k] - 128) - 0.71414 * (V[k] - 128),
            //         Y[k] + 1.402   * (V[k] - 128));
            Info[3 * k    ] = min(255, max(0, (Y[k] + 128) + 1.772   * U[k]));                          // B
            Info[3 * k + 1] = min(255, max(0, (Y[k] + 128) - 0.3441  * U[k] - 0.7141 * V[k])); // G
            Info[3 * k + 2] = min(255, max(0, (Y[k] + 128) + 1.402   * V[k]));                          // R
        }
}