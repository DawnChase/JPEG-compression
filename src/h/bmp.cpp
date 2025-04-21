#include "bmp.h"

// read a BMP file from filename
unsigned char *ReadBMP(string FileName, BMPInfoHeader *InfoHeader)
{
	BMPFileHeader FileHeader;
	FILE *fp;
	if (!(fp = fopen(FileName.c_str(), "rb")))
	{
		printf("Input File Open unsucceeded!\n");
		return 0;
	}
	fread(&FileHeader, sizeof(BMPFileHeader), 1, fp);
	// check if the file is a BMP file
	if (FileHeader.bfType != 0x4D42)
	{
		printf("It isn't a BMP!!!");
		return 0;
	}
	fread(InfoHeader, sizeof(BMPInfoHeader), 1, fp);
	// pad the image to 16×16 blocks
	int OffsetWidth = (4 - (InfoHeader -> biWidth * 3) % 4) % 4;
	int width  = ((InfoHeader -> biWidth - 1) / 16 + 1) * 16;
	int height = ((InfoHeader -> biHeight - 1) / 16 + 1) * 16; 
	// calc new SizeImage
	int len = height * width * 3;
	unsigned char *Info = (unsigned char *) malloc(sizeof(unsigned char) * len);
	memset(Info, 0, sizeof(unsigned char) * len);
	for (int Infoi = 0; Infoi < InfoHeader -> biHeight; Infoi ++)
	{
		fread(&(Info[Infoi * width * 3]), 3, InfoHeader -> biWidth, fp);	//read the image for this line
		fseek(fp, OffsetWidth, SEEK_CUR);							    	//change the pointer "fp" to the next line
	}
	fclose(fp);
	// reset the InfoHeader
	InfoHeader -> biWidth = width;
	InfoHeader -> biHeight = height;
	InfoHeader -> biSizeImage = len;

	return Info;
}

// reset BMP Information header
BMPInfoHeader *setInfoHeader(int height, int width)
{
	BMPInfoHeader *InfoHeader = (BMPInfoHeader *)malloc(sizeof(BMPInfoHeader));
	InfoHeader -> biSize = sizeof(BMPInfoHeader);
	InfoHeader -> biWidth = width;
	InfoHeader -> biHeight = height;
	InfoHeader -> biPlanes = 1;
	InfoHeader -> biBitCount = 24;
	InfoHeader -> biCompression = 0;
	InfoHeader -> biSizeImage = width * height * 3;
	InfoHeader -> biXPelsPerMeter = 2834;
	InfoHeader -> biYPelsPerMeter = 2834;
	InfoHeader -> biClrUsed = 0;
	InfoHeader -> biClrImportant = 0;
	return InfoHeader;
}

// print BMP file
void PrintBMP(string FileName, unsigned char *Info, BMPInfoHeader *InfoHeader)
{
	BMPFileHeader FileHeader;
	FILE *fp;
	if (!(fp = fopen(FileName.c_str(),"wb")))
	{
		printf("Output File Open unsucceeded!\n");
		return ;
	}
	// set the file header
	FileHeader.bfType = 0x4D42;
	FileHeader.bfSize = (InfoHeader -> biSizeImage) + sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
	FileHeader.bfReserve1 = 0;
	FileHeader.bfReserve2 = 0;
	FileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
	// print the file header
	fwrite(&FileHeader, sizeof(BMPFileHeader), 1, fp);
	// print the information header
	fwrite(InfoHeader, sizeof(BMPInfoHeader), 1, fp);
	// print the image
	fwrite(Info, 1, InfoHeader -> biSizeImage, fp);
	fclose(fp);
}