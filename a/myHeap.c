// COMP1521 19t2 ... Assignment 2: heap management system

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myHeap.h"

/** minimum total space for heap */
#define MIN_HEAP 4096
/** minimum amount of space for a free Chunk (excludes Header) */
#define MIN_CHUNK 32

#define ALLOC 0x55555555
#define FREE  0xAAAAAAAA

/// Types:

typedef unsigned int  uint;
typedef unsigned char byte;

typedef uintptr_t     addr; // an address as a numeric type

/** The header for a chunk. */
typedef struct header {
	uint status;    /**< the chunk's status -- ALLOC or FREE */
	uint size;      /**< number of bytes, including header */
	byte data[];    /**< the chunk's data -- not interesting to us */
} header;

/** The heap's state */
struct heap {
	void  *heapMem;     /**< space allocated for Heap */
	uint   heapSize;    /**< number of bytes in heapMem */
	void **freeList;    /**< array of pointers to free chunks */
	uint   freeElems;   /**< number of elements in freeList[] */
	uint   nFree;       /**< number of free chunks */
};

/// Variables:

/** The heap proper. */
static struct heap Heap;

/// Functions:
static addr heapMaxAddr (void);
static int round(int size);
static header *findSmallestChunk();
static void deleteInList(int pos);
static int findInList(header *p);
static void insertInList(void *p);
static void splitChunks(header *p, int size, int memorySize);
static void mergeInList();

/** Initialise the Heap. */
int initHeap (int size)
{
	Heap.nFree = 0;
	Heap.freeElems = 0;

	// Check the size 
	if (size < MIN_HEAP)
	{
		size = MIN_HEAP;
	}
	// Checks if size is a multiple of 4 
	if (size % 4 != 0)
	{
		size = round(size); 
	}
	Heap.heapSize =  size;

	// Allocates a memory of 'size' bytes 
	header *memory = calloc(size, sizeof(header));
	if (memory == NULL)
	{
		fprintf(stderr, "Error in allocating %d bytes \n", size);
		return -1;
	}
	Heap.heapMem = memory;
	memory->size = size;
	memory->status = FREE;

	// allocate a list with size array_size 
	int array_size = size/MIN_CHUNK;
	void **list = malloc(array_size*sizeof(int));
	if (list == NULL)
	{
		fprintf(stderr, "Error in allocating array with %d bytes \n", size);
		return -1;
	}
	
	// Point to the start of the free memory 
	list[0] = memory;
	Heap.freeList = list;

	Heap.nFree = 1;
	Heap.freeElems = array_size;
	
	return 0; 
}

/** Release resources associated with the heap. */
void freeHeap (void)
{
	free (Heap.heapMem);
	free (Heap.freeList);
}

/** Allocate a chunk of memory large enough to store `size' bytes. */
void *myMalloc (int size)
{
	// Make sure the invalid size is not provided
	if (size < 1)
	{
		return NULL; 
	}
	size = round(size);

	// Returns the smallest valid chunk 
	header *memory = findSmallestChunk(size + sizeof(header));
	if (memory == NULL)
	{
		return NULL;
	}
	int memorySize = memory->size;
	memory->status = ALLOC;
	
	if (memorySize < (size + sizeof(header) + MIN_CHUNK))
	{
		int pos = findInList(memory);
		if (pos == -1)
		{
			fprintf(stderr, "Error in finding the position of the pointer \n");
			return NULL;
		}
		deleteInList(pos);
	}
	else
	{
		// Split the memory into parts if it is bigger than total size and MIN_CHUNK
		splitChunks(memory, size, memorySize);
	}
	// Point to the starting point of writing the data 
	memory += 1;
	return memory;
}

/** Deallocate a chunk of memory. */
void myFree (void *obj)
{
	// Check if a valid pointer is given 
	if (obj == NULL)
	{
		fprintf(stderr, "Attempt to free unallocated chunk\n");
		exit(1);
	}
	// Point to the start of the header
	addr temp = (addr) obj - sizeof(header);
	header *chunk = (header *) temp;

	// if the obj is pointing randomly in the memory then exit 
	if (chunk->status != ALLOC)
	{
		fprintf(stderr, "Error in freeing memory \n");
		exit(1);
	}
	// Update the chunk 
	chunk->status = FREE;
	insertInList(chunk);
	mergeInList();
}

//Split the chunks into parts 
static void splitChunks(header *p, int size, int memorySize)
{
	int totalSize = size + sizeof(header);

	// split the memory into a newChunk
	addr new = (addr)p;
	new += size + sizeof(header);
	header *newChunk = (header *) new;

	// Update the header for both the chunks 
	newChunk->status = FREE;
	newChunk->size = p->size - totalSize;
	p->size = size + sizeof(header);

	// Add the pointer of new chunk into the free list
	int pos = findInList(p);
	if (pos == -1)
	{
		fprintf(stderr, "Error in finding the position of the pointer \n");
		return;
	}
	Heap.freeList[pos] = newChunk;
}

// Merges if free chunks are together in the list 
static void mergeInList()
{	 
	// A forloop to iterate through the list 
	for (int i = 0; i < (Heap.nFree); i++)
	{
		header *chunk = (header *) Heap.freeList[i];
		// Merges all the free adjacent chunks together 
		while (((addr) chunk + chunk->size) == (addr) Heap.freeList[i+1])
		{
			header *next = (header *) Heap.freeList[i+1];
			chunk->size = chunk->size + next->size;
			deleteInList(i+1);
		}
	}
}

// Delete the recieved pointer from the list, if pos not given -1 should be passed instead
static void deleteInList(int pos)
{
	// Deleting the pointer 
	for (int i = pos; i < (Heap.nFree - 1); i++)
	{
		Heap.freeList[i] = Heap.freeList[i+1];
	}
	Heap.nFree--;
}

// Find the position of the recieved pointer from the list 
static int findInList(header *p)
{
	// Loop through the list to find the pointer
	for (int i = 0; i < Heap.nFree; i++)
	{
		header *curr = (header *) Heap.freeList[i];
		if (curr == p)
		{
			return i;
		}
	}
	return -1;
}

// Insert the recieved pointer into the list 
static void insertInList(void *p)
{
	// Cannot insert more elements if array limit reached 
	if (Heap.nFree >= Heap.freeElems) 
        return; 

	// Loop through the free list and add the pointer 
	int i;
	for (i = (Heap.nFree -1); (i >= 0 && Heap.freeList[i] > p); i--)
	{
		Heap.freeList[i+1] = Heap.freeList[i];
	}
   	Heap.freeList[i+1] = p;

   	// Increment number of chunks in free list
   	Heap.nFree++;
}

// Finds the chunk of memory to be allocated 
static header *findSmallestChunk(int size)
{
	// Checks if a free chunk in memory is available
	if (Heap.nFree == 0)
	{
		fprintf(stderr, "No more free chunks available \n");
		exit(1);
	}

	int smallest, found = 1;
	void *returnChunk = NULL;

	// Loop through the list to find the smallest chunk of memory
	for (int i = 0; i < Heap.nFree; i++)
	{
		header *curr = ((header *) Heap.freeList[i]);
		
		if (curr->size >= size)
		{
			// Save the chunk if it is of sufficient size 
			if (found)
			{
				smallest = curr->size;
				returnChunk = curr;	
				found = 0;
			}
			else if (smallest > curr->size)
			{
				// Update the smallest available chunk 
				smallest = curr->size;
				returnChunk = curr;			
			}
		}
	}

	// If no free chunk is available with the required size 
	if (found)
	{
		fprintf(stderr, "Error: required size not available in memory \n");
		exit(1);
	}
	return returnChunk;
}

// Returns the rounded value for the size 
static int round(int size)
{
	int rem = size % 4;

	if (rem == 1)
	{
		size += 3;
	}
	else if (rem == 2)
	{
		size += 2;
	}
	else if (rem == 3)
	{
		size += 1;
	}
	return size; 
}

/** Return the first address beyond the range of the heap. */
static addr heapMaxAddr (void)
{
	return (addr) Heap.heapMem + Heap.heapSize;
}

/** Convert a pointer to an offset in the heap. */
int heapOffset (void *obj)
{
	addr objAddr = (addr) obj;
	addr heapMin = (addr) Heap.heapMem;
	addr heapMax = heapMaxAddr ();
	if (obj == NULL || !(heapMin <= objAddr && objAddr < heapMax))
		return -1;
	else
		return (int) (objAddr - heapMin);
}

/** Dump the contents of the heap (for testing/debugging). */
void dumpHeap (void)
{
	int onRow = 0;

	// We iterate over the heap, chunk by chunk; we assume that the
	// first chunk is at the first location in the heap, and move along
	// by the size the chunk claims to be.
	addr curr = (addr) Heap.heapMem;
	while (curr < heapMaxAddr ()) {
		header *chunk = (header *) curr;

		char stat;
		switch (chunk->status) {
			case FREE:  stat = 'F'; break;
			case ALLOC: stat = 'A'; break;
			default:
			fprintf (
				stderr,
				"myHeap: corrupted heap: chunk status %08x\n",
				chunk->status
			);
			exit (1);
		}

		printf (
			"+%05d (%c,%5d)%c",
			heapOffset ((void *) curr),
			stat, chunk->size,
			(++onRow % 5 == 0) ? '\n' : ' '
		);

		curr += chunk->size;
	}

	if (onRow % 5 > 0)
		printf ("\n");
}
