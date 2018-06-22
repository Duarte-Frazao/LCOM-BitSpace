//TO-DO organize the struct as the professor told in class
#ifndef __QUEUE_H
#define __QUEUE_H

/** @defgroup Queue Queue
 * @{
 *  Queue
 */

typedef struct node node;
typedef struct queue queue;


/** @name Node */
/**@{
 *
 * Node struct
 */
struct node
{
	char value;		/**< @brief Node value */
	node* next;		/**< @brief Next node in queue */
};
/** @} end of node struct*/

/** @name queue */
/**@{
 *
 * queue struct
 */
struct queue
{
	node* first;		/**< @brief First queue node */
	node* last;		/**< @brief Last queue node */

	unsigned char size;		/**< @brief Queue size */
};
/** @} end of queue struct*/


/**
 * @brief Creates a new queue object
 *
 * @return Returns queue object
 */
queue* createQueue();

/**
 * @brief Pops node from queue
 *
 * @param queue Queue to pop node
 *
 * @return Returns character from popped node
 */
char popQueue(queue *q);

/**
 * @brief Gets value of the first node of the queue
 *
 * @param queue Queue to analyze
 *
 * @return Returns value of the first node of the queue
 */
char firstValueQueue(queue *q);

/**
 * @brief Gets value of the last node of the queue
 *
 * @param queue Queue to analyze
 *
 * @return Returns value of the last node of the queue
 */
char lastValueQueue(queue *q);

/**
 * @brief Pops every node from the queue
 *
 * @param queue Queue to clean
 *
 * @return Returns 0 on sucess, 1 on error
 */
int cleanQueue(queue *q);

/**
 * @brief Push a new new node with desired value to the queue
 *
 * @param queue Queue to push to
 * @param c Character to push
 *
 * @return Returns 0 on sucess, 1 on error
 */
int pushQueue(queue * q, char c);

/**
 * @brief Checks is a queue is empty
 *
 * @param queue Queue to analyze
 *
 * @return Returns 1 if empty, 0 if not
 */
int isEmptyQueue(queue *q );

/**
 * @brief Deletes queue
 *
 * @param queue Queue to delete
 *
 * @return
 */
void deleteQueue(queue *q);

/**
 * @brief Prints queue
 *
 *	Debug purposes
 *
 * @param queue Queue to print
 *
 * @return
 */
void printQueue(queue *q);

/**
 * @brief Gets size of queue
 *
 * @param queue Queue to analyze
 *
 * @return Returns size of queue
 */
float sizeQueue(queue *q);

/** @} end of Queue*/

#endif
