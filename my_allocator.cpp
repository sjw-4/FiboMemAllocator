/*
	File: my_allocator.cpp

	Author: Samuel Wilkins IV
			Department of Computer Science
			Texas A&M University
	Date  : 9/28/19

	Modified: yes

	This file contains the implementation of the class MyAllocator.

*/

#include <cstdlib>
#include "my_allocator.hpp"
#include <assert.h>
#include <iostream>

using namespace std;

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) {
	basicBlockSize = _basic_block_size;
	int requestedBlocks = _size / basicBlockSize;									//This line and the following line calculate the number of segments needed for the requested size
	if (_size % basicBlockSize != 0) requestedBlocks++;
	for (int i = 0; i < lengthToFlIndex(getFibNum(requestedBlocks)) + 1; i++) {		//Creates freeLists of each fibo*bbs size and adds them to the freeList vector (fls)
		fls.push_back(new FreeList(flIndexToLength(i)));
	}
	initP = std::malloc(fls.back()->getSize());										//Assigns address for initP, used for destructor
	SegmentHeader* newSeg = new(initP) SegmentHeader(fls.back()->getSize());		//Creates the initial SH that contains all allocated memory
	fls.back()->Add(newSeg);														//Adds initial SH to the FL at the back of fls (which holds SH of that size)
}
MyAllocator::~MyAllocator() {
	std::free(initP);																//Frees all memory
}

SegmentHeader* MyAllocator::splitSegment(SegmentHeader* _toSplit, int _reqBlocks) {		//Splits the sent segment into two smaller segments, makes decisions on how to split based on requested blocks
	if (_toSplit->getLength() == basicBlockSize * 2) {									//splitSegment is designed to not split segments of 2*BBS, so if this happes it outputs an error (should never happen... fingers crossed)
		cout << "Error in splitSeg" << endl;
		return nullptr;
	}
	_toSplit->setLength(flIndexToLength(lengthToFlIndex(_toSplit->getLength() / basicBlockSize) - 2));																		//Sets the _toSplit segment size to 2 less fibo numbers (ie _toSplit==8, 8=5+3, _toSplit=3)
	SegmentHeader* rightSeg = new((char*)_toSplit + _toSplit->getLength()) SegmentHeader(flIndexToLength(lengthToFlIndex(_toSplit->getLength() / basicBlockSize) + 1));		//Creates the right segment of the split, sets address properly, sets size to 5 in the above example (8=5+3)
	rightSeg->setInherit(_toSplit->getInherit()); rightSeg->setLR('r'); rightSeg->setIsFree(false);		//Sets the proper values for right seg; inherit, LR, and is_free
	_toSplit->setInherit(_toSplit->getLR());   _toSplit->setLR('l');									//Sets the proper values for left seg (_to split); inherit, and LR
	if (_reqBlocks == 1 && _toSplit->getLength() == 2 * basicBlockSize) {								//Special case for requested block size of 1, and the length of _toSplit is 2 (tells us the split that just occured was 5=3+2)
		fls.at(lengthToFlIndex(_toSplit->getLength() / basicBlockSize))->Add(_toSplit);					//Adds the left seg (_toSplig) to the free list (since its length is 2*bbs, and we don't split 2*bbs in this part of town)
		return splitSegment(rightSeg, _reqBlocks);														//Splits the rightSeg and returns what that split returns (right seg would be 3, so it will return that split 3=2+1, which will return 1 since bbs is 1 in this if statement)
	}
	else if (_reqBlocks > _toSplit->getLength() / basicBlockSize) {										//If the requested blocks is larger than left seg (_toSplit) it will return righSeg to the user and add _toSplit to the free list
		fls.at(lengthToFlIndex(_toSplit->getLength() / basicBlockSize))->Add(_toSplit);
		return rightSeg;
	}
	else {																								//The requested blocks must be less than both new segments, so it returns the left seg (_toSplit, which is smaller, and will be split again in the while loop in free), and adds rightSeg to the freeList
		fls.at(lengthToFlIndex(rightSeg->getLength() / basicBlockSize))->Add(rightSeg);
		return _toSplit;
	}
}
SegmentHeader* MyAllocator::combineSegment(SegmentHeader* _toCombine) {									//Combines segment sent with it's buddy segment if the buddy is in the freeList
	if (_toCombine->getLength() >= fls.back()->getSize()) return nullptr;
	if (_toCombine->getLR() == 'l') {
		SegmentHeader* rightBuddy = (SegmentHeader*)((char*)_toCombine + _toCombine->getLength());
		if (!rightBuddy->CheckValid() || !rightBuddy->getIsFree() || rightBuddy->getLR() != 'r' || lengthToFlIndex(rightBuddy->getLength() / basicBlockSize) != lengthToFlIndex(_toCombine->getLength() / basicBlockSize) + 1) return nullptr;	//Checks to see if the _toCombine segment can be combined with it's buddy
		fls.at(lengthToFlIndex(rightBuddy->getLength() / basicBlockSize))->Remove(rightBuddy);		//This line and next remove the two segments to be combined from the freeList
		fls.at(lengthToFlIndex(_toCombine->getLength() / basicBlockSize))->Remove(_toCombine);
		_toCombine->setLength(_toCombine->getLength() + rightBuddy->getLength());					//This line and the next 2 set the appropriate values in the combined segment header (LR and inherit values)
		_toCombine->setLR(_toCombine->getInherit());
		_toCombine->setInherit(rightBuddy->getInherit());
		rightBuddy->~SegmentHeader();																//Resets values in SH, helps to avoid future issues of combining segments
		fls.at(lengthToFlIndex(_toCombine->getLength() / basicBlockSize))->Add(_toCombine);			//Adds newly combined segment to the freeList
		combineSegment(_toCombine);																	//Recursively calls combineSegment on the newly created segment to see if it can be combined further
	}
	else {
		size_t expectedLBSize = flIndexToLength(lengthToFlIndex(_toCombine->getLength() / basicBlockSize) - 1);
		SegmentHeader* leftBuddy = (SegmentHeader*)((char*)_toCombine - expectedLBSize);
		if (!leftBuddy->CheckValid() || !leftBuddy->getIsFree() || leftBuddy->getLR() != 'l' || lengthToFlIndex(leftBuddy->getLength() / basicBlockSize) != lengthToFlIndex(_toCombine->getLength() / basicBlockSize) - 1) return nullptr;	//Checks to see if the _toCombine segment can be combined with it's buddy
		fls.at(lengthToFlIndex(leftBuddy->getLength() / basicBlockSize))->Remove(leftBuddy);		//This line and next remove the two segments to be combined from the freeList
		fls.at(lengthToFlIndex(_toCombine->getLength() / basicBlockSize))->Remove(_toCombine);
		leftBuddy->setLength(_toCombine->getLength() + leftBuddy->getLength());						//This line and the next 2 set the appropriate values in the combined segment header (LR and inherit values)
		leftBuddy->setLR(leftBuddy->getInherit());
		leftBuddy->setInherit(_toCombine->getInherit());
		_toCombine->~SegmentHeader();																//Resets values in SH, helps to avoid future issues of combining segments
		fls.at(lengthToFlIndex(leftBuddy->getLength() / basicBlockSize))->Add(leftBuddy);			//Adds newly combined segment to the freeList
		combineSegment(leftBuddy);																	//Recursively calls combineSegment on the newly created segment to see if it can be combined further
	}
	return nullptr;
}

Addr MyAllocator::Malloc(size_t _length) {
	if (_length > fls.back()->getSize())													//checks to see if the requested memory is even possible with the total memory size set
		return nullptr;
	int requestedBlocks = (_length + sizeof(SegmentHeader)) / basicBlockSize;				//This line and the next calculate the necessary number of blocks needed for the memory allocation
	if ((_length + sizeof(SegmentHeader)) % basicBlockSize != 0) requestedBlocks++;
	for (int i = lengthToFlIndex(getFibNum(requestedBlocks)); i < fls.size(); i++) {		//Starts at the smalles FreeList that can hold the requested size, then iterates through fls to find the smalles available segment that can hold the requested size
		if (fls.at(i)->getHead() != nullptr && fls.at(i)->getSize() / basicBlockSize == 2 && requestedBlocks == 1) {	//Sees if the freelist at that index is empty or not, and if so gives user 2* basic block size if they requested 1 but no segment of size 1 * bbs is available
			SegmentHeader* tempSeg = fls.at(i)->getHead();
			fls.at(i)->Remove(tempSeg);
			return (char*)tempSeg + sizeof(SegmentHeader);
		}
		if (fls.at(i)->getHead() != nullptr ) {												//Sees if the freelist at that index is empty or not, if it's not it grabs the first segment header in the list and recursively splits it (if necessary) into the correct size segment
			SegmentHeader* tempSeg = fls.at(i)->getHead();
			fls.at(i)->Remove(tempSeg);
			while (tempSeg->getLength() > getFibNum(requestedBlocks) * basicBlockSize) {
				tempSeg = splitSegment(tempSeg, requestedBlocks);
			}
			return (char*)tempSeg + sizeof(SegmentHeader);
		}
	}
	return nullptr;																			//If no segment is found it returns nullptr (memory full)
}
bool MyAllocator::Free(Addr _a) {
	SegmentHeader* freeSeg = (SegmentHeader*)((char*)_a - sizeof(SegmentHeader));			//gets the segment at the requested address
	if (!freeSeg->CheckValid()) return false;												//returns false if the segment is invalid (incorrect address sent)
	fls.at(lengthToFlIndex(freeSeg->getLength() / basicBlockSize))->Add(freeSeg);			//adds the segment to the freelist
	combineSegment(freeSeg);																//Tries to combine the segment with its buddy, if successful it does it recursively until unable to
	return true;
}

//The following are helper functions to get help with fibonacci stuff

int MyAllocator::getFibNum(int f) {				//returns smalles fibo number greater than or equal to f
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
int MyAllocator::lengthToFlIndex(int f) {		//input length of segment in bbs, returns which fl index it should be put into
	int a = 1;
	int b = 1;
	int temp;
	int i = 0;
	for (i; a != f; i++) {
		temp = a;
		a += b;
		b = temp;
		if (a > f)
			return -1;
	}
	return i;
}
int MyAllocator::flIndexToLength(int n) {		//input index of fl, returns what length the segments are at that index
	int a = 1;
	int b = 1;
	int temp;
	for (int i = 0; i < n; i++) {
		temp = a;
		a += b;
		b = temp;
	}
	return a * basicBlockSize;
}