#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "malloc.h"

typedef struct BlockList
{
	unsigned char     in_use;   // is this item in use
	size_t            size;     // size of item
	struct BlockList *next;     // next item
} BlockList_t;

#define BLOCK_SIZE sizeof(BlockList_t)

// This is our main global variable with this we can get where we
// need to get given a size to malloc
BlockList_t  *p_BlockList = NULL;


BlockList_t * 
find_add_block(int n)
{
	BlockList_t *p_tmp = p_BlockList;
	BlockList_t *p_prev = NULL;
	
	while(p_tmp)
	{
		// Best Fit
		if((p_tmp->in_use == 0) && 
		   (p_tmp->size >= n && (p_tmp->size - n <= 4)))  
		{
			return p_tmp;
		}
		
		p_prev = p_tmp;
		p_tmp = p_tmp->next;
	}

	// found nothing
	// add new one to list
	p_tmp = sbrk(BLOCK_SIZE + n);
	p_tmp->size = n;
	p_tmp->next = NULL;
	if(p_prev != NULL) {
		p_prev->next = p_tmp;
	}
	else {
		p_BlockList = p_tmp; // First ever block set global
	}
	return p_tmp;
}

void myfree(void * p)
{
	BlockList_t *p_tmp = p_BlockList;
	
	while(p_tmp)
	{
		// printf("%p  %p\n", p, p_tmp+1);

		// pointer math moves p_tmp to the area of user memory
		if(p_tmp + 1 == p) {
			p_tmp->in_use = 0;
			fprintf(stderr, "Freed %zu bytes\n", p_tmp->size);
			//printf("FREED %p\n", p);
			return;
		}
		p_tmp = p_tmp->next;
	}
}

void *mymalloc(size_t n)
{
	BlockList_t *p_block = find_add_block(n);

	if(p_block == NULL)
	{
		printf("Severe find_add_block\n");
		return NULL;
	}

	fprintf(stderr, "malloc %zu bytes\n", p_block->size);
	// Set one time - set it here
	p_block->in_use = 1;
	//printf("ALLOC %p\n", p_block + 1);
	return p_block + 1;
}

void *mycalloc(size_t nmemb, size_t s)
{
	size_t sz = nmemb * s;

	void *p_data = (void *) mymalloc(sz);
	if(p_data == NULL)
	{
		printf("mycalloc failure\n");
		return NULL;
	}

	fprintf(stderr, "calloc %zu bytes\n", sz);
	//printf("CALLOC %p\n", p_data);

	// init user memory to 0
	memset(p_data, 0, sz);

	return p_data;
}

void printAll()
{
	int i = 0;

	BlockList_t *p_tmp_blk = p_BlockList;
	while(p_tmp_blk)
	{
		printf("Node[%d]\n", ++i);
		printf("   Size  ---> %zu\n", p_tmp_blk->size);
		printf("   In Use --> %d\n", p_tmp_blk->in_use);
		p_tmp_blk = p_tmp_blk->next;
	}
}

/*

int main()
{
	int i;
	char *buf = (char *) mymalloc(80);

	buf = (char *) mymalloc(80);
	buf = (char *) mymalloc(80);
	buf = (char *) mymalloc(30);
	buf = (char *) mymalloc(30);
	buf = (char *) mymalloc(20);
	buf = (char *) mymalloc(100);
	buf = (char *) mymalloc(98);
	myfree(buf);
	buf = (char *) mymalloc(99);
	buf = (char *) mymalloc(99);
	buf = (char *) mymalloc(15);
	buf = (char *) mymalloc(15);
	buf = (char *) mymalloc(15);
	buf = (char *) mymalloc(15);
	buf = (char *) mymalloc(15);
	buf = (char *) mymalloc(3);
	buf = (char *) mymalloc(2);
	buf = (char *) mymalloc(1);
	myfree(buf);
	buf = (char *) mymalloc(1);
	myfree(buf);
	buf = (char *) mymalloc(1);
	myfree(buf);
	buf = (char *) mymalloc(2);
	buf = (char *) mymalloc(1);
	buf = (char *) mymalloc(1);
	buf = (char *) mymalloc(1);
	buf = (char *) mymalloc(14);
	buf = (char *) mymalloc(95);

	
	buf = (char *) mymalloc(6000);
	myfree(buf);
	buf = (char *) mymalloc(10000); // comment this out to crash program
	buf = (char *) mymalloc(1000);
	buf = (char *) mymalloc(1);
	char *buf1 = (char *) mycalloc(128,1);
	buf = (char *) mycalloc(128,1);

	myfree(buf1);
	myfree(buf);
	printAll();
}

*/
