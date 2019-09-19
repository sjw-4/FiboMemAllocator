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
	//add check to make sure input makes sense
	basicBlockSize = _basic_block_size;
	int tempFibo = 1;
	for (int i = 0; ((tempFibo + 1) * basicBlockSize) < _size + 20; i++) {	//Create the vector of freeLists to the correct size based on total memory requested
		tempFibo = nToFibo(i + 1);
		FreeList* temp = new FreeList();
		fls.push_back(temp);
	}
	SegmentHeader* newSeg = new(std::malloc(nToFibo(fls.size()) * basicBlockSize)) SegmentHeader(nToFibo(fls.size()) * basicBlockSize);
	initP = (char*)newSeg;		//Sets start point of program memory. Used for destructor
	FreeList* temp = new FreeList(newSeg);		//Creates free list with one SH that contains total memory
	fls.back() = temp;		//Replaces the last FL in the vector of FL (which is null) with a FL with the full memory SH
}

MyAllocator::~MyAllocator() {
	std::free(initP);
}

Addr MyAllocator::Malloc(size_t _length) {
	int requestedBlocks = (_length + sizeof(SegmentHeader)) / basicBlockSize;
	if (_length % basicBlockSize != 0) requestedBlocks++;	//This line and previous figure out how many block sizes the user requested
	SegmentHeader* curSeg = nullptr;
	for (int i = fiboToN(getFibNum(requestedBlocks)) - 1; i < fls.size() && curSeg == nullptr; i++) {	//Starts traversing list at the smallest fibo number FL that is big enough for requested memory
		if (fls.at(i)->head != nullptr) {		//Checks for a SH in the free list
			curSeg = fls.at(i)->head;
			fls.at(i)->Remove(curSeg);		//Removes the head from the FreeList
			while (curSeg->length / basicBlockSize > getFibNum(requestedBlocks)) {		//Reduces size of segment to be appropriat for the requested amount of memory. Iterates as necessary
				curSeg = splitSeg(curSeg, requestedBlocks);		//Splits the memory block into smaller fibo chunks
			}
		}
	}
	//dumpFls();
	if (curSeg == nullptr) return nullptr;
	else {
		curSeg->is_free = false;
		return (char*)curSeg + sizeof(SegmentHeader);
		//dumpFls();
	}
}

bool MyAllocator::Free(Addr _a) {
	SegmentHeader* fSeg = (SegmentHeader*)((char*)_a - sizeof(SegmentHeader));		//Creates fSeg which is at the same address as the segment that should be removed
	if (fSeg->CheckValid()) {
		fls.at(fiboToN(fSeg->length / basicBlockSize) - 1)->Add(fSeg);		//Adjusts memory location to start of header
		fSeg->is_free = true;
		combineSegs();
		return true;
	}
	else
		return false;
}

SegmentHeader* MyAllocator::splitSeg(SegmentHeader* curSeg, int requestedBlocks) {		//Takes the segment to be split and the number of blocks requested as input
	//To keep the segments in fls the largest size possible, it checks to see if the segment to be split should be made into the smaller
	//or larger fibo number (ie segment is 8, should 5 or 3 be added to the free list or given to the user)

	//Both functions follow the same steps - 1) reduce size of current segment, 2) create new segment of the proper size and at the proper mem address, 3) add newseg to proper freeList

	if (fiboToN(curSeg->length / basicBlockSize) > fiboToN(getFibNum(requestedBlocks)) + 1) {	//if the selected block size is 2 or more fibo numbers larger than the current block size, it will make current block size smaller than the new block size
		curSeg->length = nToFibo(fiboToN(curSeg->length / basicBlockSize) - 2) * basicBlockSize;
		SegmentHeader* newSegment = new ((char*)((char*)curSeg + curSeg->length)) SegmentHeader(nToFibo(fiboToN(curSeg->length / basicBlockSize) + 1) * basicBlockSize);
		fls.at(fiboToN(newSegment->length / basicBlockSize) - 1)->Add(newSegment);
	}
	else {	//if the selected block is only one fibo number (ie seg is 8, should be 5) above the requested mem size
		curSeg->length = nToFibo(fiboToN(curSeg->length / basicBlockSize) - 1) * basicBlockSize;
		SegmentHeader* newSegment = new ((char*)((char*)curSeg + curSeg->length)) SegmentHeader(nToFibo(fiboToN(curSeg->length / basicBlockSize) - 1) * basicBlockSize);
		fls.at(fiboToN(newSegment->length / basicBlockSize) - 1)->Add(newSegment);
	}
	return curSeg;		//Returns the adjusted curseg that will be given to the user
}

bool MyAllocator::combineSegs() {
	for (int i = 0; i < fls.size(); i++) {

	}
}

void MyAllocator::dumpFls() {		//Used for debugging purposes, should be removed before submit
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

void MyAllocator::checkFls() {
	for(int i = 0; i < fls.size(); i++){
		SegmentHeader* temp = fls.at(0)->head;
		while(temp != nullptr && temp->next != nullptr){
			if(temp->length != temp->next->length){
				cout << "ERROR IN FLS" << endl;
				dumpFls();
			}
			temp = temp->next;
		}
	}
}

int MyAllocator::getFibNum(int f) {		//see hpp file for details
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

int MyAllocator::fiboToN(int f) {		//see hpp file for details
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

int MyAllocator::nToFibo(int n) {		//see hpp file for details
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