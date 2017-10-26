#include "BaseStream.h"

CBaseStream::CBaseStream()
:m_fd(-1)
{

}

CBaseStream::CBaseStream(int fd)
:m_fd(fd)
{

}

CBaseStream::~CBaseStream(void)
{

}

void CBaseStream::SetFd(int fd)
{
	m_fd = fd;
}

int CBaseStream::GetFd() const
{
	return m_fd;
}

int CBaseStream::Read(char *buf, const int buf_len)
{
	int rdsize = recv(m_fd, buf, buf_len, 0);
	if (rdsize < 0)
	{
		perror("recv data error : ");
	}
	return rdsize;
}

int CBaseStream::Read(char *buf, const int buf_len, struct timeval *tv/*int timeout == -1*/)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(m_fd, &set);
	int ret = select(m_fd+1, &set, NULL, NULL, tv);
	if (ret > 0)
	{
		if (FD_ISSET(m_fd, &set))
		{
			return Read(buf, buf_len);
		}
	}
	else if (ret == 0)
	{
		printf("select timeout\n");
		return 0;
	}
	else
	{
		perror("select error");
		return -1;
	}

}

int CBaseStream::Read(char *buf, const int buf_len, CHostAddress &remote_addr)
{
	int rdsize = recvfrom(m_fd, buf, buf_len, 0, remote_addr.Address(), remote_addr.LengthPtr());

	if (rdsize < 0)
	{
		perror("recvfrom data error : ");
	}
	return rdsize;
}

int CBaseStream::Read(char *buf, const int buf_len, CHostAddress &remote_addr, struct timeval *tv)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(m_fd, &set);

	int ret = select(m_fd+1, &set, NULL, NULL, tv);
	if (ret > 0)
	{
		if (FD_ISSET(m_fd, &set))
		{
			return Read(buf, buf_len, remote_addr);
		}
	}
	else if (ret == 0)
	{
		printf("select timeout\n");
		return 0;
	}
	else
	{
		perror("select error");
		return -1;
	}
}

int CBaseStream::Write(const char *buf, const int buf_len)
{
	int wdsize = send(m_fd, buf, buf_len, 0);
	if (wdsize <= 0)
	{
		perror("send data error : ");
	}
	return wdsize;
}

int CBaseStream::Write(const char *buf, const int buf_len, struct timeval *tv)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(m_fd, &set);

	int ret = select(m_fd+1, NULL, &set, NULL, tv);
	if (ret > 0)
	{
		if (FD_ISSET(m_fd, &set))
		{
			return Write(buf, buf_len);
		}
	}
	else if (ret == 0)
	{
		printf("select timeout\n");
		return 0;
	}
	else
	{
		perror("select error");
		return -1;
	}
}

int CBaseStream::Write(const char *buf, const int buf_len, CHostAddress remote_addr)
{
	int wdsize = sendto(m_fd, buf, buf_len, 0, remote_addr.Address(), remote_addr.Length());
	if (wdsize <= 0)
	{
		perror("sendto data error : ");
	}
	return wdsize;
}

int CBaseStream::Write(const char *buf, const int buf_len, CHostAddress remote_addr, struct timeval *tv)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(m_fd, &set);

	int ret = select(m_fd+1, NULL, &set, NULL, tv);
	if (ret > 0)
	{
		if (FD_ISSET(m_fd, &set))
		{
			return Write(buf, buf_len, remote_addr);
		}
	}
	else if (ret == 0)
	{
		printf("select timeout\n");
		return 0;
	}
	else
	{
		perror("select error");
		return -1;
	}
}

//enum EStatus
//{
//	BeforeLogin;
//	SendLogin;
//	RecvLoginResult;
//};
//
//EStatus stat;
//int timeout = 0;
//
//void * handler(void *arg)
//{
//	while (1)
//	{
//		//接收结果
//		int ret = Read(3s);
//		if (ret == 0 || stat == SendLogin)
//		{
//			
//			timeout++;
//			if (timeout > 3)
//			{
//				//退出
//			}
//			//重新发送
//		}
//		else if (ret > 0)
//		{
//			stat = RecvLoginResult;
//			break;
//		}
//
//
//	}
//}
//
//int main()
//{
//	
//	stat = BeforeLogin;
//	//发送登录包
//
//	stat = SendLogin;
//
//}


//TCP要考虑断线重连
//UDP要考虑超时重发