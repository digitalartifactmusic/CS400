// CS 400 LAB 1
// Dynamic Array RoT
// Noah Grassmeyer

#include <iostream>
#include <time.h>

class DynamicArray
{
private:

	int* _p;
	size_t _size;

	uint64_t _seed = time(0) * 0xd5bed23087190ff3;
	uint64_t _t = _seed * 0x56997494b8af43a0, s0, s1;

public:

	DynamicArray() : _p{ new int[10] }, _size{ 10 } {}

	DynamicArray(size_t size) : _p{ new int[size] }, _size{ size } {}

	DynamicArray(const DynamicArray& that)	// copy constructor
	{
		this->_size = that._size;

		this->_p = new int[_size];
		for (int i = 0; i < _size; i++)		// deep copy
		{
			this->_p[i] = that._p[i];
		}
	}

	DynamicArray& operator = (const DynamicArray& that)	// copy assignment operator
	{
		if (this != &that)
		{
			this->_size = that._size;

			this->_p = new int[_size];
			for (int i = 0; i < _size; i++)
			{
				this->_p[i] = that._p[i];
			}
		}

		return *this;
	}

	void fillRandom()
	{
		for (int i = 0; i < _size; i++)
		{
			s0 = _seed;
			s1 = _t;

			s1 ^= s0;
			_seed = ((s0 << 24) | (s0 >> (64 - 24))) ^ s1 ^ (s1 << 16);	// xoroshiro128+ pseudo random number generator
			_t = (s1 << 37) | (s1 >> (64 - 37));

			_p[i] = s0 + s1;
		}
	}

	void print()
	{
		for (int i = 0; i < _size; i++)
		{
			std::cout << _p[i] << " ";
		}
	}

	void sort()
	{
		if (_size > 1)
		{
			for (int i = 1; i < _size; i++)
			{
				int temp;

				int* itr = _p + i;

				while ((*itr > *(itr - 1)) && (itr != _p)) // insertion sort
				{
					temp = *itr;
					*itr = *(itr - 1);
					*(--itr) = temp;
				}
			}
		}
	}

	~DynamicArray()
	{
		delete[] _p;
	}
};

int main()
{
	DynamicArray d1;

	d1.fillRandom();
	d1.print();

	d1.sort();

	std::cout << "\n\n";

	d1.print();

	return 0;
}
