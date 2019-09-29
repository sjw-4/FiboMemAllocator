/*
	File: my_allocator.hpp

	Author: Samuel Wilkins IV
			Department of Computer Science
			Texas A&M University
	Date  : 9/28/19

	Modified: way too many times

*/
#ifndef _my_allocator_hpp_
#define _my_allocator_hpp_

#include <cstdlib>
#include "free_list.hpp"
#include <vector>

typedef void* Addr;

class MyAllocator {
private:
	Addr initP;					//adress used as argument for malloc()
	size_t basicBlockSize;		//basic block size defined when constructing
	std::vector<FreeList*> fls;	//holds all of the free lists of different fibo sizes

	SegmentHeader* splitSegment(SegmentHeader* _toSplit, int _reqBlocks);	//Takes the SH to be split and the number of required blocks as input, splits the segment and returns the segment to best fit required memory (may occassionally return SH of size 2*bbs when only 1*bbs is requested)
	SegmentHeader* combineSegment(SegmentHeader* _toComine);				//Takes a SH as argument and attempts to combine it with other segments in fls, if it does combine with another segment it will repeat the combineSegment until it no longer can

	int getFibNum(int f);	//returns smalles fibo number greater than f
	int lengthToFlIndex(int f);	//input fibo number, tells you which fibo number it is 1|1, 2|2, 3|3, 4|5, 5|8,...
	int flIndexToLength(int n);	//input which fibo number you want, returns fibo (invers of fiboToN())
public:
	MyAllocator(size_t _basic_block_size, size_t _size);	//Constructor
	~MyAllocator();											//Destructor

	Addr Malloc(size_t _length);		//Allocates enough memory for requested length and SH, returns address that user can put data in. Will round up to nearest fibo number
	bool Free(Addr _a);					//Frees memory at the specified address (_a), returns if it was successful or not (will fail if invalid address, etc)
};

#endif