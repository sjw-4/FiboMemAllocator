/* 
    File: my_allocator.cpp

    Author: Samuel Wilkins IV
            Department of Computer Science
            Texas A&M University
    Date  : 9/16/19

    Modified: 

    This file contains the implementation of the class MyAllocator.

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>
#include "my_allocator.hpp"
#include <assert.h>
#include <iostream>

/*--------------------------------------------------------------------------*/
/* NAME SPACES */ 
/*--------------------------------------------------------------------------*/

using namespace std;
/* I know, it's a bad habit, but this is a tiny program anyway... */

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS MyAllocator */
/*--------------------------------------------------------------------------*/

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) {
	fl = new FreeList(std::malloc(_size), _size);
	basicBlockSize = _basic_block_size;
}

MyAllocator::~MyAllocator() {
	std::free(initP);
}

Addr MyAllocator::Malloc(size_t _length) {
	int requestedBlocks = _length / basicBlockSize;
	if(_length % basicBlockSize != 0) requestedBlocks++;
	SegmentHeader* curSeg = fl->head;
	while (curSeg != nullptr && curSeg->length < (sizeof(SegmentHeader) + (requestedBlocks * basicBlockSize))) {
		curSeg = curSeg->next;
	}
	if (curSeg == nullptr) {
		return nullptr;
	}
	fl->Remove(curSeg);
	if(curSeg->length > requestedBlocks * basicBlockSize + 2 * sizeof(SegmentHeader)) {	//Tests if there's enough mem to split segment, if not it returns an unmodified current segment
		SegmentHeader* newSeg = new ((char*)curSeg + sizeof(SegmentHeader) + (requestedBlocks * basicBlockSize)) SegmentHeader(curSeg->length - sizeof(SegmentHeader) - (requestedBlocks * basicBlockSize));
		fl->Add(newSeg);
		curSeg->is_free = false;
		curSeg->length = sizeof(SegmentHeader) + (requestedBlocks * basicBlockSize);
	}
	return (char*)curSeg + sizeof(SegmentHeader);
}

bool MyAllocator::Free(Addr _a) {
	SegmentHeader* fSeg = (SegmentHeader*)((char*)_a - sizeof(SegmentHeader));
	if (fSeg->CheckValid()) {
		fl->Add(fSeg);
		fSeg->is_free = true;
		return true;
	}
	else
		return false;
}

size_t MyAllocator::memAtAddr(Addr _a) {
	SegmentHeader* fSeg = (SegmentHeader*)((char*)_a - sizeof(SegmentHeader));
	if (fSeg->CheckValid()) return fSeg->length + 20;
	else return 0;
}

