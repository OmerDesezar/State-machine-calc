/*
    team: OL125-126    
    version: 1.01
    date: 27/04/2022
*/
#ifndef __STACK_H__
#define __STACK_H__

#include <stddef.h> /* size_t */

struct stack;

typedef struct stack *stack_ptr_t;
typedef struct stack stack_t;

/*
 * Recommended max size handling:
 *
 * #define MAX_CAPACITY_IN_BYTES 1048576
 *
 *
 *
 * Recommended struct impl:
 *
 * struct stack
 * {
 *		char * top;
 *		size_t size_of_element;
 *		size_t capacity;
 *		size_t size;
 * }
 *
 */



/* DESCRIPTION:
 * Function creates an empty stack of specified capacity, with total storage space being limited by implementation defined constant.
 *
 * PARAMS:
 * capacity 	- the max capacity in elements of the stack to be created
 * element_size - size of each element to be stored in the array
 *         
 * RETURN:
 * Returns a pointer to the new stack, or NULL on error
 */

stack_ptr_t StackCreate(size_t capacity, size_t element_size);

/* DESCRIPTION:
 * Function destroys and performs cleanup on the given stack,
 * but not on any passed elements.
 *
 * PARAMS:
 * stack - pointer to the stack to be destroyed
 *         
 * RETURN:
 * void
 */

void StackDestroy(stack_ptr_t stack);

/* DESCRIPTION:
 * Function pushes given element address into the stack
 * trying to push into a full stack will result in undefined behavior
 *
 * PARAMS:
 * stack 			- pointer to the stack to push into
 * element_to_push  - pointer to the element to push
 *         
 * RETURN:
 * void
 */

void StackPush(stack_ptr_t stack, const void * element_to_push);

/* DESCRIPTION:
 * Function removes the top element of the stack and returns it
 * trying to pop an empty stack will result in undefined behavior
 *
 * PARAMS:
 * stack - pointer to the stack to pop from
 *         
 * RETURN:
 * pointer to the stored variable.
 */

void * StackPop(stack_ptr_t stack);

/* DESCRIPTION:
 * Function checks whether our stack is empty
 *
 * PARAMS:
 * stack - pointer to the stack to check if empty
 *         
 * RETURN:
 * 1 if the stack is empty or 0 otherwise
 */

int StackIsEmpty(const stack_ptr_t stack);

/* DESCRIPTION:
 * Function gets the size of the given stack
 *
 * PARAMS:
 * stack		  - pointer to the stack to get the size of
 *         
 * RETURN:
 * the size of the given stack
 */

size_t StackGetSize(const stack_ptr_t stack);

/* DESCRIPTION:
 * Function gets the capacity of the given stack
 *
 * PARAMS:
 * stack		  - pointer to the stack to get the capacity of
 *         
 * RETURN:
 * the capacity of the given stack
 */

size_t StackGetCapacity(const stack_ptr_t stack);

/* DESCRIPTION:
 * Function returns the top element of the array
 * trying to peek into an empty stack will result in undefined behavior 
 *
 * PARAMS:
 * stack		  - pointer to the stack to peek_on
 *         
 * RETURN:
 * the stored variable cast into void pointer
 */

void * StackPeek(const stack_ptr_t stack);

#endif /* __STACK_H__ */

