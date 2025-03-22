#include "bmp.h"

unsigned char *ReadBMP(const char* FileName, BMPInfoHeader *InfoHeader)
{
	BMPFileHeader FileHeader;
	FILE *fp;
	if (!(fp = fopen(FileName, "rb")))
	{
		printf("Input File Open unsucceeded!\n");
		return 0;
	}
	fread(&FileHeader, sizeof(BMPFileHeader), 1, fp);
	if (FileHeader.bfType != 0x4D42)
	{
		printf("It isn't a BMP!!!");
		return 0;
	}
	fread(InfoHeader, sizeof(BMPInfoHeader), 1, fp);
	int OffsetWidth = (4 - (InfoHeader -> biWidth * 3) % 4) % 4;		//calculate the offset bytes for width
	int width = (InfoHeader -> biWidth * 3 + OffsetWidth);			    //calculate the total bytes for width
	int len = InfoHeader -> biHeight * width;
	unsigned char *Info = (unsigned char*)malloc(sizeof(unsigned char)*len);
	for (int Infoi = 0; Infoi < InfoHeader -> biHeight; Infoi ++)
	{
		fread(&(Info[Infoi * width]), 3, InfoHeader -> biWidth, fp);	//read the image for this line
		fseek(fp, OffsetWidth, SEEK_CUR);							    //change the pointer "fp" to the next line
	}
	fclose(fp);
	InfoHeader -> biSizeImage = len;	//update the contents of the InfoHeader
	return Info;
}


void PrintBMP(const char *FileName, unsigned char *Info, BMPInfoHeader *InfoHeader)
{
	BMPFileHeader FileHeader;
	FILE *fp;
	if (!(fp = fopen(FileName,"wb")))
	{
		printf("Output File Open unsucceeded!\n");
		return ;
	}

	FileHeader.bfType = 0x4D42;
	FileHeader.bfSize = (InfoHeader -> biSizeImage) + sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
	FileHeader.bfReserve1 = 0;
	FileHeader.bfReserve2 = 0;
	FileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
	
	fwrite(&FileHeader, sizeof(BMPFileHeader), 1, fp);
	fwrite(InfoHeader, sizeof(BMPInfoHeader), 1, fp);
	fwrite(Info,1,InfoHeader -> biSizeImage,fp);
	fclose(fp);
}