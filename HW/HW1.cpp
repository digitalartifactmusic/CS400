#include <iostream>
#include <cmath>
#include <vector>

#include <chrono>

#include <thread>

using namespace std;

static unsigned AVAILABLE_THREADS = thread::hardware_concurrency();

static vector<thread> THREADS(AVAILABLE_THREADS);

bool isPrime(int n)
{
	if (n <= 1)													// Reject trivial non-primes.
		return 0;

	for (int i = 2; i * i <= n; ++i)							// Optimize square root.
		if (n % i == 0)
			return 0;

	return 1;
}

class ConsecutiveFactorial
{
private:

	int _aLowerBound, _bLowerBound, _aUpperBound, _bUpperBound;

	vector<int> _primes;										// If n = 0, n^2 + an + b = b, so b must be prime.

public:

	vector<tuple<int, int, int>> data;

	ConsecutiveFactorial() : _aLowerBound{ 0 }, _bLowerBound{ 0 }, _aUpperBound{ 0 }, _bUpperBound{ 0 }  
	{
		cout << "Please enter lower and upper bounds for a and b respectively, seperating each value by a space: ";
			cin >> _aLowerBound;
			cin >> _aUpperBound;
			cin >> _bLowerBound;
			cin >> _bUpperBound;
	}

	ConsecutiveFactorial(int aLowerBound, int bLowerBound, int aUpperBound, int bUpperBound) : _aLowerBound{ aLowerBound }, _bLowerBound{ bLowerBound }, _aUpperBound{ aUpperBound }, _bUpperBound{ bUpperBound }  {}

	void init()													// Store all primes in the range specified for b.
	{
		if (!(_aLowerBound % 2))								// If n = 1, n^2 + an + b = a + b. Because b is prime, a must be odd for n = 1 to be prime.
			--_aLowerBound;

		for (int i = 0; i < _bUpperBound; ++i)					// Assuming b is always positive as the graph doesnt seem like it fits when n is negative.
		{
			if (isPrime(abs(i)))
				_primes.push_back(i);
		}

		for (unsigned i = 0; i < AVAILABLE_THREADS; ++i)
		{
			data.push_back(tuple<int, int, int>(0, 0, 0));
		}
	}

	void core(int thread)
	{
		vector<int>::iterator pItr = _primes.begin() + thread;
		while (pItr < _primes.end())
		{
			for (int a = _aLowerBound; a <= _aUpperBound; a += 2)
			{
				int n = 2;

				while (isPrime((n * n) + (a * n) + *pItr))
					++n;

				if (n > get<2>(data[thread]))
					data[thread] = tuple<int, int, int>(a, *pItr, n);
			}
			pItr += AVAILABLE_THREADS;
		}
	}

	void execute()
	{
		int n = -1;
		auto start = chrono::high_resolution_clock::now();
		for (auto& i : THREADS)
		{
			i = thread{ &ConsecutiveFactorial::core, this, ++n };
		}
		for (auto& i : THREADS)
		{
			i.join();
		}
		cout << chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << "ms" << endl;

		for (auto& i : data)
			cout << "\n\n Consecutive Primes: " << get<2>(i) << "\t\ta: " << get<0>(i) << "\t\tb: " << get<1>(i) << endl;
	}
};


int main()
{
	ConsecutiveFactorial cf1{ -999 , -1000, 999 , 1000 };

	cf1.init();
	cf1.execute();

	cout << endl << endl;

	for (unsigned i = 0; i < 40; ++i)
	{
		cout << (i * i) + i + 41 << endl;
	}

	cout << endl << endl;

	for (unsigned i = 0; i < 80; ++i)
	{
		cout << ((i * i) - (79 * i)) + 1601 << endl;
	}

	return 0;
}
