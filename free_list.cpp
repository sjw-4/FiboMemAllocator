/* 
    File: free_list.cpp

    Author: Samuel Wilkins IV
            Department of Computer Science
            Texas A&M University
    Date  : 9/16/19

    Modified: 

    This file contains the implementation of the class FreeList.

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include "free_list.hpp"

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
/* FUNCTIONS FOR CLASS SegmentHeader */
/*--------------------------------------------------------------------------*/

SegmentHeader::SegmentHeader(size_t _length, bool _is_free) {
  length = _length;
  is_free = _is_free;
  cookie = COOKIE_VALUE;
  next = nullptr; prev = nullptr;
  lr = 'l';
  inherit = 'l';
}


SegmentHeader::~SegmentHeader() {
	next = nullptr; prev = nullptr;
	length = -1;
	lr = 'z'; inherit = 'z';
}


bool SegmentHeader::CheckValid() {
	if (cookie != COOKIE_VALUE) return false;
	else return true;
}

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS FreeList */
/*--------------------------------------------------------------------------*/

FreeList::FreeList() {
	head = nullptr;
}

FreeList::FreeList(SegmentHeader* _segment) {
	head = nullptr;
	Add(_segment);
}

FreeList::FreeList(void* addr, size_t _size) {
	head = nullptr;
	Add(new (addr) SegmentHeader(_size));
}

FreeList::~FreeList() {
}

bool FreeList::Add(SegmentHeader * _segment) {
	//cout << "Adding size " << _segment->length << endl;
	if (head == nullptr) {
		head = _segment;
	}
	else {
		head->prev = _segment;
		_segment->next = head;
		head = _segment;
	}
	_segment->is_free = true;
	return true;
}

bool FreeList::Remove(SegmentHeader * _segment) {
	if (_segment == head && head->next == nullptr) {	//segment is only SH in FL
		head = nullptr;
	}
	else if (_segment == head) {		//segment is head in FL, makes next item in FL head
		_segment->next->prev = nullptr;
		head = head->next;
	}
	else if (_segment->next == nullptr) {	//segment is tail in FL, makes prev item the new tail
		_segment->prev->next = nullptr;
	}
	else {		//segment is somewhere in the middle, removes it and sets proper next and prev pointers
		_segment->next->prev = _segment->prev;
		_segment->prev->next = _segment->next;
	}
	_segment->is_free = false;
	_segment->prev = nullptr; _segment->next = nullptr;
	return true;
}
