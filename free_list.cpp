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
}

SegmentHeader::~SegmentHeader() {
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
	if (head == nullptr) {
		head = _segment;
	}
	else {
		head->prev = _segment;
		_segment->next = head;
		head = _segment;
	}
	return true;
}

bool FreeList::Remove(SegmentHeader * _segment) {
	if (_segment == head && head->next == nullptr) {
		head = nullptr;
	}
	else if (_segment == head) {
		_segment->next->prev = nullptr;
		head = head->next;
	}
	else if (_segment->next == nullptr) {
		_segment->prev->next = nullptr;
	}
	else {
		_segment->next->prev = _segment->prev;
		_segment->prev->next = _segment->next;
	}
	return true;
}
