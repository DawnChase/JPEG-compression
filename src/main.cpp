#include "h/head.h"
#include "h/bmp.h"
#include "h/menu.h"
#include "h/decode.h"
#include "h/encode.h"

BMPInfoHeader InfoHeader;
unsigned char *Info; 
int Menu;

int main()
{
	PrintMenu();
	scanf("%d", &Menu);
	while (Menu != 7)
	{
		switch (Menu)
		{
			case 1: 	// read BMP file
			{
				printf("Please input the BMP file name.\n");
				char FileName[100];
				scanf("%s", FileName);
				if (!(Info = ReadBMP(FileName, &InfoHeader)))
				{
					printf("Please try again.\n");	
					break;
				}
				printf("Read BMP file successfully.\n");
				printf("Width: %d\n", InfoHeader.biWidth);
				printf("Height: %d\n", InfoHeader.biHeight);
				break;
			}
			case 2:		// print BMP file
			{
				if (!Info)
				{
					printf("Please read BMP file first.\n");
					break;
				}
				printf("Please input the BMP file name.\n");
				char FileName[100];
				scanf("%s", FileName);
				PrintBMP(FileName, Info, &InfoHeader);
				printf("Print BMP file successfully.\n");
				break;
			}
			case 3:		// compress BMP file
			{
				if (!Info)
				{
					printf("Please read BMP file first.\n");
					break;
				}
				Compress(Info, &InfoHeader);
				printf("Compress BMP file by DCT successfully.\n");
				break;
			}
			case 4:		// decode BMP file
			{
				if (!Info)
				{
					printf("Please read BMP file first.\n");
					break;
				}
				printf("Please input the compressed file name.\n");
				char FileName[100];
				scanf("%s", FileName);
				Decompress(FileName, "output.bmp", &InfoHeader);
				printf("Decode BMP file from DCT successfully.\n");
				break;
			}
			// case 5:		// compress BMP file by WT
			// {
			// 	if (!Info)
			// 	{
			// 		printf("Please read BMP file first.\n");
			// 		break;
			// 	}
			// 	printf("Please input the BMP file name.\n");
			// 	char FileName[100];
			// 	scanf("%s", FileName);
			// 	CompressWT(FileName, Info, &InfoHeader);
			// 	printf("Compress BMP file by WT successfully.\n");
			// 	break;
			// }
			// case 6:		// decode BMP file from WT and print it
			// {
			// 	if (!Info)
			// 	{
			// 		printf("Please read BMP file first.\n");
			// 		break;
			// 	}
			// 	printf("Please input the BMP file name.\n");
			// 	char FileName[100];
			// 	scanf("%s", FileName);
			// 	DecodeWT(FileName, Info, &InfoHeader);
			// 	printf("Decode BMP file from WT and Print it successfully.\n");
			// 	break;
			// }
			default:
			{
				printf("Please input the right number.\n");
				break;
			}
		}

		// system("pause");
		PrintMenu();
		scanf("%d", &Menu);
	}
    return 0;
}

