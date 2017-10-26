#ifndef _EPOLLSECOND_H_
#define _EPOLLSECOND_H_

#include <pthread.h>

#include "EpollBase.h"
#include "ThreadPool.h"

#include <string.h>

//读取缓冲区的任务类
class readbufftask : public CTask
{
public:
	readbufftask();
	~readbufftask();
   
	void set_buff(char *buff);
    char* get_buff();
    int set_fd(int fd);
	int run();



private:
	
	int fd;
	char buff[120];	
	
	static unsigned long count;
};







class CEpollSecond:public CEpollBase
{
public:
     CEpollSecond();
	 ~CEpollSecond();
	 int get_lis_fd_sum();//返回目前监听的fd的总数
	 int add_list_fd_sum();//目前监听的fd的总数+1
     int sub_list_fd_sum();//目前监听的fd的总数-1



	static void* handler(void *arg);//创建线程所需要的处理函数

private:
	void OnReadData(int fd, char *buf, int bufLen);

	void OnClientClose(int fd);

	void Run(int nevent);

	char m_buf[20][120];

	CThreadPool pool;

	pthread_t tid;//线程id用于记录创建监听的

	int lis_fd_sum;//目前监听的总数

protected:

};

#endif