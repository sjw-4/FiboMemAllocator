#include "my_allocator.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

typedef void* Addr;

int main() {
	int totMem = 14400;
	int basicBlockSize = 100;
	//cout << "Enter total mem size (0 to quit): "; cin >> totMem;
	//cout << "Enter basic block size (0 to quit): "; cin >> basicBlockSize;
	if (totMem == 0 || basicBlockSize == 0) return 0;
	MyAllocator test = MyAllocator(basicBlockSize, totMem);
	char inputC = 'z';
	int inputI = 1;
	Addr memAd;
	int blocksLeft;
	vector<Addr> allocatedMem;
	do {
		cout << "a - allocate | g - fill memory | f - free mem | r - free all mem | d - dump mem | t - auto test | q - quit" << endl;
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
			blocksLeft = test.getTotSize() / basicBlockSize;
			while (blocksLeft > 0) {
				int temp = rand() % (blocksLeft / 2);
				if (temp == 0) temp++;
				allocatedMem.push_back(test.Malloc(temp * basicBlockSize - (basicBlockSize / 2)));
				blocksLeft -= temp;
				if (allocatedMem.back() == nullptr) {
					blocksLeft = 0;
					allocatedMem.pop_back();
				}
			}
			cout << "--------------------------------" << endl << "Allocated memory" << endl;
			for (int i = 0; i < allocatedMem.size(); i++) {
				cout << test.getSegSize((SegmentHeader*)allocatedMem.at(i)) << ", ";
			}
			cout << endl;
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
		case 't':
			cout << "Enter number of iterations: " << endl;
			cin >> inputI;
			for (int i = 0; i < inputI; i++) {
				//Fill memory
				while (!test.isFull()) {
					int to_alloc = ((2 << (rand() % 19)) * (rand() % 100)) / 100;
					if (to_alloc < 4) to_alloc = 4;
					allocatedMem.push_back(test.Malloc(to_alloc));
					if (allocatedMem.back() == nullptr)
						allocatedMem.pop_back();
				}
				//Output allocated memory
				cout << "--------------------------------" << endl << "Allocated memory" << endl;
				for (int i = 0; i < allocatedMem.size(); i++) {
					cout << test.getSegSize((SegmentHeader*)allocatedMem.at(i)) << ", ";
				}
				cout << endl;
				//Free allocated memory
				for (int i = 0; i < allocatedMem.size(); i++) {
					if (allocatedMem.at(i) == nullptr || !test.Free(allocatedMem.at(i)))
						cout << "Failed to free mem, ";
				}
				//Clear memory
				allocatedMem.clear();
				//Check for issues with combining segs
				test.dumpFls();
			}
			break;
		case 'r':
			for (int i = 0; i < allocatedMem.size(); i++) {
				if (allocatedMem.at(i) == nullptr || !test.Free(allocatedMem.at(i)))
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