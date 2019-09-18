#include "my_allocator.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

typedef void* Addr;

int main() {
	int totMem;
	cout << "Enter total mem size: "; cin >> totMem;
	MyAllocator test = MyAllocator(10, totMem);
	char inputC = 'z';
	int inputI = 1;
	Addr memAd;
	Addr temp;
	void* temps[20];
	int tempsI = 0;
	int memLeft = totMem - 20;
	do {
		cout << "a - allocate | f - free mem | m - get free mem left | q - quit" << endl;
		cin >> inputC;
		switch (inputC) {
		case 'a':
			cout << "Enter mem to allocate" << endl;
			cin >> inputI;
			temp = test.Malloc(inputI);
			if (temp != 0) {
				cout << "Mem address: " << temp << endl;
				temps[tempsI] = temp;
				tempsI++;
				memLeft -= test.memAtAddr(temp);
			}
			else cout << "Memory full" << endl;
			break;
		case 'f':
			cout << "Enter mem address to free" << endl;
			cin >> memAd;
			memLeft += test.memAtAddr(memAd);
			if (test.Free(memAd)) cout << "Free successful" << endl;
			else cout << "Error freeing memory address" << endl;
			break;
		case 'm':
			cout << "Free memory left: " << memLeft << endl;
			break;
		case 'z':
			for (int i = 0; i < tempsI; i++) {
				test.Free(temps[i]);
			}
			tempsI = 0;
			break;
		default:
			inputC = 'q';
			inputI = 0;
			break;
		}
	} while (inputC != 'q' && inputI != 0);
	return 0;
}