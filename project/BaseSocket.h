#pragma once


#include "BaseStream.h"
#include "HostAddress.h"


class CBaseSocket
{
public:
	CBaseSocket();
	CBaseSocket(int type, int sin_farmly = PF_INET, int protocol = 0);
	~CBaseSocket();
	int GetSocket() const { return m_fd; }
	void SetSocket(int fd);
	CHostAddress GetAddress();
	bool Bind(CHostAddress addr);
	bool Close();

	int get_m_fd();

protected:
	void Create(int sin_farmly, int flag, int protocl);
	int m_fd;
	CBaseStream m_stream;
};
