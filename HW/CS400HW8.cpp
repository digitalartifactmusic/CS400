#include <iostream>

#include <stack>

#include <fstream>

using namespace std;

class Node
{
public:
	char character;

	Node *left, *right;

	Node() = delete;

	Node(char character) : character{ character }, left{ nullptr }, right{ nullptr } {}

	Node(Node* left, Node* right) : character{ NULL }, left{ left }, right{ right } {}
};

class HuffmanDecompress
{
public:
	string result;

	HuffmanDecompress(string& compressed, string& tree, ofstream& decompressed) : result{ "" } 
	{
		stack<Node*> treeStack;


		cout << "Building Huffman tree from schematic...\n";
		for (auto itr = tree.begin(); itr != tree.end(); ++itr)
		{
			if (*itr == '1')								// The schematic is used to build the Huffman tree before compression. If a 1 is encountered, then the next character
			{												// is put into a leaf node and pushed to the stack. Otherwise a 0 must've been read by convention.
				Node* leaf = new Node(*++itr);				// In this case the two top nodes of the stack are popped and put into a parent node which is pushed to the stack.
				treeStack.push(leaf);						// Eventually, once the end of the schematic has been reached, there will only be one Node in the stack, the root.
			}
			else
			{
				Node* right = treeStack.top();
				treeStack.pop();
				Node* left = treeStack.top();
				treeStack.pop();

				Node* parent = new Node(left, right);

				treeStack.push(parent);
			}
		}

		cout << "Decompressing file...\n";
		Node* currentNode = treeStack.top();				// For each character in the file the bits are read, moving left for 0s and right for 1s, until a leaf node is encountered. 
		for (char& c : compressed)							// Once a leaf node is encountered the character present is stored in result and the current node is set back to tree root.
		{
			for (unsigned i = 0; i < 8; ++i)
			{
				if (0x00000001 & c)
					currentNode = currentNode->right;
				else
					currentNode = currentNode->left;

				c >>= 1;

				if (currentNode->character)
				{
					result += currentNode->character;
					currentNode = treeStack.top();
				}
			}
		}
		
		decompressed.write(result.data(), result.size());	// Store decompressed file.
	}
};

int main()
{
	cout << "Opening files and preparing data...\n";
	ifstream fin1("pride.huff", ios::binary);

	fin1.seekg(0, fin1.end);
	string compressed(fin1.tellg(), 0);
	fin1.seekg(0);
	fin1.read(compressed.data(), compressed.size());

	ifstream fin2("huff.sch", ios::binary);

	fin2.seekg(0, fin2.end);
	string tree(fin2.tellg(), 0);
	fin2.seekg(0);
	fin2.read(tree.data(), tree.size());

	ofstream decompressed("decompressed.txt", ios::binary | ios::trunc);

	HuffmanDecompress(compressed, tree, decompressed);

	cout << "Closing files...\n";
	fin1.close();
	decompressed.close();
	fin2.close();

	cout << "Finished!\n";

	return 0;
}
