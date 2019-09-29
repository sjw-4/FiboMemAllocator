#ifndef _my_allocator_hpp_
#define _my_allocator_hpp_

#include <cstdlib>
#include "free_list.hpp"
#include <vector>

typedef void* Addr;

class MyAllocator {
private:
	Addr initP;		//adress used as argument for malloc()
	size_t basicBlockSize;
	std::vector<FreeList*> fls;

	SegmentHeader* splitSegment(SegmentHeader* _toSplit, int _reqBlocks);
	SegmentHeader* combineSegment(SegmentHeader* _toComine);

	int getFibNum(int f);	//returns smalles fibo number greater than f
	int lengthToFlIndex(int f);	//input fibo number, tells you which fibo number it is 1|1, 2|2, 3|3, 4|5, 5|8,...
	int flIndexToLength(int n);	//input which fibo number you want, returns fibo (invers of fiboToN())
public:
	MyAllocator(size_t _basic_block_size, size_t _size);
	~MyAllocator();

	Addr Malloc(size_t _length);
	bool Free(Addr _a);

	//FOR DEBUG, REMOVE BEFORE SUBMIT
	void dumpFls();
};

#endif