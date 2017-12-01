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
#define DELETE	2			/* Deleted*/

struct	lentry	{		/* Lock table entry		*/
	char	lstate;		/* the state LFREE or LUSED		*/
	int	ltype;		/* the type READ or WRITE or DELETED*/
	int	lckcnt;		/* count for this lock		*/
	int	lqhead;		/* q index of head of list		*/
	int	lqtail;		/* q index of tail of list		*/
	llist * lhead;	// TODO define plist
	int lprio;
};

typedef struct llist {
	struct llist *lnext;
	int item;
}llist;
extern	struct	lentry	locktab[];
extern	int	nextlock;

#define	isbadlock(l)	(l<0 || l>=NLOCKS)

llist* addlist(int item, llist* lhead);

#endif /* LOCK_H_ */
