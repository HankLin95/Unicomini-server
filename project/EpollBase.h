#ifndef _EPOLLBASE_H_
#define _EPOLLBASE_H_

#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

class CEpollBase
{
public:
	CEpollBase(unsigned int maxEvent = 20, int timeout = -1);
	~CEpollBase();
	bool AddEvent(int fd, unsigned int events);
	bool ModEvent(int fd, unsigned int events);
	bool DelEvent(int fd, unsigned int events);
	void SetTimeout(const int timeout);
	int GetTimeout()const;
	int Wait();
	void Start();
	void Stop();
protected:
	struct epoll_event *m_retEvent;
	int m_maxEvent;
private:
	virtual void Run(int nevent) = 0; 
	int m_epfd;
	struct epoll_event m_event;
	int m_timeout;
	bool m_bStop;
};

#endif //_EPOLLBASE_H_