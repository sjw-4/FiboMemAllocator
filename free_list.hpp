#ifndef _free_list_hpp_
#define _free_list_hpp_

#include <cstdlib>

class SegmentHeader {
private:
	static const unsigned COOKIE_VALUE = 0xBAAB00;
	unsigned int cookie; /* To check whether this is a genuine header! */
	size_t length;
	bool is_free;
	SegmentHeader* prev;
	SegmentHeader* next;
	char lr;
	char inherit;
public:
	SegmentHeader(size_t _length);

	~SegmentHeader();

	size_t getLength();
	bool getIsFree();
	SegmentHeader* getPrev();
	SegmentHeader* getNext();
	char getLR();
	char getInherit();

	void setLength(size_t _length);
	void setIsFree(bool _is_free);
	void setPrev(SegmentHeader* _prev);
	void setNext(SegmentHeader* _next);
	void setLR(char _lr);
	void setInherit(char _inherit);

	bool CheckValid();
};

class FreeList {
private:
	SegmentHeader* head;
	size_t flSize;

public:
	FreeList(size_t _length);

	SegmentHeader* getHead();

	size_t getSize();
	bool Add(SegmentHeader* _segment);
	bool Remove(SegmentHeader* _segment);
};
#endif