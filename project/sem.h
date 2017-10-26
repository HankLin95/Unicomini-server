#ifndef _SEMH_H_
#define _SEMH_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>

//必须要手动自己定义这个联合体
union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};


class CSem
{
public:

int sem_create(key_t key, int nums);//创建一组信号量，返回信号量的id

int sem_open(key_t key);

int sem_setval( int pos, int val);

int sem_getval( int pos);

int sem_p( int pos, unsigned short val);

int sem_v( int pos, unsigned short val);

int sem_setall( unsigned short *val);

int sem_getall( unsigned short *val);

int sem_close();

protected:

private:

	int id;

	int num;
	
};

#endif


