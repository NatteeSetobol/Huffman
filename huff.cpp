#include <stdio.h>
#include "huffman.h"


enum { COMPRESS, DECOMPRESS };


huffman* huffmanCompression=NULL;
char* fileName=NULL;
char* outFile=NULL;



void Logo();
void Usage(const char* error);
void Delete();

int main(int argc, char **argv)
{
	int mode=-1;
	int parameterCount = argc+1;

	if (argc == 1)
	{
		
		Usage("no arguements\n");
		return 0;
	}

	for (int i=0;i<argc;i++)
	{
		if (strcmp(argv[i], "-c") == 0)
		{
			if (mode == -1) 
			{
				mode = COMPRESS;
			} else {
				Usage("Please choose to compress (-c) or decompress (-d). You can't do both\n");
				Delete();
				return 0;
			}
		}
		if (strcmp(argv[i], "-d") == 0)
		{
			if (mode == -1)
			{
				mode = DECOMPRESS;
			} else {
				Usage("Please choose to compress (-c) or decompress (-d). You can't do both\n");
				Delete();
				return 0;
			}
		}
		if (strcmp(argv[i], "-f") == 0)
		{
			if (i+1 > parameterCount)
			{
				Usage("Please enter a file to compress.\n");
				Delete();
				return 0;
			} else {
				if (strlen(argv[i+1]) > 3)
				{
					fileName = new char[strlen(argv[i+1])+1];
					strcpy(fileName, argv[i+1]);
				}
			}
		}
		if (strcmp(argv[i], "-o") == 0)
		{
			if (i+1 > parameterCount)
			{
				Usage("Please include an output file\n");
				Delete();
				return 0;
			} else {
				if (strlen(argv[i+1]) > 3)     
				{ 
					outFile = new char[strlen(argv[i+1])+1];
					strcpy(outFile, argv[i+1]);
				} 
			}
		}

	}
	

	if (fileName == NULL)
	{
		Usage("Please enter a file to compress\n");
		Delete();
		return 0;
	}

	if (outFile == NULL)
	{
		Usage("Please enter an  output file name");
		Delete();
		return 0;
	}
	
	if (mode == -1)
	{
		Usage("Please choose whether to compress (-c) or decompress (-d)!\n");
		Delete();
		return 0;
	}

	huffmanCompression = new huffman();
	Logo();
	if (mode == COMPRESS)
	{
		printf("\nCompressing File..Please Wait\n");
		if (huffmanCompression->Compress(fileName, outFile) == false)
		{
			Delete();
			return 0;
		}
		
	} else 
	if (mode == DECOMPRESS)
	{
		printf("\nDecompressing File...Please Wait!\n");
		if (huffmanCompression->Decompress(fileName,outFile) == false)
		{
			Delete();
			return 0;
		}
		
	}
	Delete();
	printf("\nDone!\n");
	return 0;
}

void Usage(const char* error)
{
	Logo();
	printf("Usage:\n\n");
	printf("Modes:\n");
	printf("-c Compresses file\n");
	printf("-d Decompresses file\n");
	printf("-f File to compress\n");
	printf("-o Commpression file to decompress\n");
	printf("\nError: %s\n", error);
}


void Logo()
{
	printf("Compression 1.0\n");
}

void Delete()
{
	if (huffmanCompression)
	{
		delete huffmanCompression;
		huffmanCompression=NULL;
	}
	if (fileName != NULL)
	{
		delete fileName;
		fileName = NULL;
	}

	if (outFile != NULL)
	{
		delete outFile;
		outFile = NULL;
	}
}
