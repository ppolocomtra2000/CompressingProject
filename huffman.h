#pragma once
#include <bits/stdc++.h>

using namespace std;

struct huffman_node {
	char id;
	int freq;
	string code;
	huffman_node* left;
	huffman_node* right;
	//constructor
	huffman_node() {
		left = right = NULL;
	}
};

typedef huffman_node* huffNode;

class huffman {
protected:
	huffNode nodeArray[128];
	fstream fin, fout;
	huffNode child, parent, root;
	char id;
	string finName, foutName;
	class compare {
	public:
		bool operator() ( const huffNode& c1, const huffNode& c2 )const {
			return c1->freq > c2->freq;
		}
	};
	priority_queue<huffNode, vector<huffNode>, compare> pq;
	//init Table Code
	void createNodeArray();
	void traverse(huffNode, string);
	//convert between Binary and Decimal
	int binaryToDecimal(string&);
	string decimalToBinary(int);
	//init tree
	inline void buildTree(string&, char);
public:
	//constructor
	huffman(string, string);
	//init priority queue
	void createPq();
	//init Huffman tree
	void createHuffmanTree();
	//Codes table
	void calculateHufmanCodes();
	//encoding
	void codingSave();
	//decoding
	void decodingSave();
	void recreateHuffmanTree();
};