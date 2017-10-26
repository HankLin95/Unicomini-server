#include "EpollSecond.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#define MESSAGE_SHM (key_t)0x1111    //用于存放所有在线用户的地方
#include "public.h"
#include <pthread.h>

#include "sem.h"

extern void* message_share_addr;
extern CSem sem;//创建一个pv信号两个

unsigned long readbufftask::count = 0;

readbufftask::readbufftask()
:CTask()
{
	memset(buff,0,120);
}

readbufftask::~readbufftask()
{}
	
void readbufftask::set_buff(char *buff)
{
	
	memcpy(this->buff,buff,120);
	
}

char* readbufftask::get_buff()
{return this->buff;}


int readbufftask::set_fd(int fd)
{
	this->fd=fd;
	return fd;
}

int readbufftask::run()//往共享内存里面写数据包  （sem（0，0）v一个）
{
	
 	int used_sum=0;

	memcpy(&used_sum,message_share_addr,sizeof(used_sum));
	
	printf("before used_sum = %d\n",used_sum);

	if(used_sum<74)
	{	
		used_sum++;

	    memcpy((char*)message_share_addr+used_sum*130,&this->fd,sizeof(this->fd));
		
		memcpy(  (char*)message_share_addr+used_sum*130+sizeof(this->fd), this->buff, sizeof(this->buff));


		memcpy(message_share_addr,&used_sum,sizeof(used_sum));
	
    	printf("after used_sum = %d\n",used_sum);
		
		sem.sem_v(0,1);//v一个

	}

	return 0;
	

}



CEpollSecond::CEpollSecond()
:CEpollBase(20,-1)
{
	this->lis_fd_sum=0;

    pthread_create(&tid, NULL, handler,this);

	pool.start(3);//开启线程
}




CEpollSecond:: ~CEpollSecond()
{}


void* CEpollSecond::handler(void *arg)
{
	pthread_detach(pthread_self());

	CEpollSecond *thr = (CEpollSecond *)arg;

	thr->Start() ;
}




void CEpollSecond::OnReadData(int fd, char *buf, int bufLen)
{
   
}



void CEpollSecond::OnClientClose(int fd)
{
	close(fd);
	sub_list_fd_sum();//监听的fd的个数减一
}


void CEpollSecond::Run(int nevent)//读取客户端发送过来的数据
{	
    for (int i = 0; i < nevent; i++)
	{ 
		if ((m_retEvent+i)->events & EPOLLIN)
		{
			memset(m_buf[i],0,120);
			
            PACKET_HEAD_T head;
			LOGIN_T log;
            BUYCARD_T buycard;
			CELLULAR_PHONE_T cellphone;
			BALANCE_ENQUIRY_T ballance;
			HISTORYBILL_ENQUIRY_T bill;
			HISTORYLIST_ENQUIRY_T list1;
			
			int rdsize1=0;
            int rdsize2=0;

            rdsize1=read((m_retEvent+i)->data.fd, &head, sizeof(head));
       
			if (rdsize1>0)
			{
				switch(head.type)
				{
				case LOGIN_id:
					{
						  rdsize2=read((m_retEvent+i)->data.fd, &log, sizeof(log));
						  memcpy(m_buf[i],&head,sizeof(head));
						  memcpy(m_buf[i]+sizeof(head),&log,sizeof(log));
					
					}
					break;
                    
				case BUYCARD_id:
					{
						  rdsize2=read((m_retEvent+i)->data.fd, &buycard, sizeof(buycard));
						  memcpy(m_buf[i],&head,sizeof(head));	
						  memcpy(m_buf[i]+sizeof(head),&buycard,sizeof(buycard));
						
					}
					break;

				case CELLULAR_id:
					{
						  rdsize2=read((m_retEvent+i)->data.fd, &cellphone, sizeof(cellphone));
						  memcpy(m_buf[i],&head,sizeof(head));	
						  memcpy(m_buf[i]+sizeof(head),&cellphone,sizeof(cellphone));	
					
					}
					break;

				case BALLANCE_id:
					{
						  rdsize2=read((m_retEvent+i)->data.fd, &ballance, sizeof(ballance));
						  memcpy(m_buf[i],&head,sizeof(head));	
						  memcpy(m_buf[i]+sizeof(head),&ballance,sizeof(ballance));		
					
					}
							break;

				case HISTORYBILL_id:
					{
						  rdsize2=read((m_retEvent+i)->data.fd, &bill, sizeof(bill));
						  memcpy(m_buf[i],&head,sizeof(head));	
						  memcpy(m_buf[i]+sizeof(head),&bill,sizeof(bill));
						
					}
					break;

				case HISTORYLIST_id:
					{
						  rdsize2=read((m_retEvent+i)->data.fd, &list1, sizeof(list1));
						  memcpy(m_buf[i],&head,sizeof(head));	
						  memcpy(m_buf[i]+sizeof(head),&list1,sizeof(list1));
						 
					}
				break;
							
				case HEARTBEAT_id:
					{rdsize2=0;}
					break;

				default:
					{rdsize2=2;}
					break;
				}

        		if(rdsize2>2)
				{

					readbufftask *readbufftask1=new readbufftask;
					readbufftask1->set_buff(m_buf[i]);
					readbufftask1->set_fd((m_retEvent+i)->data.fd);
					pool.addTask(readbufftask1);

				}

			}
				
			if(rdsize1<0)
			{
				OnClientClose((m_retEvent+i)->data.fd);
			}
		

		}
	}
}



int CEpollSecond::get_lis_fd_sum()//返回目前监听的fd的总数
{return this->lis_fd_sum;}

int CEpollSecond::add_list_fd_sum()//目前监听的fd的总数+1
{
  	 this->lis_fd_sum++;
	 return this->lis_fd_sum;
}

int CEpollSecond::sub_list_fd_sum()//目前监听的fd的总数-1
{
	this->lis_fd_sum--;
	return this->lis_fd_sum;
}

//////////////////////////////////////////////////////////////////////////
//还要添加一个线程池工作的任务(线程池对这20个1024的缓冲区进行读取操作。)
