#include "TcpSocket.h"

CTcpSocket::CTcpSocket(void)
{
	Create();
}

CTcpSocket::~CTcpSocket(void)
{
}

CTcpSocket::CTcpSocket(const CTcpSocket &other)
{
	SetSocket(other.GetSocket());
	SetRemoteAddr(other.GetRemoteAddr());
}

CTcpSocket & CTcpSocket::operator =(const CTcpSocket &other)
{
	SetSocket(other.GetSocket());
	SetRemoteAddr(other.GetRemoteAddr());
}

CTcpSocket::CTcpSocket(int fd)
{
	CBaseSocket::SetSocket(fd);
}

void CTcpSocket::Create()
{
	CBaseSocket::Create(PF_INET, SOCK_STREAM, 0);
}

CHostAddress CTcpSocket::GetRemoteAddr() const
{
	return m_remote_addr;
}

void CTcpSocket::SetRemoteAddr(const CHostAddress remote_addr)
{
	m_remote_addr = remote_addr;
}

bool CTcpSocket::Connect(CHostAddress hostaddr)
{
	int ret = connect(m_fd, hostaddr.Address(), hostaddr.Length());

	if(ret == 0)
		SetRemoteAddr(hostaddr);
	else
		perror("connect to server error : ");

	return ret == 0 ? true : false;
}

int CTcpSocket::Read(char *buf, const int buf_len)
{
	return m_stream.Read(buf, buf_len);
}

int CTcpSocket::Read(char *buf, const int buf_len, struct timeval *tv)
{
	return m_stream.Read(buf, buf_len, tv);
}

int CTcpSocket::Write(char *buf, const int buf_len)
{
	return m_stream.Write(buf, buf_len);
}

int CTcpSocket::Write(char *buf, const int buf_len, struct timeval *tv)
{
	return m_stream.Write(buf, buf_len, tv);
}
