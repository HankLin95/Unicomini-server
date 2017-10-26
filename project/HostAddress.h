#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>




class CHostAddress
{
public:
	CHostAddress();
	CHostAddress(const char *ip, unsigned short port);
	~CHostAddress();
	void  SetIp(const char *ip);
	const char* GetIp();
	void  SetPort(unsigned short port);
	const unsigned short GetPort();
	struct sockaddr * Address();
	struct sockaddr_in * InAddress();
	int Length();
	socklen_t* LengthPtr();
private:
	struct sockaddr_in m_addr;
	int m_length;
};
