/* lqueue.c - ldequeue, lenqueue */

#include <conf.h>
#include <kernel.h>
#include <lq.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * enqueue  --	insert an item at the tail of a list
 *------------------------------------------------------------------------
 */
int lenqueue(int item, int type, int priority, int head)
/*	int	item;			- item to enqueue on a list	*/
/*	int	type;			- type of the item on a list*/
/*	int	priority;		- item priority on a list	*/
/*	int	head;			- index in q of list head	*/
{
	struct	lqent	*hptr;		/* points to head entry		*/
	struct	lqent	*mptr;		/* points to item entry		*/

	hptr = &lq[lq[head].lqnext];
	mptr = &lq[item];
	mptr->lqtype = type;
	while(hptr != EMPTY)
	{
		if(hptr->lqkey >priority){
			hptr = hptr->lqnext;
		}
		else if(hptr->lqkey == priority){
			int ltype = hptr->lqtype;
			if(ltype == READ){
				break;
			}
			hptr = hptr->lqnext;
		}
		else{
			break;
		}
	}
	//mptr->lqprev = lq[hptr->lqnext].lqprev;
	mptr->lqprev = hptr->lqprev;
	mptr->lqnext = lq[hptr->lqprev].lqnext;
	lq[hptr->lqprev].lqnext = item;
	hptr->lqprev = item;
	return(item);
}


/*------------------------------------------------------------------------
 *  ldequeue  --  remove an item from the head of a list and return it
 *------------------------------------------------------------------------
 */
int ldequeue(int item)
{
	struct	lqent	*mptr;		/* pointer to lq entry for item	*/

	mptr = &lq[item];
	lq[mptr->lqprev].lqnext = mptr->lqnext;
	lq[mptr->lqnext].lqprev = mptr->lqprev;
	return(item);
}

/*------------------------------------------------------------------------
 * getfirstl  --	 remove and return the first process on a list
 *------------------------------------------------------------------------
 */
int getfirstl(int head)
{
	int	proc;			/* first process on the list	*/

	if ((proc=lq[head].lqnext) < NPROC)
		return( ldequeue(proc) );
	else
		return(EMPTY);
}



/*------------------------------------------------------------------------
 * getlastl  --  remove and return the last process from a list
 *------------------------------------------------------------------------
 */
int getlastl(int tail)
{
	int	proc;			/* last process on the list	*/

	if ((proc=lq[tail].lqprev) < NPROC)
		return( ldequeue(proc) );
	else
		return(EMPTY);
}
