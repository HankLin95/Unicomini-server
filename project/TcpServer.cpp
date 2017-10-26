#include "TcpServer.h"

#include <iostream>
using namespace std;

CTcpServer::CTcpServer()
{
	Create();
}

CTcpServer::~CTcpServer()
{
}

void CTcpServer::Create()
{
	CBaseSocket::Create(PF_INET, SOCK_STREAM, 0);

}

bool CTcpServer::Listen(int backlog)
{
	int ret = listen(m_fd, backlog);
	if (ret < 0)
	{
		perror("listen error ");
	}

	return ret == 0 ? true : false;
}

CTcpSocket CTcpServer::Accept()
{

	CHostAddress remote_addr;
	int fd = accept(m_fd, remote_addr.Address(), remote_addr.LengthPtr());

	CTcpSocket tcpclient(fd);
	tcpclient.SetRemoteAddr(remote_addr);

	return tcpclient;

}


