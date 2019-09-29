#include "free_list.hpp"

using namespace std;

////////Segment Header///////////////
/*
static const unsigned COOKIE_VALUE = 0xBAAB00;
size_t length;
bool is_free;
SegmentHeader* prev;
SegmentHeader* next;
char lr;
char inherit;
*/
SegmentHeader::SegmentHeader(size_t _length) {
	length = _length;
	cookie = COOKIE_VALUE;
	next = nullptr; prev = nullptr;
	lr = NULL; inherit = NULL;
}
SegmentHeader::~SegmentHeader() {
	next = nullptr; prev = nullptr;
	length = NULL;
	lr = NULL; inherit = NULL;
	is_free = NULL;
}

//getters
size_t SegmentHeader::getLength() { return length; }
bool SegmentHeader::getIsFree() { return is_free; }
SegmentHeader* SegmentHeader::getPrev() { return prev; }
SegmentHeader* SegmentHeader::getNext() { return next; }
char SegmentHeader::getLR() { return lr; }
char SegmentHeader::getInherit() { return inherit; }

//setters
void SegmentHeader::setLength(size_t _length) { length = _length; }
void SegmentHeader::setIsFree(bool _is_free) { is_free = _is_free; }
void SegmentHeader::setPrev(SegmentHeader* _prev) { prev = _prev; }
void SegmentHeader::setNext(SegmentHeader* _next) { next = _next; }
void SegmentHeader::setLR(char _lr) { lr = _lr; }
void SegmentHeader::setInherit(char _inherit) { inherit = _inherit; }

bool SegmentHeader::CheckValid() {
	if (cookie != COOKIE_VALUE) return false;
	else return true;
}

////////FREE LIST///////////////
/*
SegmentHeader* head;
size_t flSize;
*/

FreeList::FreeList(size_t _flSize) {
	head = nullptr;
	flSize = _flSize;
}

SegmentHeader* FreeList::getHead() { return head; }
size_t FreeList::getSize() { return flSize; }
bool FreeList::Add(SegmentHeader* _segment) {
	if (!_segment->CheckValid() || _segment->getLength() != flSize)	//returns false if the segment isn't valid or is the incorrect size for this list
		return false;
	if (head == nullptr)	//makes segment head if fl is empty
		head = _segment;
	else {	//makes segment head and makes previous head the next pointer for this segment
		_segment->setNext(head);
		head->setPrev(_segment);
		head = _segment;
	}
	_segment->setIsFree(true);
	return true;
}
bool FreeList::Remove(SegmentHeader* _segment) {
	if (!_segment->CheckValid() || _segment->getLength() != flSize || head == nullptr)	//returns false if the segment is invalid, is the incorrect size for this list, or if the list is empty
		return false;
	if (_segment == head) {		//if the segment is the head it makes the next segment the new head
		head = head->getNext();
		_segment->setNext(nullptr);
		_segment->setPrev(nullptr);
		_segment->setIsFree(false);
		return true;
	}
	SegmentHeader* temp = head;
	while (temp != _segment && temp != nullptr) {	//makes sure the segment is in fact in this list
		temp = temp->getNext();
	}
	if (temp == nullptr)	//returns false if the segment isn't in this list
		return false;
	if (_segment->getNext() == nullptr) {	//if segment is at the back of the fl
		_segment->getPrev()->setNext(nullptr);
	}
	else {		//if segment is in the middle of the fl
		_segment->getPrev()->setNext(_segment->getNext());
		_segment->getNext()->setPrev(_segment->getPrev());
	}
	_segment->setNext(nullptr);		//clears the prev and next of this segment
	_segment->setNext(nullptr);
	_segment->setIsFree(false);
	return true;
}