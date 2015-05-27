#ifndef huffman_h
#define huffman_h

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "tree.h"
#include <vector>
#include "lettercount.h"
#include "node.h"
#include <math.h>

using namespace std;

const int SIGN_SIZE=2;
const char SIGN[] = "QC";

bool compareByLength(const lettercount & a, const lettercount & b);

class huffman
{
  private:
	int Filesize;
	tree *HTree;
	int Count;
	unsigned char results;
	int resultscount;
	vector < lettercount > *letters;
	unsigned char reverseBits(unsigned char bits, int maxHeight);
	bool  SearchTree(unsigned char letter, node* n, bool direction);
  public:
	huffman();
	 ~huffman();
	int GetSize();	
	bool  GetCharsFromFile(char *filename);
	void PrintAllWeight();
	void PrintTree(node * n);
	bool Compress(char* filename, char* outFile);
	bool Decompress(char* filename, char* outFile);
	void GetProb();
	unsigned char Decode(unsigned char character);
	unsigned char Encode(unsigned char character);
	int GetFileSize(char* filename);
	bool isQCompression(char* sign);

};

huffman::huffman()
{
	Filesize = 0;
	resultscount=0;
	letters=NULL;
	HTree=NULL;
	results = 0x00;
	Count=0;
	results=0;
	resultscount=0;
	letters=NULL;
}

huffman::~huffman()
{
	if (letters != NULL)
	{
		delete letters;
		letters = NULL;
	}
	if (HTree != NULL)
	{
		delete HTree;
		HTree = NULL;
    	}
}

int huffman::GetSize()
{
	return Filesize;
}

void huffman::PrintAllWeight()
{
/*
	=================================================
	This is for debugging purposes.
	=================================================
	*/
	for (int j = 0; j < letters->size(); j++)
	{
		printf("letter: %x count %i prob: %f\n", letters->at(j).letter,letters->at(j).count,letters->at(j).prob);
	}

	printf("\n\nsize: %i\n\n", letters->size());
}

void huffman::PrintTree(node * n)
{
	if (n->left == NULL && n->right == NULL)
	{
    		printf("root: %i\n", n->GetLetter());
	}
	if (n->left != NULL) PrintTree(n->left);
	if (n->right != NULL) PrintTree(n->right);
}

void huffman::GetProb()
{
	/*
	=================================================
	For debugging purpose
	=================================================
	*/
	int totalLetterCount=0;

	for (int i=0;i< letters->size(); i++)
	{
		totalLetterCount = totalLetterCount +  letters->at(i).count;
	}
        for (int j=0;j< letters->size(); j++)
        {
                letters->at(j).prob =  (double) letters->at(j).count / totalLetterCount;
        }
}

bool huffman::GetCharsFromFile(char *filename)
{
	/*
	=================================================
	We need to get all the characters that is used
	so we can build the tree.
	=================================================
	*/
	FILE *file = NULL;
	bool found = false;
	char charFromFile=0;

	file = fopen(filename, "rb");
	if (file == NULL)
	{
		printf("file not found\n");
		return false;
	}

	while (!feof(file))
	{
		charFromFile = fgetc(file);
		found = false;
		for (int i = 0; i < letters->size(); i++)
		{
			lettercount *letterCountNode = (lettercount *) & letters->at(i);
			if (letterCountNode->letter == (unsigned char)charFromFile)
			{
				found = true;
				letterCountNode->count++;
				break;
			}

		}
		if (found == false)
		{
			lettercount newLetterCountNode;
			newLetterCountNode.letter = (unsigned char)charFromFile;
			newLetterCountNode.count++;
			letters->push_back(newLetterCountNode);
		}
		Filesize++;
	}
	fclose(file);
	return true;
}




bool huffman::SearchTree(unsigned char letter, node* n, bool direction)
{
	/*
	=================================================
	In order to compressed the data we must find
	the path to the character.
	=================================================
	*/
	bool found = false;

	if (n->left != NULL)
	{
		found = SearchTree(letter,n->left,true);
	}
	if (n->right != NULL && found == false)
	{
		found = SearchTree(letter, n->right,false);
	}
	if (found == true)
	{
		if (HTree->root == n) return true;
		results <<= 1;
		results |= direction;
		resultscount++;
	
	}
	if (letter == n->GetLetter())
	{
		results <<= 1;
		results |= direction;
		resultscount++;
		found = true;	
	}

	return found;
}



unsigned char huffman::Decode(unsigned char character)
{
	node* temp = HTree->root;
	unsigned char bit=0;
	Count=0;


	while (temp != NULL)
	{
		bit = character;
		bit >>= Count;
	
		bit <<= 7;
		
		bit >>= 7;
		
		
		if (bit == 1)
		{
			if (temp->left != NULL)
			{
				temp = temp->left;
			}
		}
		if (bit == 0)
		{
			if (temp->right != NULL)
			{
				temp = temp->right;
			}
		}

		if (temp->left == NULL && temp->right == NULL)
		{
			return temp->GetLetter();
		}
		Count++;
	}
}


unsigned char huffman::Encode(unsigned char character)
{
	results = 0;
	SearchTree(character, HTree->root,0);
	return results;
}

bool huffman::Compress(char* filename, char* outFile)
{
	/*
		Algorthmn to write compressed data to a file

		while not End of file
		set count to zero
		set byteCount to zero
		set decodedVar variable to zero
		Get character from file and put in gotChar variable
		while count not equal to Tree's Height
			Shift decodedVar variable by one to the left
			Set oneByte variable to gotChar variable
			Shift oneByte variable to the right by count
			Shift oneByte variable to the left by seven
			Shift oneByte variable to the right by seven
			or oneByte to decoded variable
			increase byteCount by one
			increase  count by one
			if byteCount equal eight then
				write decodedVar to file
				set decoded variable to zero
				set bytecount to zero
	*/	
	FILE *origFile=NULL;
	FILE *newFile=NULL;
	char charFromFile=0;
	unsigned char decodedChar=0;
	unsigned char encodedChar=0;
	int count=0;
	int byteCount=0;
	unsigned char  oneByte=0;
	int fileCount=0;
	int fileSize=GetFileSize(filename);
	int treeMaxHeight=0;

	letters = new vector < lettercount >;

	if (letters == NULL) return false;

	HTree = new tree();
	if (HTree == NULL) return false;

	if (GetCharsFromFile(filename) == false) return false;
	GetProb();

	newFile = fopen(outFile,"w");
	if (newFile == NULL) return false;

	fputc(SIGN[0], newFile);
	fputc(SIGN[1], newFile);

	if (letters->size() == 256)
	{
		letters->clear();
		fputc(0,newFile);
	
		for (int i=0;i<256;i++)
		{
                        lettercount newLetterCountNode;
                        newLetterCountNode.letter = (char) i;
                        newLetterCountNode.count++;
                        letters->push_back(newLetterCountNode);
		}
	} else {
		fputc(letters->size(),newFile);

		/*put all the letters that are used*/
		for (int i=0;i< letters->size();i++)
		{
			fputc(letters->at(i).letter,newFile);
		}
	}

	HTree->BuildTree(letters);

	origFile = fopen(filename,"rb");
	if (origFile == NULL)
	{
		printf("Error opening file\n ");
		return false;
	}
	count=0;
	byteCount=0;

	treeMaxHeight = HTree->GetMaxHeight();

	while (!feof(origFile))
	{
		charFromFile = fgetc(origFile);
		encodedChar = Encode(charFromFile);
		count=0;
		while (count != treeMaxHeight)
		{
			decodedChar <<= 1;
			oneByte = encodedChar;
			oneByte >>= count;
			oneByte <<= 7;
			oneByte >>=7;
			decodedChar |= oneByte;
			byteCount++;
			count++;
			if (byteCount == 8)
			{
				fputc(decodedChar,newFile);
				byteCount=0;
				decodedChar = 0;
			}
		} 
		fileCount++;
		if (fileCount == fileSize) break;	
	}
	return true;
}


bool huffman::Decompress(char* filename, char* outFile)
{
	/*
		Header Infomation:
	First two bytes "QE" (4 bytes)  is the Header signature.
	Second is the number of character stored (2 Bytes)
	The next set of bytes is the characters in the file (depending on how many character. Max=255)
	The next bytes is the actually data

		Decompress Algorthm: 
	Open decompressed file
	Get the first two bytes and put it in sig
	Check sig is valid
	Get the third byte of the file (total letters) and put it in totalLetters
	Go though a loop and get the letters from the file and put them into an array
	Create Tree from those letters
	
	Set byteCount to zero
	Set count to zero
	Set unsigned decodedChar to zero
	Set unsigned encodedChar to zero
	Set charFromFile to zero
	Set unsigned oneByte

	while not end of file
		set byteCount to zero
		Get byte from file and store it in charFromFile
		while byteCount not eight
			Store charFromFile to oneByte
			Shift onebyte to the left by byteCount
			Shift onebyte  to the right by seven
			Shift encodedChar by oneByte
			Or encodedChar with count
			Increase byteCount by one
			Increase count by one
			if count is equal to Max Tree Height
				Decode encodedChar into results
				write to results to file
				set encodedChar to zero
				set count to zero
				
				 
		
	*/
	FILE* codedFile=NULL;
	FILE* decodedFile=NULL;
	char sig[1]={};
	int totalLetters=0;
	char charFromFile=0;
	int byteCount=0;
	int count=0;
	unsigned char decodedChar=0;
	unsigned char encodedChar=0;
	unsigned char oneByte=0;
	unsigned char reversedBit=0;
	//int fileCount=0;
	int fileSize=GetFileSize(filename);
	int treeMaxHeight=0;

	sig[0] = '\0';
	sig[1] = '\0';

	HTree = new tree();

	if (HTree == NULL) return false;

	codedFile = fopen(filename,"rb");

	if (codedFile != NULL)
	{

		for (int k=0;k<SIGN_SIZE;k++)
		{
			sig[k] = fgetc(codedFile);
		}

		if (!isQCompression(sig))
		{
			printf("Not a compression file\n");
			return false;
		}

		totalLetters = fgetc(codedFile);

		letters = new vector < lettercount >;

		if (letters == NULL) return false;

		if (totalLetters == 0)
		{
			for (int i=0;i<256;i++)
			{
				lettercount add;
                                add.letter = (unsigned char) i;
                                letters->push_back(add);
			}
		} else {
			for (int i=0; i < totalLetters; i++)
			{
				lettercount add;
				add.letter = (unsigned char) fgetc(codedFile);
				letters->push_back(add);
			}
		}

		HTree->BuildTree(letters);

		decodedFile = fopen(outFile,"w");


		treeMaxHeight = HTree->GetMaxHeight();

		while (!feof(codedFile))
		{
			byteCount = 0;
			encodedChar = fgetc(codedFile);
			while (byteCount != 8)
			{
				oneByte = encodedChar;
				oneByte <<= byteCount;
				oneByte >>= 7;
				decodedChar <<= 1;
				decodedChar |= oneByte;
				byteCount++;
				count++;
				if (count == treeMaxHeight)
				{
					reversedBit = reverseBits(decodedChar, treeMaxHeight);
				
					fputc(Decode(reversedBit),decodedFile);
					count = 0;
					decodedChar=0;
					reversedBit = 0;
				}
			}
		}

	} else {
		printf("unable to open file.\n");
	}

	return true;
}	

bool huffman::isQCompression(char* sign)
{
	for (int i=0; i < SIGN_SIZE; i++)
	{
		if (SIGN[i] != sign[i])
		{
			return false;
		}
	}	
	return true;
}


int huffman::GetFileSize(char* filename)
{
        FILE* fileForSize = fopen(filename, "rb");
	if (fileForSize == NULL) return -1;
        fseek(fileForSize,0,SEEK_END);
        return ftell(fileForSize);
}

unsigned char huffman::reverseBits(unsigned char bits, int maxHeight)
{
	/*
		1000
		0001

		Set bitCount to 0
		
		while bitCount not equal to 8
			set bit to bits
			shift bit to the right by byte count
			shift bit to the left by seven
			shift bit to the right by seven
			shift reversedBits to the left by one
			or bit with reversedBit
			clear bit
			increase bitcount
		
	*/
	int bitCount=0;
	unsigned char bit=0;
	unsigned char reverseBits=0;
	//int maxHeight = HTree->GetMaxHeight();
		
	while (bitCount != maxHeight)
	{
		bit = bits;
		bit >>= bitCount;
		bit <<= 7;
		bit >>= 7;
		reverseBits <<= 1;
		reverseBits |= bit;
		bit=0;
		bitCount++;
	}
	return reverseBits;
}


bool compareByLength(const lettercount & a, const lettercount & b)
{ 
	return (double)a.prob < (double)b.prob;
}



#endif
