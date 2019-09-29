#include <cstdlib>
#include "my_allocator.hpp"
#include <assert.h>
#include <iostream>

using namespace std;
/*
Addr initP;
size_t basicBlockSize;
vector<FreeList*> fls;
*/

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) {
	//cout << sizeof(SegmentHeader) << endl;
	basicBlockSize = _basic_block_size;
	int requestedBlocks = _size / basicBlockSize;
	if (_size % basicBlockSize != 0) requestedBlocks++;
	for (int i = 0; i < lengthToFlIndex(getFibNum(requestedBlocks)) + 1; i++) {
		fls.push_back(new FreeList(flIndexToLength(i)));
	}
	initP = std::malloc(fls.back()->getSize());
	SegmentHeader* newSeg = new(initP) SegmentHeader(fls.back()->getSize());
	fls.back()->Add(newSeg);
}
MyAllocator::~MyAllocator() {
	std::free(initP);
}

SegmentHeader* MyAllocator::splitSegment(SegmentHeader* _toSplit, int _reqBlocks) {
	if (_toSplit->getLength() == basicBlockSize * 2) {
		cout << "Error in splitSeg" << endl;
		return nullptr;
	}
	_toSplit->setLength(flIndexToLength(lengthToFlIndex(_toSplit->getLength() / basicBlockSize) - 2));
	SegmentHeader* rightSeg = new((char*)_toSplit + _toSplit->getLength()) SegmentHeader(flIndexToLength(lengthToFlIndex(_toSplit->getLength() / basicBlockSize) + 1));
	rightSeg->setInherit(_toSplit->getInherit()); rightSeg->setLR('r'); rightSeg->setIsFree(false);
	_toSplit->setInherit(_toSplit->getLR());   _toSplit->setLR('l');
	if (_reqBlocks == 1 && _toSplit->getLength() == 2 * basicBlockSize) {
		fls.at(lengthToFlIndex(_toSplit->getLength() / basicBlockSize))->Add(_toSplit);
		return splitSegment(rightSeg, _reqBlocks);
	}
	else if (_reqBlocks > _toSplit->getLength() / basicBlockSize) {
		fls.at(lengthToFlIndex(_toSplit->getLength() / basicBlockSize))->Add(_toSplit);
		return rightSeg;
	}
	else {
		fls.at(lengthToFlIndex(rightSeg->getLength() / basicBlockSize))->Add(rightSeg);
		return _toSplit;
	}
}
SegmentHeader* MyAllocator::combineSegment(SegmentHeader* _toCombine) {
	if (_toCombine->getLength() >= fls.back()->getSize()) return nullptr;
	if (_toCombine->getLR() == 'l') {
		SegmentHeader* rightBuddy = (SegmentHeader*)((char*)_toCombine + _toCombine->getLength());
		if (!rightBuddy->CheckValid() || !rightBuddy->getIsFree() || rightBuddy->getLR() != 'r' || lengthToFlIndex(rightBuddy->getLength() / basicBlockSize) != lengthToFlIndex(_toCombine->getLength() / basicBlockSize) + 1) return nullptr;
		fls.at(lengthToFlIndex(rightBuddy->getLength() / basicBlockSize))->Remove(rightBuddy);
		fls.at(lengthToFlIndex(_toCombine->getLength() / basicBlockSize))->Remove(_toCombine);
		_toCombine->setLength(_toCombine->getLength() + rightBuddy->getLength());
		_toCombine->setLR(_toCombine->getInherit());
		_toCombine->setInherit(rightBuddy->getInherit());
		rightBuddy->~SegmentHeader();
		fls.at(lengthToFlIndex(_toCombine->getLength() / basicBlockSize))->Add(_toCombine);
		combineSegment(_toCombine);
	}
	else {
		size_t expectedLBSize = flIndexToLength(lengthToFlIndex(_toCombine->getLength() / basicBlockSize) - 1);
		SegmentHeader* leftBuddy = (SegmentHeader*)((char*)_toCombine - expectedLBSize);
		if (!leftBuddy->CheckValid() || !leftBuddy->getIsFree() || leftBuddy->getLR() != 'l' || lengthToFlIndex(leftBuddy->getLength() / basicBlockSize) != lengthToFlIndex(_toCombine->getLength() / basicBlockSize) - 1) return nullptr;
		fls.at(lengthToFlIndex(leftBuddy->getLength() / basicBlockSize))->Remove(leftBuddy);
		fls.at(lengthToFlIndex(_toCombine->getLength() / basicBlockSize))->Remove(_toCombine);
		leftBuddy->setLength(_toCombine->getLength() + leftBuddy->getLength());
		leftBuddy->setLR(leftBuddy->getInherit());
		leftBuddy->setInherit(_toCombine->getInherit());
		_toCombine->~SegmentHeader();
		fls.at(lengthToFlIndex(leftBuddy->getLength() / basicBlockSize))->Add(leftBuddy);
		combineSegment(leftBuddy);
	}
	return nullptr;
}

Addr MyAllocator::Malloc(size_t _length) {
	if (_length > fls.back()->getSize())
		return nullptr;
	int requestedBlocks = (_length + sizeof(SegmentHeader)) / basicBlockSize;
	if ((_length + sizeof(SegmentHeader)) % basicBlockSize != 0) requestedBlocks++;
	for (int i = lengthToFlIndex(getFibNum(requestedBlocks)); i < fls.size(); i++) {
		if (fls.at(i)->getHead() != nullptr && fls.at(i)->getSize() / basicBlockSize == 2 && requestedBlocks == 1) {	//gives user 2* basic block size if they requested 1 but no segment of size 1 * bbs is available
			SegmentHeader* tempSeg = fls.at(i)->getHead();
			fls.at(i)->Remove(tempSeg);
			return (char*)tempSeg + sizeof(SegmentHeader);
		}
		if (fls.at(i)->getHead() != nullptr ) {
			SegmentHeader* tempSeg = fls.at(i)->getHead();
			fls.at(i)->Remove(tempSeg);
			while (tempSeg->getLength() > getFibNum(requestedBlocks) * basicBlockSize) {
				tempSeg = splitSegment(tempSeg, requestedBlocks);
			}
			return (char*)tempSeg + sizeof(SegmentHeader);
		}
	}
	return nullptr;
}
bool MyAllocator::Free(Addr _a) {
	SegmentHeader* freeSeg = (SegmentHeader*)((char*)_a - sizeof(SegmentHeader));
	if (!freeSeg->CheckValid()) return false;
	fls.at(lengthToFlIndex(freeSeg->getLength() / basicBlockSize))->Add(freeSeg);
	combineSegment(freeSeg);
	return true;
}

/*Reference for fib numbers
n: 0|1|2|3|4| 5| 6| 7| 8| 9|
f: 1|2|3|5|8|13|21|34|55|89
*/

int MyAllocator::getFibNum(int f) {		//returns smalles fibo number greater than or equal to f
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
int MyAllocator::flIndexToLength(int n) {		//input index of fl, returns what length SH goes in that list
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



//FOR DEBUG, REMOVE BEFORE SUBMIT
void MyAllocator::dumpFls() {
	SegmentHeader* temp;
	cout << "Memory Dump:" << endl << "--------------------------------" << endl;
	size_t totMem = 0;
	for (int i = 0; i < fls.size(); i++) {
		cout << "Free list " << i << ": ";
		temp = fls.at(i)->getHead();
		while (temp != nullptr) {
			cout << temp->getLength() << ", ";
			totMem += temp->getLength();
			temp = temp->getNext();
		}
		cout << endl;
		if (i > 50) {
			cout << "Error occurred with mem dump" << endl;
			break;
		}
	}
	cout << "Total free mem: " << totMem << endl << "--------------------------------" << endl;
}