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
static void deleteInList(header *p);
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
	// Allocates a memory of size bytes 
	header *memory = calloc(size, sizeof(*memory));
	if (memory == NULL)
	{
		fprintf(stderr, "Error in allocating %d bytes", size);
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
		fprintf(stderr, "Error in allocating array with %d bytes", size);
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
		// memory->size = memorySize;
		deleteInList(memory);
		// Split the memory into parts if it is bigger than total size and MIN_CHUNK
	}else
	{
		splitChunks(memory, size, memorySize);
	}
	// Point to the starting point of writing the data 
	memory += 1;
	
	return memory;
}

/** Deallocate a chunk of memory. */
void myFree (void *obj)
{
	/// TODO ///
	addr temp = (addr)obj - sizeof(header);
	header *chunk = (header *)temp;
	// if the obj is pointing randomly in the memory
	if (chunk->status != ALLOC)
	{
		fprintf(stderr, "Error in freeing memory");
		exit(1);
	}
	chunk->status = FREE;
	insertInList(chunk);
	mergeInList();
	printf("No elements %d \n", Heap.nFree);
}

// Merges if chunks are together 
static void mergeInList()
{	
	for (int i = 0; i < (Heap.nFree); i++)
	{
		header *chunk = (header *)Heap.freeList[i];
		
		while (((addr)chunk + chunk->size) == (addr)Heap.freeList[i+1])
		{
			header *next = (header *)Heap.freeList[i+1];
			chunk->size = chunk->size + next->size;
			deleteInList(Heap.freeList[i+1]);
		}
	}
}
//Split the chunks into parts 
static void splitChunks(header *p, int size, int memorySize)
{
	int totalSize = size + sizeof(header);

	// split the memory into a newChunk
	addr new = (addr)p;
	new += size + sizeof(header);
	header *newChunk = (header *)new;
	newChunk->status = FREE;
	newChunk->size = p->size - totalSize;
	p->size = size + sizeof(header);

	// Add the pointer of new chunk into the free list
	int pos = findInList(p);
	if (pos == -1)
	{
		fprintf(stderr, "Error in finding the position of the pointer");
		return;
	}
	Heap.freeList[pos] = newChunk;
}

// Delete the recieved pointer from the list 
static void deleteInList(header *p)
{
	// Find the position of the pointer in the list 
	int pos = findInList(p);
	if (pos == -1)
	{
		fprintf(stderr, "Error in finding the position of the pointer");
		return;
	}
	// Deleting the pointer 
	for (int i = pos ; i < Heap.nFree - 1; i++)
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
		header *curr = (header *)Heap.freeList[i];
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
	if(Heap.nFree == 0)
	{
		fprintf(stderr, "No more free chunks available");
		return NULL;
	}

	// header *chunk = ((header *)Heap.freeList[0]);
	int smallest, found = 1;
	void *returnChunk = NULL;

	// Loop through the list to find the smallest chunk of memory
	for (int i = 0; i < Heap.nFree; i++)
	{
		header *curr =((header *)Heap.freeList[i]);
		if (curr->size > size)
		{
			if (found)
			{
				smallest = curr->size;
				returnChunk = curr;	
				found = 0;
			}else
			{
			// Update the smallest available chunk 
			smallest = curr->size;
			returnChunk = curr;			
			}
		}
	}

	if (found)
	{
		fprintf(stderr, "Error: required size not available in memory");\
		return NULL;
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
	}else if (rem == 2)
	{
		size += 2;
	}else if (rem == 3)
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
	addr heapMax =        heapMaxAddr ();
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
