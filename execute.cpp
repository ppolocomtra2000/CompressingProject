#include "execute.h"


void execute()
{
	int choice;
	cout << "===========COMPRESSING TOOLS============" << endl;
	cout << "1. Compressing file" << endl;
	cout << "2. Decompressing file" << endl;
	cout <<	"3. Compressing folder"<<endl;
	cout << "4. Decompressing folder"<<endl;
	cout << "5. Exit" << endl;
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
		cout<<"====================="<<endl;
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
		cout<<"====================="<<endl;
		cout << "Decompressing successfully !!" << endl;
		cout << "Time duration: " << (double)duration / 1000 << " s" << endl;
		cout << "File name after decompress: " << outFile << endl;
		break;
	}
	case 3:
		compressFolder();
		break;
	case 4:
		decompressFolder();
		break;
	default:
		exit(0);
	}
}

void compressFolder()
{
	vector <string> filesInclude;
	string folder;
	cout << "Enter your folder (D:\\...\\...\\) : ";
	cin.ignore();
	getline(cin, folder);
	//traverse all files include folder
	//add to vector filesInclude
	path p(folder);
	directory_iterator end_itr;
	for (directory_iterator itr(p); itr != end_itr; ++itr)
	{
		if (is_regular_file(itr->path()))
		{
			string current_file = itr->path().string();
			filesInclude.push_back(current_file);
		}
	}
	//create new folder
	if (_wmkdir((wchar_t*)L"Hello") != 0)
	{
		perror("Error creating file");
	}
	else
	{
		puts("File successfully created");
	}
	//Hard folder path
	string newFolderPath = "D:\\Visual Studio\\CompressionHuffman\\Project1\\Hello";
	//init vector Decompress
	vector <string> filesDecompress;
	for (int i = 0; i < filesInclude.size(); i++)
	{
		filesDecompress.push_back(newFolderPath+"\\newEncoding" + to_string(i) + ".bin");
	}
	//compress
	for (int i = 0; i < filesInclude.size(); i++)
	{
		huffman h(filesInclude[i], filesDecompress[i]);
		h.createPq();
		h.createHuffmanTree();
		h.calculateHufmanCodes();
		h.codingSave();
		cout << "Compressing..." << endl;
	}
	cout << "All done!" << endl;
	cout << "Your new folder: " << newFolderPath << endl;
}

void decompressFolder()
{
	string folder;
	cout << "Enter folder name to decompress (D:\\...\\...\\...): ";
	vector <string> filesInclude;
	cin.ignore();
	getline(cin, folder);
	//traverse folder to get files
	path p(folder);
	directory_iterator end_itr;
	for (directory_iterator itr(p); itr != end_itr; ++itr)
	{
		if (is_regular_file(itr->path()))
		{
			string current_file = itr->path().string();
			filesInclude.push_back(current_file);
		}
	}
	//create new folder
	if (_wmkdir((wchar_t*)L"newDecodingFolder") != 0)
	{
		perror("Error creating file");
	}
	else
	{
		puts("File successfully created");
	}
	//Hard folder path
	string newFolderPath = "D:\\Visual Studio\\CompressionHuffman\\Project1\\newDecodingFolder";
	//init vector Decompress
	vector <string> filesDecompress;
	for (int i = 0; i < filesInclude.size(); i++)
	{
		std::ofstream fout(newFolderPath + "\\newDecoding" + to_string(i) + ".txt");
		filesDecompress.push_back(newFolderPath + "\\newDecoding" + to_string(i) + ".txt");
	}
	//decompress
	for (int i = 0; i < filesInclude.size(); i++)
	{
		huffman h(filesInclude[i], filesDecompress[i]);
		h.recreateHuffmanTree();
		h.decodingSave();
		cout << "Decompressing..." << endl;
	}
	cout << "All done!" << endl;
	cout << "Your new folder: " << newFolderPath << endl;
}
