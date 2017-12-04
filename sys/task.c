#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#define DEFAULT_LOCK_PRIO 20

void lock_reader (char *msg, int lck);
void normal_process (char *msg);
void lock_writer (char *msg, int lck);
void semaphore_reader_writer (char *msg, int sem);
void task1 ();

void lock_reader(char *msg, int lck)
{
	kprintf ("  %s: to acquire lock\n", msg);
	lock (lck, READ, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired lock\n", msg);
	sleep(2);
	int count = 100;
	int i;
	kprintf("  %s started working\n", msg);
	while(count > 0)
	{
		for (i = 0; i < 1000000; ++i);
		count--;
	}
	kprintf("  %s completed its work\n", msg);
	kprintf ("  %s: to release lock\n", msg);
	int r = releaseall (1, lck);
}

void normal_process(char *msg)
{
    sleep(2);
	int     ret;
	int i;
	int count = 100;
	kprintf("  %s started working\n", msg);
	while(count > 0)
	{
		for (i = 0; i < 1000000; ++i);
		count--;
	}
	kprintf("  %s completed its work\n", msg);
}

void lock_writer(char *msg, int lck)
{
	sleep(2);
	kprintf ("  %s: to acquire lock\n", msg);
	lock (lck, WRITE, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired lock, sleep 10s\n", msg);
	int count = 100,i;
	kprintf("  %s started working\n", msg);
	while(count > 0)
	{
		for (i = 0; i < 1000000; ++i);
		count--;
	}
	kprintf("  %s completed its work\n", msg);
	kprintf ("  %s: to release lock\n", msg);
	int r = releaseall (1, lck);
}

void semaphore_reader_writer(char *msg, int sem)
{
	kprintf ("  %s: to acquire sem\n", msg);
	wait(sem);
	kprintf ("  %s: acquired sem\n", msg);
	sleep(4);
	int count = 100;
	int i;
	kprintf("  %s started working\n", msg);
	while(count > 0)
	{
		for (i = 0; i < 1000000; ++i);
		count--;
	}
	kprintf("  %s completed its work\n", msg);
	kprintf ("  %s: to release sem\n", msg);
	signal(sem);
}

void task1()
{

	int lck, rd1, rd2, wr1;

	kprintf("\nTask 1: Comparing Semaphore and reader/writer locks\n");

	lck  = lcreate ();

	rd1 = create(lock_reader, 2000, 10, "reader", 2, "Reader L", lck);
	rd2 = create(normal_process, 2000, 20, "normal process", 1, "Normal M");
	wr1 = create(lock_writer, 2000, 30, "writer", 2, "Writer H", lck);

	kprintf("-start reader A\n");
	resume(rd1);
	sleep (1);

	kprintf("-start writer\n");
	resume(wr1);
	sleep (1);

	kprintf("-start M\n");
	resume(rd2);
	sleep (5);

	kprintf("\n\nSemaphore implementation\n\n");
	int sem, sem_rd1, sem_rd2, sem_wr1;
	sem = screate(1);

	sem_rd1 = create(semaphore_reader_writer, 2000, 10, "reader", 2, "Reader L", sem);
	sem_rd2 = create(normal_process, 2000, 20, "normal process", 1, "Normal M");
	sem_wr1 = create(semaphore_reader_writer, 2000, 30, "writer", 2, "Writer H", sem);

	kprintf("-start reader A\n");
	resume(sem_rd1);
	sleep (1);

	kprintf("-start writer\n");
	resume(sem_wr1);
	sleep (1);

	kprintf("-start M\n");
	resume(sem_rd2);
	sleep (1);

	sleep (5);
	kprintf ("Task 1 Completed!\n");
}
