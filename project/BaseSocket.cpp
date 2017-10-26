#include "BaseSocket.h"

CBaseSocket::CBaseSocket()
:m_fd(-1)
{
	m_stream.SetFd(m_fd);
}

CBaseSocket::CBaseSocket(int type, int sin_farmly/* = AF_INET*/, int protocol/* = 0*/)
{
	Create(sin_farmly, type, protocol);
}

CBaseSocket::~CBaseSocket()
{
	//不要调用close
}

void CBaseSocket::Create(int sin_farmly, int flag, int protocl)
{
	m_fd = socket(sin_farmly, flag, protocl);
	if (m_fd < 0)
	{
		perror("create socket error : ");
	}

	m_stream.SetFd(m_fd);

}

void CBaseSocket::SetSocket(int fd)
{
	if (m_fd > 0)
	{
		printf("[CBaseSocket Warning] old fd %d will changed to %d\n", m_fd, fd);
	}

	if (fd < 0)
	{
		printf("[CBaseSocket Warning] set fd %d\n", fd);
	}
	m_fd = fd;
	m_stream.SetFd(m_fd);
}

CHostAddress CBaseSocket::GetAddress()
{
	CHostAddress addr;
	memset(&addr, 0, sizeof(addr));
	if( getsockname(m_fd, addr.Address(), addr.LengthPtr()) != 0)
	{
		perror("getsockname error : ");
	}
	
	return addr;
}

bool CBaseSocket::Bind(CHostAddress hostaddr)
{
	int ret = bind(m_fd, hostaddr.Address(), hostaddr.Length());
	if (ret < 0)
	{
		perror("bind error");

		exit(-1);
	}
	return ret == 0 ? true : false;
}

bool CBaseSocket::Close()
{
	if (m_fd < 0)
	{
		printf("[CBaseSocket Warning] close socket fd %d\n",m_fd);
		return false;
	}

	int ret = close(m_fd);

	return ret == 0 ? true : false;
}


int CBaseSocket::get_m_fd()
{
	return this->m_fd;
}
