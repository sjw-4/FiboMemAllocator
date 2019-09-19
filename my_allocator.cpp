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
	basicBlockSize = _basic_block_size;
	int tempFibo = 1;
	for (int i = 0; ((tempFibo + 1) * basicBlockSize) < _size + 20; i++) {
		tempFibo = nToFibo(i + 1);
		FreeList* temp = new FreeList();
		fls.push_back(temp);
	}
	//void* tempVal = std::malloc(nToFibo(fls.size()) * basicBlockSize /*+ sizeof(SegmentHeader)*/);		//REMOVE VAR
	SegmentHeader* newSeg = new(std::malloc(nToFibo(fls.size()) * basicBlockSize)) SegmentHeader(nToFibo(fls.size()) * basicBlockSize);			//REMOVE TEMPVAL
	FreeList* temp = new FreeList(newSeg);
	fls.back() = temp;
}

MyAllocator::~MyAllocator() {
	std::free(initP);
}

Addr MyAllocator::Malloc(size_t _length) {
	int requestedBlocks = _length / basicBlockSize;
	if (_length % basicBlockSize != 0) requestedBlocks++;
	SegmentHeader* curSeg = nullptr;
	for (int i = fiboToN(getFibNum(requestedBlocks)) - 1; i < fls.size() && curSeg == nullptr; i++) {
		if (fls.at(i)->head != nullptr) {
			curSeg = fls.at(i)->head;
			fls.at(i)->Remove(curSeg);
			while (curSeg->length / basicBlockSize > getFibNum(requestedBlocks)) {
				curSeg = splitSeg(curSeg, requestedBlocks);
			}
		}
	}
	if (curSeg == nullptr) return nullptr;
	else return (char*)curSeg + sizeof(SegmentHeader);
}

bool MyAllocator::Free(Addr _a) {
	SegmentHeader* fSeg = (SegmentHeader*)((char*)_a - sizeof(SegmentHeader));
	if (fSeg->CheckValid()) {
		fls.at(fiboToN(fSeg->length / basicBlockSize) - 1)->Add(fSeg);
		fSeg->is_free = true;
		combineSegs();
		return true;
	}
	else
		return false;
}

SegmentHeader* MyAllocator::splitSeg(SegmentHeader* curSeg, int requestedBlocks) {
	if (fiboToN(curSeg->length / basicBlockSize) > fiboToN(getFibNum(requestedBlocks)) + 1) {	//if the selected block size is 2 or more fibo numbers larger than the current block size, it will make current block size smaller than the new block size
		curSeg->length = nToFibo(fiboToN(curSeg->length / basicBlockSize) - 2) * basicBlockSize;
		SegmentHeader* newSegment = new ((char*)((char*)curSeg + curSeg->length)) SegmentHeader(nToFibo(fiboToN(curSeg->length / basicBlockSize) + 1) * basicBlockSize);
		fls.at(fiboToN(newSegment->length / basicBlockSize) - 1)->Add(newSegment);
	}
	else {
		curSeg->length = nToFibo(fiboToN(curSeg->length / basicBlockSize) - 1) * basicBlockSize;
		SegmentHeader* newSegment = new ((char*)((char*)curSeg + curSeg->length)) SegmentHeader(nToFibo(fiboToN(curSeg->length / basicBlockSize) - 1) * basicBlockSize);
		fls.at(fiboToN(newSegment->length / basicBlockSize) - 1)->Add(newSegment);
	}
	return curSeg;
}

bool MyAllocator::combineSegs() {
	for (int i = 0; i < fls.size(); i++) {

	}
}

void MyAllocator::dumpFls() {
	SegmentHeader* temp;
	cout << "Memory Dump:" << endl << "--------------------------------" << endl;
	size_t totMem = 0;
	for (int i = 0; i < fls.size(); i++) {
		cout << "Free list " << i << ": ";
		temp = fls.at(i)->head;
		while (temp != nullptr) {
			cout << temp->length << ", ";
			totMem += temp->length;
			temp = temp->next;
		}
		cout << endl;
	}
	cout << "Total free mem: " << totMem << endl << "--------------------------------" << endl;
}

int MyAllocator::getFibNum(int f) {		//Two functions in one because it makes function calls less confusing for me
	/*
		if n = num & f = 0: n|return val-> 1|1, 2|2, 3|3, 4|5, 5|8, 6|13, 7|21, ...     returns the corresponding fibo number
		else if n = 0 & f = num: returns the smallest fibo number that is larger than f
		else it returns -1 (error)
	*/
	int a = 1;
	int b = 1;
	int temp;
	while (a < f) {
		temp = a;
		a += b;
		b = temp;
	}
	return a;

}

int MyAllocator::fiboToN(int f) {
	int a = 1;
	int b = 1;
	int temp;
	int i = 1;
	for (i; a != f; i++) {
		temp = a;
		a += b;
		b = temp;
	}
	return i;
}

int MyAllocator::nToFibo(int n) {
	int a = 1;
	int b = 1;
	int temp;
	for (int i = 1; i < n; i++) {
		temp = a;
		a += b;
		b = temp;
	}
	return a;
}