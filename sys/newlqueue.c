/* newlqueue.c  -  newlqueue */

#include <conf.h>
#include <kernel.h>
#include <lq.h>

/*------------------------------------------------------------------------
 * newlqueue  --  initialize a new list in the lq structure
 *------------------------------------------------------------------------
 */
int newlqueue()
{
	struct	lqent	*hptr;
	struct	lqent	*tptr;
	int	hindex, tindex;

	hptr = &lq[ hindex=nextlqueue++]; /* assign and rememeber queue	*/
	tptr = &lq[ tindex=nextlqueue++]; /* index values for head&tail	*/
	hptr->lqnext = tindex;
	hptr->lqprev = EMPTY;
	hptr->lqkey  = MAXINT;
	tptr->lqnext = EMPTY;
	tptr->lqprev = hindex;
	tptr->lqkey  = MININT;
	return(hindex);
}
