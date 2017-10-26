#pragma once

#include "BaseSocket.h"
#include "HostAddress.h"

class CTcpSocket :	public CBaseSocket
{
public:
	CTcpSocket(void);
	CTcpSocket(const CTcpSocket &other);
	CTcpSocket(int fd);
	~CTcpSocket(void);
	CTcpSocket & operator= (const CTcpSocket &other);
	CHostAddress GetRemoteAddr() const;
	void SetRemoteAddr(const CHostAddress remote_addr);
	bool Connect(CHostAddress addr);
	int Read(char *buf, const int buf_len);
	int Read(char *buf, const int buf_len, struct timeval *tv);
	int Write(char *buf, const int buf_len);
	int Write(char *buf, const int buf_len, struct timeval *tv);
protected:
	void Create();
	CHostAddress m_remote_addr;
};

