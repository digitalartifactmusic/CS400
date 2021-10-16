#include <iostream>

#include <sstream>

#include <deque>
#include <queue>

#include <fstream>

using namespace std;

class Node
{
public:
	char character;
	int frequency;

	uint32_t code, codeLength;

	Node *left, *right;

	Node() = delete;

	Node(char character, int frequency) : character{ character }, frequency{ frequency }, left{ nullptr }, right{ nullptr } {}

	Node(int sum, Node* left, Node* right) : character{ NULL }, frequency{ sum }, left{ left }, right{ right } {}
};

class CompareNodes
{
public:
	bool operator()(Node* nodeOne, Node* nodeTwo)
	{
		return nodeOne->frequency > nodeTwo->frequency;
	}
};

Node* charMap[1024];									// Loosely keeps characters within bounds for other data structures. Should work in most cases.
														// I would use a hash map for this, but c++'s built in unordered_map was comparatively slow.
class FrequencyList										// I'm not sure how I would tackle this another way that is equally or more efficient while staying portable.
{
public:
	deque<Node> container;

	FrequencyList(string& text) : container{ }
	{
		for (char& c : text)
		{
			int i = c + 512;
			if (charMap[i] == nullptr)
			{
				container.emplace_back(c, 1);
				charMap[i] = &container.back();
			}
			else
				++charMap[i]->frequency;
		}

		cout << endl;
	}
};

class HuffmanTree
{
public:
	string result, tree;
	Node *left, *right;

	HuffmanTree() = delete;
	HuffmanTree(deque<Node>& frequencyList, string& text, ofstream& output, ofstream& encoding) : result{ "" }, tree{ "" }
	{
		priority_queue<Node*, vector<Node*>, CompareNodes> nodeQueue;

		result.reserve(text.size());

		cout << "Generating Huffman tree...\n";
		for (auto& i : frequencyList)
			nodeQueue.push(&i);

		while (nodeQueue.size() != 1)					// Using a priority queue, which sorts elements as they are added approaching O(nlog(n)) time, build the Huffman Tree starting at the two least probable nodes.
		{												// Each pair of two nodes is summed and a new node is made with this sum as the parent.
			left = nodeQueue.top();						// This parent is pushed back into the priority queue to be summed with the next pairs until the queue is empty.
			nodeQueue.pop();							// I think it would be neat to experiment with different data structures here and I will do this in my free time.
			right = nodeQueue.top();
			nodeQueue.pop();

			Node* parent = new Node(left->frequency + right->frequency, left, right);

			nodeQueue.push(parent);
		}

 		Node* temp = nodeQueue.top();

		cout << "Generating character codes...\n";
		getCodes(temp, 0, 0);

		cout << "Generating compressed data...\n";
		uint32_t data = 0, size = 0, i;					// This will be my first time really diving into bitwise operators for data manipulation and file writing so bear with me.
														// Visually this works like this:
														// Code for 'C': 101110101000, Code for 'h': 0011.
														// The codes are actually stored in reverse so data gets allocated like so: 1100 + 0001 01011101.
		for (char& c : text)
		{												// The first character in the file will be 01011101 and the second will be 11000001 and so on.
			i = c + 512;								// For decoding we can now do 00000001 & 01011101 to get the first bit and then shift one to the right instead.
			data |= charMap[i]->code << size;			// This is much more elegant than any alternative I could come up with.
			size += charMap[i]->codeLength;

			while(size >= 8)
			{
				result += (char)data;
				data >>= 8;
				size -= 8;
			}

		}
		result += (char)(data >> 24);

		cout << "Storing compressed data...\n";
		encoding.write(tree.data(), tree.size());
		output.write(result.data(), result.size());
	}

	void getCodes(Node* root, uint32_t code, uint32_t codeLength)
	{
		if (root)
		{
			getCodes(root->left, code, codeLength + 1);		// Codes are stored in reverse. Encoding scheme is stored in correct order.
			getCodes(root->right, (1 << codeLength) | code, codeLength + 1);
			if (root->character)							// If going right, a 1 is appended to the beginning of the code, otherwise the code stays the same and the code length is increased.
															// Traverses Huffman Tree in post-order building encoding scheme.
			{												// If a leaf node is encountered, '1' and the node character are added to tree encoding.
															// Otherwise, the function recurses further, looking for leaf nodes, and '0' is added to the encoding.
				root->code = code;
				root->codeLength = codeLength;

				tree += '1';
				tree += root->character;

				cout << root->character << "|" << root->frequency << "\t\t" << code << "|" << codeLength << endl;
			}
			else
				tree += '0';
		}
		else
			return;
	}
};

int main()
{
	cout << "Opening files and preparing data..." << endl;
	ifstream fin("Pride_and_Prejudice.txt", ios::binary);
	fin.seekg(0, fin.end);
	string text(fin.tellg(), 0);
	fin.seekg(0);
	fin.read(text.data(), text.size());

	ofstream result("pride.huff", ios::binary | ios::trunc);
	ofstream tree("huff.sch", ios::binary | ios::trunc);

	cout << "Generating frequency list...";
	FrequencyList fl1(text);

	HuffmanTree ht1(fl1.container, text, result, tree);

	cout << "Closing Files...\n";

	fin.close();
	result.close();
	tree.close();

	cout << "Finished!\n";

	return 0;
}
