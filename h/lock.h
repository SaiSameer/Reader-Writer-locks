/*
 * lock.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Shriya
 */

#ifndef LOCK_H_
#define LOCK_H_

#ifndef	NLOCKS
#define	NLOCKS		50	/* number of locks, if not defined	*/
#endif

#define	LFREE	'\01'		/* this lock is free		*/
#define	LUSED	'\02'		/* this lock is used		*/
#define READ	0			/* Acquired by a reader*/
#define WRITE	1			/* Acquired by a writer*/

typedef struct llist {
	struct llist *lnext;
	int item;
}llist;

struct	lentry	{		/* Lock table entry		*/
	char	lstate;		/* the state LFREE or LUSED		*/
	int	ltype;		/* the type READ or WRITE or DELETED*/
	int	lckcnt;		/* count for this lock		*/
	int	lqhead;		/* lq index of head of list		*/
	int	lqtail;		/* lq index of tail of list		*/
	llist * lhead;
	llist * remprocs;
	int lprio;
};

extern	struct	lentry	locktab[];
extern	int	nextlock;
extern	long ctr1000;

#define	isbadlock(l)	(l<0 || l>=NLOCKS)

llist* addlist(int item, llist* lhead);
llist* removelist( llist* lhead,int item);
llist* release(int lock, int pid);
int acqlock(int pid, int lock, int type);
int ppriority(int pid);

#endif /* LOCK_H_ */
