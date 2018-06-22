#include "Queue.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>


queue* createQueue()
{
	queue* q=(queue*) malloc(sizeof(queue));

	//Error in malloc
	if(q==NULL)return NULL;

	q->first=NULL;
	q->last=NULL;

	q->size=0;

	return q;
}


int pushQueue(queue * q, char c)
{
	//allocat space for new node
	node* nodeToAdd= malloc(sizeof(node));
	if(nodeToAdd ==NULL)
	{
		printf("In pushQueue():: Error in malloc\n");
		return 1;
	}

	nodeToAdd->value=c;
	nodeToAdd->next=NULL;


	if(q->size ==0)
	{
		//Case where it's the first node
		q->first=nodeToAdd;
		q->last=nodeToAdd;
	}else
	{
		//Case where it's not the first node
		q->last->next=nodeToAdd;
		q->last=nodeToAdd;
	}

	q->size++;


	return 0;

}

char popQueue(queue *q)
{
	//Store value and pointer to node
	char retrievedValue =q->first->value;
	//node* nodeToEliminate= q->first;

	//Update queue
	q->first= q->first->next;
	q->size--;

	//Get rid of the node
	//free(nodeToEliminate);

	return (retrievedValue);
}

int cleanQueue(queue *q)
{
	while(!isEmptyQueue(q))
	{
		popQueue(q);
	}
	return 0;
}


int isEmptyQueue(queue *q )
{
	return (q->size==0 ? 1:0);
}

float sizeQueue(queue *q)
{
	return (q->size);
}


void deleteQueue(queue *q)
{
	char nodeNum = sizeQueue(q);

	unsigned int i;

	char values;

	for(i=0; i < nodeNum;i++)
	{
		values= popQueue(q);
	}

	free(q);
}

char firstValueQueue(queue *q)
{
		//printf("Size:%d\n",sizeQueue(q) );
		if(q->first==NULL) printf("In firstValueQueue()::  First Value is Null\n" );
		if(q->last==NULL)printf("In lastValueQueue()::  Last Value is Null\n" );
		return (q->first->value);
}

char lastValueQueue(queue *q)
{
		return (q->last->value);
}

//Prints state of queue for debug purposes
void printQueue(queue *q)
{
	node* n=q->first;

	printf("\nCurrent State of Queue: \n");

	//Case where queue is empty
	if(isEmptyQueue(q))
	{
		printf("Empty\n");
		return;
	}

	int count=1;

	while(n!=NULL)
	{
		printf("%d ",  (n->value));
		n=n->next;
		count++;
	}
	printf("\n-----\n");

}
