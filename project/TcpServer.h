#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_



#include "BaseSocket.h"
#include "BaseStream.h"
#include "HostAddress.h"
#include "TcpSocket.h"




class CTcpServer : public CBaseSocket
{
public:
	CTcpServer();
	~CTcpServer();
	bool Listen(int backlog);
	CTcpSocket Accept();
	

private:
	void Create();
};


#endif 