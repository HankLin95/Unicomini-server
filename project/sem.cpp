#include "sem.h"


//创建一组信号量，返回信号量的id
int CSem::sem_create(key_t key, int nums)
{
	this->id = semget(key, nums, IPC_CREAT|0766);
	if (id < 0)
	{
		perror("semget error ");
		return -1;
	}
	this->num=nums;
	
	return id;
}

int  CSem::sem_open(key_t key)
{
	this->id = semget(key, 0, 0);
	if (this->id < 0)
	{
		perror("semget error ");
		return -1;
	}
	return id;
}

int CSem::sem_setval(int pos, int val)
{
	union semun sem;
	sem.val = val;
	int ret = semctl(this->id, pos, SETVAL, sem);
	if (ret < 0)
	{
		perror("sem ctl error ");
	}
	return ret;
}



int  CSem::sem_getval(int pos)
{
	int ret = semctl(this->id, pos, GETVAL);
	if (ret < 0)
	{
		perror("sem ctl error ");
	}
	return ret;
}

int  CSem::sem_p(int pos, unsigned short val)
{
	struct sembuf buf = {pos, -val, 0};
	int ret = semop(this->id, &buf, 1);
	if (ret < 0)
	{
		perror("sem op p error ");
	}
}

int  CSem::sem_v( int pos, unsigned short val)
{
	struct sembuf buf = {pos, val, 0};
	int ret = semop(this->id, &buf, 1);
	if (ret < 0)
	{
		perror("sem op v error ");
	}
}


int  CSem::sem_setall( unsigned short *val)
{
	union semun sem;
	sem.array = val;
	int ret = semctl(this->id, 0, SETALL, sem);
	if (ret < 0)
	{
		perror("sem ctl error ");
	}
	return ret;
}

int  CSem::sem_getall(unsigned short *val)
{
	union semun sem;
	sem.array = val;
	int ret = semctl(this->id, 0, GETALL, sem);
	if (ret < 0)
	{
		perror("sem ctl error ");
	}
	return ret;
}

int  CSem::sem_close()
{
	int ret = semctl(this->id, 0, IPC_RMID);
	if (ret < 0)
	{
		perror("close sem error ");
	}
	return ret;
}





