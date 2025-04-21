#include "h/head.h"
#include "h/bmp.h"
#include "h/decode.h"
#include "h/encode.h"

int Menu;

int main()
{
	PrintMenu();
	cin >> Menu;
	while (Menu != 4)
	{
		switch (Menu)
		{
			case 1: 	// read BMP file
			{
				printf("Please input the BMP file name.\n");
				string FileName;
				BMPInfoHeader InfoHeader;
				unsigned char *Info; 
				cin >> FileName;
				// read BMP file by FileName
				if (!(Info = ReadBMP(FileName, &InfoHeader)))
				{
					printf("Please try again.\n");	
					break;
				}
				// print BMP file information
				printf("Read BMP file successfully.\n");
				printf("Width: %d\n", InfoHeader.biWidth);
				printf("Height: %d\n", InfoHeader.biHeight);
				// PrintBMP(FileName.substr(0, FileName.length() - 4) + "_tailored.bmp", Info, &InfoHeader);
				break;
			}
			case 2:		// compress BMP file
			{
				printf("Please input the BMP file name.\n");
				string FileName;
				BMPInfoHeader InfoHeader;
				unsigned char *Info; 
				cin >> FileName;
				// read BMP file by FileName
				if (!(Info = ReadBMP(FileName, &InfoHeader)))
				{
					printf("Please try again.\n");	
					break;
				}
				FileName = FileName.substr(0, FileName.length() - 4) + ".JPEG";
				// Compress BMP file by JPEG
				Compress(Info, FileName, &InfoHeader);
				printf("Compress BMP file successfully, the file is '%s' .\n", FileName.c_str());
				break;
			}
			case 3:		// decode BMP file
			{
				printf("Please input the JPEG file name.\n");
				string FileName;
				cin >> FileName;
				string outputFileName = FileName.substr(0, FileName.length() - 5) + "_decompression.bmp";
				// Decompress JPEG file to BMP
				Decompress(FileName, outputFileName);
				printf("Decode BMP file successfully, the file is '%s'.\n", outputFileName.c_str());
				break;
			}
			default:
			{
				printf("Please input the right number.\n");
				break;
			}
		}
		PrintMenu();
		cin >> Menu;
	}
    return 0;
}

