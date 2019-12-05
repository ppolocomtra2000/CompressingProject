#include "execute.h"

void execute()
{
	int choice;
	cout << "===========COMPRESSING TOOLS============" << endl;
	cout << "1. Compressing file" << endl;
	cout << "2. Decompressing file" << endl;
	cout << "3. Exit" << endl;
	cout << "------------------" << endl;
	cout << "Which option do you want: ";
	cin >> choice;
	switch (choice)
	{
	case 1:
	{	//encoding
		string inFile, outFile;
		cout << "Enter your file name to compress: ";
		cin.ignore();
		getline(cin, inFile);
		outFile = "newEncoding.bin";
		clock_t start = clock();
		cout << "Compressing..." << endl;
		huffman h(inFile+".txt", outFile);
		h.createPq();
		h.createHuffmanTree();
		h.calculateHufmanCodes();
		h.codingSave();
		clock_t finish = clock();
		double duration = (double)finish - start;
		cout << "Compressing successfully !!"  << endl;
		cout << "Time duration: " << (double)duration / 1000 << " s" << endl;
		cout << "File name after compress: " << outFile << endl;
		break;
	}
	case 2:
	{	//decoding
		string inFile, outFile;
		cout << "Enter your file name to decompress: ";
		cin.ignore();
		getline(cin, inFile);
		outFile = "newDecoding.txt";
		clock_t start = clock();
		cout << "Decompressing..." << endl;
		huffman h(inFile + ".bin", outFile);
		h.recreateHuffmanTree();
		h.decodingSave();
		clock_t finish = clock();
		double duration = (double)finish - start;
		cout << "Decompressing successfully !!" << endl;
		cout << "Time duration: " << (double)duration / 1000 << " s" << endl;
		cout << "File name after decompress: " << outFile << endl;
		break;
	}
	default:
		exit(0);
	}
}
