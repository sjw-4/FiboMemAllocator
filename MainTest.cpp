#include "my_allocator.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

typedef void* Addr;

int main() {
	int totMem = 2100;
	int basicBlockSize = 100;
	//cout << "Enter total mem size (0 to quit): "; cin >> totMem;
	//cout << "Enter basic block size (0 to quit): "; cin >> basicBlockSize;
	if (totMem == 0 || basicBlockSize == 0) return 0;
	MyAllocator test = MyAllocator(basicBlockSize, totMem);
	char inputC = 'z';
	int inputI = 1;
	Addr memAd;
	vector<Addr> allocatedMem;
	do {
		cout << "a - allocate | g - fill memory (must be 2100 allocated) | f - free mem | r - free all mem | d - dump mem | q - quit" << endl;
		cin >> inputC;
		switch (inputC) {
		case 'a':
			cout << "Enter mem to allocate" << endl;
			cin >> inputI;
			memAd = test.Malloc(inputI);
			if (memAd != 0) {
				cout << "Mem address: " << memAd << endl;
				allocatedMem.push_back(memAd);
			}
			else cout << "Memory full" << endl;
			break;
		case 'g':
			allocatedMem.push_back(test.Malloc(450));
			allocatedMem.push_back(test.Malloc(50));
			allocatedMem.push_back(test.Malloc(50));
			allocatedMem.push_back(test.Malloc(50));
			allocatedMem.push_back(test.Malloc(250));
			allocatedMem.push_back(test.Malloc(750));
			allocatedMem.push_back(test.Malloc(150));
			break;
		case 'f':
			cout << "Enter mem address to free" << endl;
			cin >> memAd;
			if (test.Free(memAd)) cout << "Free successful" << endl;
			else cout << "Error freeing memory address" << endl;
			break;
		case 'd':
			test.dumpFls();
			break;
		case 'r':
			for (int i = 0; i < allocatedMem.size(); i++) {
				if (test.Free(allocatedMem.at(i)))
					cout << "Mem freed, ";
				else
					cout << "Failed to free mem, ";
			}
			cout << endl;
			allocatedMem.clear();
			break;
		default:
			inputC = 'q';
			inputI = 0;
			break;
		}
	} while (inputC != 'q' && inputI != 0);
	return 0;
}