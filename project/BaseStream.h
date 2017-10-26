#pragma once

#include "HostAddress.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

class CBaseStream
{
public:
	CBaseStream();
	CBaseStream(int fd);
	~CBaseStream(void);
	void SetFd(int fd);
	int GetFd() const;
	int Read(char *buf, const int buf_len);
	int Read(char *buf, const int buf_len, struct timeval *tv/*int timeout == -1*/);
	int Read(char *buf, const int buf_len, CHostAddress &remote_addr);
	int Read(char *buf, const int buf_len, CHostAddress &remote_addr, struct timeval *tv);
	int Write(const char *buf, const int buf_len);
	int Write(const char *buf, const int buf_len, struct timeval *tv);
	int Write(const char *buf, const int buf_len, CHostAddress remote_addr);
	int Write(const char *buf, const int buf_len, CHostAddress remote_addr, struct timeval *tv);
private:
	int m_fd;
};
