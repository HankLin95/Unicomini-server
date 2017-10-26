#ifndef _EPOLLSECOND_H_
#define _EPOLLSECOND_H_

#include <pthread.h>

#include "EpollBase.h"
#include "ThreadPool.h"

#include <string.h>

//��ȡ��������������
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
	 int get_lis_fd_sum();//����Ŀǰ������fd������
	 int add_list_fd_sum();//Ŀǰ������fd������+1
     int sub_list_fd_sum();//Ŀǰ������fd������-1



	static void* handler(void *arg);//�����߳�����Ҫ�Ĵ�����

private:
	void OnReadData(int fd, char *buf, int bufLen);

	void OnClientClose(int fd);

	void Run(int nevent);

	char m_buf[20][120];

	CThreadPool pool;

	pthread_t tid;//�߳�id���ڼ�¼����������

	int lis_fd_sum;//Ŀǰ����������

protected:

};

#endif