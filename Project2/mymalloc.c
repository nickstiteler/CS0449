// Nicholas Stiteler
// cs 0449
// project 2
// Prof Billingsley 

// Successfully runs all tests in bigdriver.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "mymalloc.h"

// USE THIS GODDAMN MACRO OKAY
#define PTR_ADD_BYTES(ptr, byte_offs) ((void*)(((char*)(ptr)) + (byte_offs)))

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8

typedef struct Node
{
	int size; //size of data 
	int usable; //1 for an open block, 0 for a close block 
	struct Node* nextNode; //pointer to next block
	struct Node* prevNode; //pointer to previous block
}Node;

void* heapStart = NULL;

Node* head = NULL;
Node* tail = NULL;

Node* findBestFit(int size) // alg for finding the best block to give the user
{	
	int currSize; // size of the current best
	Node* currBest = NULL; // standing best that gets returned 
	Node* temp = NULL; // node for itterating 

	if(head == NULL) // returns NULL if the list isn't initializec
	{
		return NULL;
	}

	for(temp = head; temp != NULL; temp = temp->nextNode)
	{
		if(temp->usable == 1)
		{
			if( temp-> size >= size)
			{
				if(currBest == NULL)
				{
					currBest = temp;
					currSize = (temp -> size - size);
				}
				if(temp-> size == size) //checks if block is a perfect fit
				{
					return temp;
				}
				else if((temp-> size - size)<currSize) // new best check
				{
					currBest = temp;
					currSize = (temp -> size - size);
				}
			}
		}
		if(temp == tail) // tail reached then break
		{
			break;
		}
	}

	if(currBest == NULL)
	{
		return NULL;
	}
	return currBest;
}

Node* createNode(int size, Node* n) // creates a new node and adds it to the LL
{
	Node* newNode = findBestFit(size); // calls bestfit for given size

	// no best fit found so increase the size of the heap
	// 
	if(newNode == NULL) 
	{
		void* address = sbrk(sizeof(Node));

		//update node info
		Node* newNode = address;
		newNode -> size = size;
		newNode -> usable = 0;
		newNode -> nextNode = NULL;
		
		if(n != NULL)
		{
			newNode -> prevNode = n;
			n -> nextNode = newNode;
		}
		else
		{
			newNode -> prevNode = NULL;
		}
		tail = newNode; 
		return newNode;
	}	
	else 
	{
		if(newNode->size > size && (newNode->size - size) >= 32) //split check
		{
			int prevSize = newNode -> size;

			newNode -> size = size;

			newNode -> usable = 0;

			Node* newBlock = newNode;

			newBlock = PTR_ADD_BYTES(newBlock,sizeof(Node));
			newBlock = PTR_ADD_BYTES(newBlock, size);

			//update node info
			newBlock -> usable = 1;
			newBlock -> size = (prevSize - size - sizeof(Node));
			newBlock -> prevNode = newNode; 
			newBlock -> nextNode = newNode -> nextNode;

			newNode -> nextNode = newBlock;
		}
		else if((newNode -> size - size) < 32)
		{
			newNode -> usable = 0;
		}
		if(n == newNode)
		{
			tail = newNode;

			newNode -> nextNode = NULL;
			newNode -> usable = 0;
		}
		return newNode;
	}
}

unsigned int round_up_size(unsigned int data_size) {
	if(data_size == 0)
		return 0;
	else if(data_size < MINIMUM_ALLOCATION)
		return MINIMUM_ALLOCATION;
	else
		return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

/*
void list_print() //prints structure of the heap
{
	Node* temp = NULL;
	int i = 0;
	for(temp = head; temp != NULL; temp = temp ->nextNode)
	{
		printf("Block: %d Size: %d",i,temp->size);
		if(temp -> usable == 1)
		{
			printf(" is usable");
			if(temp == tail)
			{
				printf(" tail\n");
			}
			else if(temp == head)
			{
				printf(" head\n");
			}
			else
			{
				printf("\n");
			}
		}
		else
		{
			printf(" is unusable");
			if(temp == tail)
			{
				printf(" tail\n");
			}
			else if(temp == head)
			{
				printf(" head\n");
			}
			else
			{
				printf("\n");
			}
		}
		i++;
	}
	printf("\n");
}
*/

void* my_malloc(unsigned int size) {
	if(size == 0)
		return NULL;

	size = round_up_size(size);

	// ------- Don't remove anything above this line. -------

	/*
	list_print(); // uncomment to see the print of each LL malloced
	*/

	if(heapStart == NULL)
	{
		heapStart = sbrk(0);
	}

	if(head == NULL) // check to see if list is initialized if not it creates it
	{
		head = createNode(size,tail);
		tail = head; 
		void* nodePointer = sbrk(size); // pointer to newly allocated block
		return nodePointer;
	}

	Node* newNode = createNode(size,tail);
	if(newNode == tail)
	{
		void* nodePointer = sbrk(size);
		return nodePointer;
	}
	else
	{
		int nodePointer;
		nodePointer = ((int)newNode +16);

		return (void*) nodePointer;
	}
}

// coalesce the current node with the node to it's right
Node* coalesceRight(Node* curr, Node* right) 
{
	int currSize = curr -> size;
	int rightSize = right -> size;
	int combSize = currSize + rightSize + sizeof(Node);

	curr -> size = combSize;
	curr -> nextNode = right -> nextNode;

	if(right -> nextNode != NULL)
	{
		right -> nextNode -> prevNode = curr;
	}
	if(right == tail)
	{
		tail = curr;
	}
	return curr;
}
//coalesce the current node with the node to it's left
Node* coalesceLeft(Node * curr, Node* left)
{
	int currSize = curr -> size;
	int leftSize = left -> size;
	int combSize = currSize + leftSize + sizeof(Node);

	left -> size = combSize;
	left -> nextNode = curr -> nextNode;

	if(curr -> nextNode != NULL)
	{
		curr -> nextNode -> prevNode = left;
	}
	if(curr == tail)
	{
		tail = left;
		left-> nextNode = NULL;
		if(left -> prevNode != NULL)
		{
			left -> prevNode ->nextNode = NULL;
		}
	}
	return left;
}
// coalesces nodes on both sides of the current 
Node* coalesceBoth(Node* curr, Node* right, Node* left)
{
	int currSize = curr -> size;
	int rightSize = right -> size;
	int leftSize = left -> size;
	int combSize = currSize + leftSize + sizeof(Node) + rightSize + sizeof(Node);

	left -> size = combSize;
	left -> nextNode = right->nextNode;

	if(right ->nextNode != NULL)
	{
		right -> nextNode -> prevNode = left;
	}
	if(tail == right)
	{
		tail = left;
	}
	return left;
}

int checkForFree(Node* curr)
{
	if(curr != NULL && curr -> usable == 1)
	{
		return 1;
	}
	return 0;
}

void my_free(void* ptr) {
	if(ptr == NULL)
		return;

	// and here's where you free stuff.
	\
	Node* node = ptr - 16;
	node -> usable = 1;

	// creates nodes and ints for both the neigboring blocks
	Node* next = node -> nextNode;
	Node* previous = node -> prevNode;
	int nextColl = checkForFree(next);
	int prevColl = checkForFree(previous);

	// checks which are able to be coalesed and coalesces them
	if(nextColl == 1 && prevColl == 1) // both blocks are free
	{
		node = coalesceBoth(node,next,previous);
	}
	else if(nextColl == 1) // right block is free
	{
		node = coalesceRight(node,next);
	}
	else if(prevColl == 1) //left block is free
	{
		node = coalesceLeft(node,previous);
	}

	if(node == tail) // the block that user would like to free is the last block
	{
		if(tail != head)
		{
			tail = tail ->prevNode;
			node -> prevNode -> nextNode = NULL;
		}

		unsigned int sizeChange = (0 - (unsigned int)node -> size - sizeof(Node));
		ptr = sbrk(sizeChange);
		void* heapChange = sbrk(0);

		if(heapStart == heapChange) // for freeing the entire heap
		{
			head = NULL;
			tail = NULL;
		}
	}
	else if(tail != NULL && (int)tail -> usable == 1)
	{
		unsigned int sizeChange = (0 - (unsigned int)tail -> size - sizeof(Node));
		ptr = sbrk(sizeChange);
	}
	return;
}
