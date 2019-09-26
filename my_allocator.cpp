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
	for (int i = 0; ((tempFibo + 1) * basicBlockSize) < _size + sizeof(SegmentHeader); i++) {	//Create the vector of freeLists to the correct size based on total memory requested
		tempFibo = nToFibo(i + 1);
		FreeList* temp = new FreeList();
		fls.push_back(temp);
	}
	SegmentHeader* newSeg = new(std::malloc(nToFibo(fls.size()) * basicBlockSize)) SegmentHeader(nToFibo(fls.size()) * basicBlockSize);
	initP = (char*)newSeg;		//Sets start point of program memory. Used for destructor
	initSize = newSeg->length;
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
	if (curSeg == nullptr) return nullptr;
	else {
		return (char*)curSeg + sizeof(SegmentHeader);
	}
}

bool MyAllocator::Free(Addr _a) {
	SegmentHeader* fSeg = (SegmentHeader*)((char*)_a - sizeof(SegmentHeader));		//Creates fSeg which is at the same address as the segment that should be removed
	if (fSeg->CheckValid()) {
		fls.at(fiboToN(fSeg->length / basicBlockSize) - 1)->Add(fSeg);
		combineSegs(fSeg);
		return true;
	}
	else
		return false;
}

SegmentHeader* MyAllocator::splitSeg(SegmentHeader* curSeg, int requestedBlocks) {		//Takes the segment to be split and the number of blocks requested as input
	if (curSeg->length / basicBlockSize == 2 && requestedBlocks == 1) {		//Special case because I was having issues
		curSeg->length = requestedBlocks * basicBlockSize;
		SegmentHeader* newSegment = new ((char*)((char*)curSeg + curSeg->length)) SegmentHeader(requestedBlocks * basicBlockSize);
		newSegment->lr = 'r';
		newSegment->inherit = curSeg->inherit;
		curSeg->inherit = curSeg->lr;
		curSeg->lr = 'l';
		fls.at(0)->Add(newSegment);
		return curSeg;
	}
	curSeg->length = nToFibo(fiboToN(curSeg->length / basicBlockSize) - 2) * basicBlockSize;		//Makes curSeg the smaller fibo number (ie 3 in 8=5+3)
	SegmentHeader* newSegment = new ((char*)((char*)curSeg + curSeg->length)) SegmentHeader(nToFibo(fiboToN(curSeg->length / basicBlockSize) + 1) * basicBlockSize);	//Makes newSeg the larger fibo number (ie 5 in 8=5+3)
	newSegment->lr = 'r';
	newSegment->inherit = curSeg->inherit;
	curSeg->inherit = curSeg->lr;
	curSeg->lr = 'l';
	if (fiboToN(curSeg->length / basicBlockSize) > fiboToN(getFibNum(requestedBlocks)) + 1) {	//if curSeg is 2+ fibo numbers larger than needed fibo num (ie 8 needed, curSeg is 21)
		fls.at(fiboToN(newSegment->length / basicBlockSize) - 1)->Add(newSegment);
		return curSeg;
	}
	else if (fiboToN(curSeg->length / basicBlockSize) == fiboToN(getFibNum(requestedBlocks))) {
			fls.at(fiboToN(newSegment->length / basicBlockSize) - 1)->Add(newSegment);
			return curSeg;
	}
	else {
		fls.at(fiboToN(curSeg->length / basicBlockSize) - 1)->Add(curSeg);
		return newSegment;
	}
}

bool MyAllocator::combineSegs(SegmentHeader* fSeg) {
	if (fSeg->length >= initSize)
		return false;
	else if (fSeg->lr == 'r' && fSeg > initP) {
		SegmentHeader* buddySeg = (SegmentHeader*)((char*)fSeg - nToFibo(fiboToN(fSeg->length / basicBlockSize) - 1) * basicBlockSize);
		if (buddySeg->CheckValid() && buddySeg->is_free && ((fiboToN(buddySeg->length / basicBlockSize) == fiboToN(fSeg->length / basicBlockSize) - 1) || fSeg->length == basicBlockSize)) {
			if (!fls.at(fiboToN(buddySeg->length / basicBlockSize) - 1)->Remove(buddySeg))
				cout << "Error line 146" << endl;
			buddySeg->length += fSeg->length;
			buddySeg->lr = buddySeg->inherit;
			buddySeg->inherit = fSeg->inherit;
			if (!fls.at(fiboToN(fSeg->length / basicBlockSize) - 1)->Remove(fSeg))
				cout << "Error line 151" << endl;
			fls.at(fiboToN(buddySeg->length / basicBlockSize) - 1)->Add(buddySeg);
			combineSegs(buddySeg);
		}
	}
	else if (fSeg->lr == 'l' && (char*)fSeg + fSeg->length < (char*)initP + initSize) {
		SegmentHeader* buddySeg = (SegmentHeader*)((char*)fSeg + fSeg->length);
		if (buddySeg->CheckValid() && buddySeg->is_free && (fiboToN(buddySeg->length / basicBlockSize) == fiboToN(fSeg->length / basicBlockSize) + 1 || buddySeg->length == basicBlockSize)) {
			if (!fls.at(fiboToN(fSeg->length / basicBlockSize) - 1)->Remove(fSeg))
				cout << "Error line 162" << endl;
			fSeg->length += buddySeg->length;
			fSeg->lr = fSeg->inherit;
			fSeg->inherit = buddySeg->inherit;
			if (!fls.at(fiboToN(buddySeg->length / basicBlockSize) - 1)->Remove(buddySeg))
				cout << "Error line 166" << endl;
			fls.at(fiboToN(fSeg->length / basicBlockSize) - 1)->Add(fSeg);
			combineSegs(fSeg);
		}
	}
	return true;
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
		if (i > 50) {
			cout << "Error occurred with mem dump" << endl;
			break;
		}
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

int MyAllocator::getFibNum(int f) {		//returns smalles fibo number greater than f
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

int MyAllocator::fiboToN(int f) {		//input fibo number, tells you which fibo number it is 1->1, 2->2, 3->3, 5->4, 8->5,...
	int a = 1;
	int b = 1;
	int temp;
	int i = 1;
	for (i; a != f; i++) {
		temp = a;
		a += b;
		b = temp;
		if (a > 1000000) {		//USED FOR DEBUG, REMOVE BEFORE SUBMIT
			cout << "Error occured at fiboToN" << endl;
			return -1;
		}
	}
	return i;
}

int MyAllocator::nToFibo(int n) {		//input which fibo number you want, returns fibo (inverse of fiboToN())
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