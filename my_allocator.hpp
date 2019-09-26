/* 
    File: my_allocator.hpp

	Author: Samuel Wilkins IV
			Department of Computer Science
			Texas A&M University
	Date  : 9/16/19

    Modified:

*/

#ifndef _my_allocator_hpp_                   // include file only once
#define _my_allocator_hpp_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>
#include "free_list.hpp"
#include <vector>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

typedef void * Addr; 

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   MyAllocator */
/*--------------------------------------------------------------------------*/

class MyAllocator {

 private:

  // YOU MAY NEED TO ADD YOUR OWN DATA STRUCTURES HERE.

	 Addr p;
	 Addr initP;	//Where address where std::malloc() was called, used for destructor
	 int initSize;
	 size_t basicBlockSize;
	 std::vector <FreeList*> fls;
	 int getFibNum(int f);	//returns smalles fibo number greater than f
	 int fiboToN(int f);	//input fibo number, tells you which fibo number it is 1|1, 2|2, 3|3, 4|5, 5|8,...
	 int nToFibo(int n);	//input which fibo number you want, returns fibo (invers of fiboToN())
	 SegmentHeader* splitSeg(SegmentHeader* curSeg, int requestedBlocks);
	 bool combineSegs(SegmentHeader* fSeg);	//Returns true if segments were combined, false if not
  
 public:
  MyAllocator(size_t _basic_block_size, size_t _size); 
  /* This function initializes the memory allocator and makes a portion of 
     ’_size’ bytes available. The allocator uses a ’_basic_block_size’ as 
     its minimal unit of allocation. 
     NOTE: In the old days one used 'unsigned int' for the size. The C
     Standards Committee has added the 'size_t' data type to ensure to 
     represent memory sizes. This makes the code more portable. Depending
     on the architecture, 'size_t' may or may not be the same 
     as 'unsigned int'. 
  */ 

  ~MyAllocator(); 
  /* This function returns any allocated memory to the operating system. 
  */ 

  Addr Malloc(size_t _length); 
  /* Allocate _length number of bytes of free memory and returns the 
     address of the allocated portion. Returns nullptr when out of memory. 
  */ 

  bool Free(Addr _a); 
  /* Frees the section of physical memory previously allocated 
     using ’Malloc’. Returns true if everything ok. */ 

  void dumpFls();		//For debugging, REMOVE BEFORE SUBMIT

  void checkFls();
};

#endif 
