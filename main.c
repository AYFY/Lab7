#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

int main()
{
	void* ptr1 = NULL;
	void* ptr2 = NULL;
	void* ptr3 = NULL;
	void* ptr4 = NULL;
	void* ptr5 = NULL;
	void* ptr6 = NULL;
	void* ptr7 = NULL;
	printf("\nProgram starts here:\n");
	
	heap_init(getpagesize());
	
	ptr1 = _malloc(1000);
	ptr2 = _malloc(500);
	ptr6 = _malloc(100);
	ptr7 = _malloc(5000);
	_free(ptr2);
	_free(ptr1);
	_free(ptr6);
	ptr1 = _malloc(1000);
	memalloc_debug_heap(stderr, HEAP_START);
	printf("Program finished.\n");
	
    return 0;
}
