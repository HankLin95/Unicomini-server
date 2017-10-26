#include "EpollBase.h"

CEpollBase::CEpollBase(unsigned int maxEvent/* = 20*/, int timeout/* = -1*/)
:m_bStop(false),
m_retEvent(NULL),
m_maxEvent(maxEvent),
m_timeout(timeout)
{
	memset(&m_event, 0, sizeof(m_event));

	m_retEvent = new struct epoll_event[m_maxEvent];

	m_epfd = epoll_create(1000);
	if (m_epfd < 0)
	{
		perror("epoll create error");
	}

}

CEpollBase::~CEpollBase()
{
	close(m_epfd);
	if (m_retEvent != NULL)
	{
		delete[] m_retEvent;
	}

}

bool CEpollBase::AddEvent(int fd, unsigned int events)
{
	m_event.events  = events;
	m_event.data.fd = fd;
	int ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &m_event);
	if (ret < 0)
	{
		perror("epoll add event error");
	}
	return ret == 0 ? true : false;
}

bool CEpollBase::ModEvent(int fd, unsigned int events)
{
	m_event.events  = events;
	m_event.data.fd = fd;
	int ret = epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &m_event);
	if (ret < 0)
	{
		perror("epoll add event error");
	}
	return ret == 0 ? true : false;
}

bool CEpollBase::DelEvent(int fd, unsigned int events)
{
	m_event.events  = events;
	m_event.data.fd = fd;
	int ret = epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, &m_event);
	if (ret < 0)
	{
		perror("epoll add event error");
	}
	return ret == 0 ? true : false;
}

void CEpollBase::SetTimeout(const int timeout)
{
	m_timeout = timeout;
}

int CEpollBase::GetTimeout()const
{
	return m_timeout;
}

int CEpollBase::Wait()
{
	//printf("wait...\n");
	int nevent=0;
	
	nevent = epoll_wait(m_epfd, m_retEvent, m_maxEvent, m_timeout);
	if (nevent < 0)
	{
		perror("epoll wait error");
		exit(-1);
	}
	else if (nevent ==0)
	{
		perror("timeout");
	}
	return nevent;
}

void CEpollBase::Start()
{
	int i=0;
	while (!m_bStop)
	{
		int nevent = Wait();	
		Run(nevent);
	}
}

void CEpollBase::Stop()
{
	m_bStop = true;
}