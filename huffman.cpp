#include "huffman.h"

void huffman::createNodeArray()
{
	//128 stands for 128 basic ASCII 
	for (int i = 0; i < 128; i++)
	{
		nodeArray[i] = new huffman_node;
		nodeArray[i]->id = i;
		nodeArray[i]->freq = 0;
	}
}
//using recursive to get each character's code
void huffman::traverse(huffNode node, string code)
{
	if (node->left == NULL && node->right == NULL)
		node->code = code;
	else
	{
		traverse(node->left, code + '0');
		traverse(node->right, code + '1');
	}
}

int huffman::binaryToDecimal(string& in)
{
	int result = 0;
	for (int i = 0; i < in.size(); i++)
		result = result * 2 + in[i] - '0';
	return result;
}

string huffman::decimalToBinary(int in)
{
	string temp = "";
	string result = "";
	while (in)
	{
		temp += ('0' + in % 2);
		in /= 2;
	}
	result.append(8 - temp.size(), '0');
	for (int i = temp.size()-1; i >= 0; i--)
		result += temp[i];
	return result;
}

inline void huffman::buildTree(string& path, char aCode)
{
	huffNode current = root;
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '0')
		{
			if (current->left == NULL)
				current->left = new huffman_node;
			current = current->left;
		}
		else if (path[i] == '1')
		{
			if (current->right == NULL)
				current->right = new huffman_node;
			current = current->right;
		}
	}
	current->id = aCode;
}
//init file name 
huffman::huffman(string in, string out)
{
	finName = in;
	foutName = out;
	createNodeArray();
}
//encoding
void huffman::createPq()
{
	fin.open(finName, ios::in);
	if (!fin)
	{
		cerr << "Error!" << endl;
		return;
	}
	fin.get(id);
	//get character (id) by character
	while (!fin.eof())
	{
		nodeArray[id]->freq++;
		fin.get(id);
	}
	fin.close();
	//priority queue: the less frequency each character the higher priority it takes in the queue
	for (int i = 0; i < 128; i++)
	{
		if (nodeArray[i]->freq)
			pq.push(nodeArray[i]);
	}
}
//init Huffman tree by definiton
void huffman::createHuffmanTree()
{
	priority_queue<huffNode, vector<huffNode>, compare> temp(pq);
	while (temp.size() > 1)
	{
		root = new huffman_node;
		root->freq = 0;
		root->left = temp.top();
		root->freq += temp.top()->freq;
		temp.pop();
		root->right = temp.top();
		root->freq += temp.top()->freq;
		temp.pop();
		temp.push(root);
	}
}
//get table code
void huffman::calculateHufmanCodes()
{
	traverse(root, "");
}
//encoding
void huffman::codingSave()
{
	fin.open(finName, ios::in);
	fout.open(foutName, ios::out | ios::binary);
	if (!fin||!fout)
	{
		cerr << "Error!" << endl;
		return;
	}
	string in = "", s = "";

	in += (char)pq.size();
	priority_queue<huffNode, vector<huffNode>, compare> temp(pq);
	while (!temp.empty())
	{
		huffNode current = temp.top();
		in += current->id;
		//Replaces the current value by n consecutive copies of character c.
		s.assign(127 - current->code.size(), '0');
		s += '1';
		s.append(current->code);
		in += (char)binaryToDecimal((string&)s.substr(0,8));
		for (int i = 0; i < 15; i++)
		{
			s = s.substr(8);
			in += (char)binaryToDecimal((string&)s.substr(0, 8));
		}
		temp.pop();
	}
	s.clear();

	fin.get(id);
	while (!fin.eof())
	{
		s += nodeArray[id]->code;
		while (s.size() > 8)
		{
			in += (char)binaryToDecimal((string&)s.substr(0, 8));
			s = s.substr(8);
		}
		fin.get(id);
	}
	int count = 8 - s.size();
	if (s.size() < 8)
	{
		s.append(count, '0');
	}
	in += (char)binaryToDecimal(s);
	in += (char)count;

	fout.write(in.c_str(), in.size());
	fin.close();
	fout.close();
}
//decoding
void huffman::recreateHuffmanTree()
{
	fin.open(finName, ios::in | ios::binary);
	if (!fin)
	{
		cerr << finName << " can't open!" << endl;
		return;
	}
	unsigned char size;
	//reinterpret_cast: type_casting ( hardly error ) ( pointer to pointer)																			
	fin.read(reinterpret_cast<char*>(&size), 1);
	root = new huffman_node;
	for (int i = 0; i < size; i++)
	{
		char newCode;
		unsigned char codeCharacter[16];																
		fin.read(&newCode, 1);
		fin.read(reinterpret_cast<char*>(codeCharacter), 16);
		string codeString = "";
		for (int i = 0; i < 16; i++)
		{
			codeString += decimalToBinary(codeCharacter[i]);
		}
		int j = 0;
		while (codeString[j] == '0')
		{
			j++;
		}
		codeString = codeString.substr(j + 1);
		buildTree(codeString, newCode);
	}
	fin.close();
}

void huffman::decodingSave()
{
	fin.open(finName, ios::in | ios::binary);
	fout.open(foutName, ios::out);
	if (!fin || !fout)
	{
		cerr << "Error!" << endl;
		return;
	}
	unsigned char size;
	//get the size of huffman tree																		
	fin.read(reinterpret_cast<char*>(&size), 1);
	//jump to the last one byte to get the number of '0' append to the string at last
	fin.seekg(-1, ios::end);															
	char countZero;
	fin.read(&countZero, 1);
	//jump to the position where text starts
	fin.seekg((1 + 17 * size), ios::beg);													
	vector<unsigned char> text;
	unsigned char textseg;
	fin.read(reinterpret_cast<char*>(&textseg), 1);
	while (!fin.eof())
	{//get the text byte by byte using unsigned char
		text.push_back(textseg);
		fin.read(reinterpret_cast<char*>(&textseg), 1);
	}
	huffNode current = root;
	string path;
	for (int i = 0; i < text.size() - 1; i++)
	{//translate the huffman code
		path = decimalToBinary(text[i]);
		if (i == text.size() - 2)
			path = path.substr(0, 8 - countZero);
		for (int j = 0; j < path.size(); j++)
		{
				if (path[j] == '0')
				{
					current = current->left;
				}
				else{
					current = current->right;
				}
				if (current->left==NULL  && current->right==NULL )
				{
					fout.put(current->id);
					current = root;
				}
		}
	}
	fin.close();
	fout.close();
}