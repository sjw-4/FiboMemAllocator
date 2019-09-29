/*
	File: free_list.hpp

	Author: Samuel Wilkins IV
			Department of Computer Science
			Texas A&M University
	Date  : 9/28/19

	Modified: quite a bit, actually redid all 4 files from scratch (yes, completely from scratch, none of that easy ctrl-c ctrl-v stuff) on 9/28 becuase of some unrelenting segfault. that was "fun"

*/

#include "free_list.hpp"

using namespace std;

////////Segment Header///////////////
SegmentHeader::SegmentHeader(size_t _length) {	//Initializes the SegmentHeader, sets variables appropriately
	length = _length;
	cookie = COOKIE_VALUE;
	next = nullptr; prev = nullptr;
	lr = NULL; inherit = NULL;
}
SegmentHeader::~SegmentHeader() {				//Makes the SegmentHeader variables all null, helps with avioding false-positives during combineSegments in MyAllocator
	next = nullptr; prev = nullptr;
	length = NULL;
	lr = NULL; inherit = NULL;
	is_free = NULL;
}

//getters, self explanatory
size_t SegmentHeader::getLength() { return length; }
bool SegmentHeader::getIsFree() { return is_free; }
SegmentHeader* SegmentHeader::getPrev() { return prev; }
SegmentHeader* SegmentHeader::getNext() { return next; }
char SegmentHeader::getLR() { return lr; }
char SegmentHeader::getInherit() { return inherit; }

//setters, self explanatory
void SegmentHeader::setLength(size_t _length) { length = _length; }
void SegmentHeader::setIsFree(bool _is_free) { is_free = _is_free; }
void SegmentHeader::setPrev(SegmentHeader* _prev) { prev = _prev; }
void SegmentHeader::setNext(SegmentHeader* _next) { next = _next; }
void SegmentHeader::setLR(char _lr) { lr = _lr; }
void SegmentHeader::setInherit(char _inherit) { inherit = _inherit; }

bool SegmentHeader::CheckValid() {		//Checks if the SH is valid
	if (cookie != COOKIE_VALUE) return false;
	else return true;
}
////////END SEGMENT HEADER///////////////



////////FREE LIST///////////////
FreeList::FreeList(size_t _flSize) {	//Constructor, sets value for flSize which is unchangable (because once MyAllocator is constructed, the freelists should stay at the same index)
	head = nullptr;
	flSize = _flSize;
}

//getters, self explanatory
SegmentHeader* FreeList::getHead() { return head; }
size_t FreeList::getSize() { return flSize; }

bool FreeList::Add(SegmentHeader* _segment) {
	if (!_segment->CheckValid() || _segment->getLength() != flSize)	return false;	//returns false if the segment isn't valid or is the incorrect size for this list
	if (head == nullptr)															//makes segment head if fl is empty
		head = _segment;
	else {																			//makes segment head and makes previous head the next pointer for this segment
		_segment->setNext(head);
		head->setPrev(_segment);
		head = _segment;
	}
	_segment->setIsFree(true);														//since the segment has been added to the freeList, it sets is_free to true
	return true;
}
bool FreeList::Remove(SegmentHeader* _segment) {		//Removes passed segment from the freeList
	if (!_segment->CheckValid() || _segment->getLength() != flSize || head == nullptr) return false;	//returns false if the segment is invalid, is the incorrect size for this list, or if the list is empty
	if (_segment == head) {								//if the segment is the head it makes the next segment the new head
		head = head->getNext();
		_segment->setNext(nullptr);
		_segment->setPrev(nullptr);
		_segment->setIsFree(false);
		return true;
	}
	SegmentHeader* temp = head;							//assigns temp to what the current head is
	while (temp != _segment && temp != nullptr) {		//makes sure the segment is in fact in this list
		temp = temp->getNext();
	}
	if (temp == nullptr)								//returns false if the segment isn't in this list
		return false;
	if (_segment->getNext() == nullptr) {				//if segment is at the back of the fl
		_segment->getPrev()->setNext(nullptr);
	}
	else {												//if segment is in the middle of the fl
		_segment->getPrev()->setNext(_segment->getNext());
		_segment->getNext()->setPrev(_segment->getPrev());
	}
	_segment->setNext(nullptr);							//clears the prev and next of this segment and sets is_free to false
	_segment->setNext(nullptr);
	_segment->setIsFree(false);
	return true;
}