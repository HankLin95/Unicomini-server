#include "BaseStream.h"
#include "HostAddress.h"
#include "TcpServer.h"
#include "TcpSocket.h"
#include "BaseSocket.h"
#include "EpollSecond.h"
#include "public.h"
#include "OperationThread.h"

#include "sem.h"



#include <string.h>

#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>


#define MESSAGE_SHARE_SHM (key_t)0x1111    //用于存放所有在线用户的地方

#define MESSAGE_WRITE_SHM (key_t)0x1112    //用于存放所有在线用户的地方


void* message_write_addr;//记录客户端发送过来的数据


void* message_share_addr;//记录服务器对数据库操作完之后将信息写回去是所用 的共享 内存


CSem sem;//创建一个pv信号两个



//封装一个函数，用来计算那个epoll里的连接的用户的数量最少。
//计算那个epid_onlineuser[31]={0};里面的用户数量最少//返回最少的下标就可以了
int min_epid_onlineuser(int *P,int length)
{
	int min=P[0];
	int num=0;
	for (int i=0;i<length;i++)
	{
		if (min>P[i])
		{
			min=P[i];
		    num=i;
		}
	}
	return num;
}



//处理函数
void* handler(void *arg)
{
	pthread_detach(pthread_self());
	  
    while (1)
    {
		sem.sem_p(1,1); 

		int write_used=0;	 
		int fd=0; 
		int length=0;
		
		char buff[1024];
	    memset(buff,0,1024);

		memcpy(&write_used, message_write_addr,sizeof( write_used));
      

		if (write_used>0 && write_used<=9)
		{
			  memcpy( &fd, (char*)message_write_addr+write_used*1030,sizeof(fd)); //找到客户端fd
	
              memcpy(&length,(char*)message_write_addr+write_used*1030+sizeof(fd),sizeof(length));//找到要写回客户端的长度
			 
			  memcpy(buff,(char*)message_write_addr+write_used*1030+sizeof(fd)+sizeof(length),length);//找到要写回客户端的数据
			 
			  write(fd,buff,length);//写客户端
			  
              write_used--;

			  memcpy(message_write_addr,&write_used,sizeof(write_used));
			  
		}
    }	

	return NULL;
}






//服务器
int main()
{



	int epid_onlineuser[30]={0};////epid_onlineuser[0]用于存放在线用户数epid_onlineuser[1]~epid_onlineuser[30]用于存放每个epoll的对应连接的用户的个数

	//打开（制定id）的共享内存，如果已经存在，则将它清楚掉，，然后在开启shmid相同的共享内存
	int message_share_shm=shmget(MESSAGE_SHARE_SHM,0,0);
	if (message_share_shm>0)
	{shmctl(MESSAGE_SHARE_SHM,IPC_RMID,NULL);}

	message_share_shm=shmget(MESSAGE_SHARE_SHM,130*75,IPC_CREAT|0766);//第一块用来存放后面所用掉的共享内存的块数。
	if (message_share_shm < 0)
	{
		perror("shmget error");
		return -1;
	}

	//映射地址块
	message_share_addr=shmat(message_share_shm,NULL,0);
	if (message_share_addr==NULL)
	{
		perror("shmat error");
		return -1;
	}
    int temp = 0;
	memcpy(message_share_addr,&temp,sizeof(temp));






	//打开（制定id）的共享内存，如果已经存在，则将它清楚掉，，然后在开启shmid相同的共享内存
	//用于写回客户端的共享内存

	int message_write_shm=shmget(MESSAGE_WRITE_SHM,0,0);
	if (message_write_shm>0)
	{shmctl(MESSAGE_WRITE_SHM,IPC_RMID,NULL);}

	message_write_shm=shmget(MESSAGE_WRITE_SHM,1030*10,IPC_CREAT|0766);//第一块用来存放后面所用掉的共享内存的块数。
	if (message_write_shm < 0)
	{
		perror("shmget error");
		return -1;
	}

	//映射地址块
	message_write_addr=shmat(message_write_shm,NULL,0);
	if (message_write_addr==NULL)
	{
		perror("shmat error");
		return -1;
	}
	temp = 0;
	memcpy(message_write_addr,&temp,sizeof(temp));

  
	CTcpServer server;//创建socket，然后绑定fd
	server.Bind(CHostAddress("0.0.0.0",8888));
 	server.Listen(100);


    //创建一个pv信号两个 并对这两个信号进行赋出值
    sem.sem_create((key_t)0x1235, 2);
    sem.sem_setval(0,0);
    sem.sem_setval(1,0);
	sem.sem_open((key_t)0x1235);
	

	int pid=fork();//创建进程


	while (1)
	{

		if (pid>0)//子进程
		{
		

			 pthread_t m_tid; //创建一个线程用来向客户端发送消息的需要有一个 pv操作（sem（1,0））

			 pthread_create(&m_tid, NULL,handler,NULL);    //创建线程


			 CEpollSecond epollsecond[30];    //创建30个epoll用来监听客户端连接上来之后的

			 int first_epfd=epoll_create(1000); //第一个epoll用来监听socket的fd；
			 if (first_epfd < 0)
			 {
				 perror("epoll create error");
				 exit(-1);
			 }

			//将接客的fd放到first_epfd集合里面里面然后进行监听和接收
			struct epoll_event event;
 			memset(&event, 0, sizeof(event));
			event.events = EPOLLIN;              //设置监听可读事件
			event.data.fd = server.get_m_fd();                  //设置fd
			epoll_ctl(first_epfd, EPOLL_CTL_ADD, server.get_m_fd(), &event);


			struct epoll_event store_event[100]; //用来存放产生事件的集合
			struct sockaddr_in remoteAddr;       //用来记录客户端信息的对象
			socklen_t len = sizeof(remoteAddr);  //用来记录客户端信息的对象的长度
			int ev_fd=0;                         //事件的fd
			int num=0;                           //用于存放那个epid_onlineuser[i]的值最少 的下标号 
			int ncount=0;                        //用以存放监听集合的返回值；（监听到的个数）
			int acid=0;                          //用以存放小妹的acid
			
			while(1)
			{
				memset(&remoteAddr, 0, sizeof(remoteAddr));

				//监听集合
				ncount = epoll_wait(first_epfd, store_event, 100, -1);//1,2,3秒，-1表示永久阻塞
				if (ncount < 0)
				{
					perror("epoll_wait error ");
					exit(-1);
				}
    		    
				if (ncount == 0)
				{
					printf("timeout");
					continue;
				}
				
				for (int i = 0; i < ncount; i++)
				{
					//是否是接客的文件描述符
					if (store_event[i].data.fd == server.get_m_fd())
					{
						//等待，如果有连接，返回一个文件描述符
						int acid = accept(server.get_m_fd(), (struct sockaddr*)&remoteAddr, &len);
						printf("new connection ip:%s port:%d\n",inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));
						//把小妹加入监听集合
				
						for (int j=0;j<30;j++)
						{
							epid_onlineuser[j]=epollsecond[j].get_lis_fd_sum();
						}
               			num=min_epid_onlineuser(epid_onlineuser,30);     //求epid_onlineuser数组里面最小的元素
                
						epollsecond[num].AddEvent(acid,EPOLLIN);
						epollsecond[num].add_list_fd_sum();

					}		
				}	
			}
		}


		if(pid==0)//父进程
		{

           COperationThread operationthread;

			PACKET_HEAD_T head;
			LOGIN_T log;
            BUYCARD_T buycard;
			CELLULAR_PHONE_T cellphone;
			BALANCE_ENQUIRY_T ballance;
			HISTORYBILL_ENQUIRY_T bill;
			HISTORYLIST_ENQUIRY_T list1;
			
			while (1)
			{
				sem.sem_p(0,1);
				int used_sum=0;
	
				memcpy(&used_sum,message_share_addr,sizeof(used_sum));
	
				printf("father used_sum=%d\n",used_sum);

				if (used_sum>0 && used_sum<=74)
				{
					int fd =0;
					memcpy(&fd,(char*)message_share_addr+used_sum*130 ,sizeof(fd));
                    
					printf("father fd= %d\n",fd);

					memcpy(&head,(char*)message_share_addr+used_sum*130+sizeof(fd),sizeof(head));


					switch(head.type) {
					case LOGIN_id:
						{
                            	memcpy(&log,(char*)message_share_addr+used_sum*130+sizeof(fd)+sizeof(head) ,sizeof(log));
								readshmtask *task=new readshmtask;
								task->set_fd(fd);
								task->set_head(head);
								task->set_log(log);
								operationthread.pool.addTask(task);
								used_sum--;
				            	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						}
						break;
                    
					case BUYCARD_id:
						{
							    memcpy(&buycard,(char*)message_share_addr+used_sum*130+sizeof(fd)+sizeof(head) ,sizeof(buycard));
								readshmtask *task=new readshmtask;
								task->set_fd(fd);
								task->set_head(head);
								task->set_buycard(buycard);
								operationthread.pool.addTask(task);
								used_sum--;
				            	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						}
						break;

					case CELLULAR_id:
						{
							    memcpy(&cellphone,(char*)message_share_addr+used_sum*130+sizeof(fd)+sizeof(head) ,sizeof(cellphone));
								readshmtask *task=new readshmtask;
								task->set_fd(fd);
								task->set_head(head);
								task->set_cellphone(cellphone);
								operationthread.pool.addTask(task);
								used_sum--;
				            	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						}
						break;

					case BALLANCE_id:
						{
						    	memcpy(&ballance,(char*)message_share_addr+used_sum*130+sizeof(fd)+sizeof(head) ,sizeof(ballance));
								readshmtask *task=new readshmtask;
								task->set_fd(fd);
								task->set_head(head);
								task->set_balance(ballance);
								operationthread.pool.addTask(task);
								used_sum--;
					            memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						}
						break;

					case HISTORYBILL_id:
						{
							    memcpy(&bill,(char*)message_share_addr+used_sum*130+sizeof(fd)+sizeof(head) ,sizeof(bill));
								readshmtask *task=new readshmtask;
								task->set_fd(fd);
								task->set_head(head);
								task->set_bill(bill);
								operationthread.pool.addTask(task);
								used_sum--;
					            memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						}
						break;

					case HISTORYLIST_id:
						{
							    memcpy(&list1,(char*)message_share_addr+used_sum*130+sizeof(fd)+sizeof(head) ,sizeof(list1));
								readshmtask *task=new readshmtask;
								task->set_fd(fd);
								task->set_head(head);
								task->set_hislist(list1);
								operationthread.pool.addTask(task);
								used_sum--;
				            	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						}
						break;
						
					case HEARTBEAT_id:
						{}
						break;

					default:
						break;

					}
				}
			}
		}
	
		else
		{
			perror("fork error!");
			exit(-1);
		}



	}

 return 0;    
}















/*

	else if(pid==0)//父进程
	{

		 Packet_Head_T head;                   //包头
		 LOGIN_T log ;                        //登入
	     REGISTER_T register1;                //注册
	     BUYCARD_T buycard;                   //买卡
		 CELLULAR_PHONE_T cellphone;          //充值
		 BALANCE_ENQUIRY_T balance;           //余额查询
		 HISTORYBILL_ENQUIRY_T bill;          //历史账单查询
		 HISTORYLIST_ENQUIRY_T hislist;       //历史详单查询
         
		
		 COperationThread operationthread;    //对共享内存操作的类。

		 //operationthread.pool.start(1);        //开启线程池

		while (1)
		{
			memset(&head, 0, sizeof(head));
			memset(&log, 0, sizeof(log));
			memset(&register1, 0, sizeof(register1));
			memset(&buycard, 0, sizeof(buycard));
			memset(&cellphone, 0, sizeof(cellphone));
			memset(&balance, 0, sizeof(balance));
			memset(&bill, 0, sizeof(bill));
			memset(&hislist, 0, sizeof(hislist));


			int fd;
			int used_sum;

			///读的时候要记得加锁；
        	memcpy(&used_sum,message_share_addr,sizeof(used_sum));
            if(used_sum==0)
			{continue;}

			else
			{
			//	pthread_mutex_t m_mutex;

             //  	pthread_mutex_lock(&m_mutex);


			//	printf("main lock ing\n");
				
				char buff[1024];

			     readshmtask *readsharetask=new readshmtask;           //读取共享内存后存放的任务栏

				memcpy(&fd,(char*)message_share_addr+used_sum*sizeof(buff),sizeof(fd));

			
				memcpy( &head,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd),sizeof(head));
				readsharetask->set_fd(fd);
				readsharetask->set_head(head);

				

				switch(head.type)
				{				
                    //心跳功能(4001)
                    case HEARTBEAT_id:  //定时器重新开启
						{operationthread.pool.addTask(readsharetask);}
                    	break;   
                    
					//登入功能(0001) 
					case LOGIN_id:  
						{
							memcpy( &log,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(log) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						
							readsharetask->set_log(log);
                            operationthread.pool.addTask(readsharetask);
						}
                    	break;
                    
					//注册功能(0002)

					case REGISTER_id: 
						{
							memcpy( &register1,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(register1) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
       
							readsharetask->set_register1(register1);
                            operationthread.pool.addTask(readsharetask);
						}
                    	break;
                    
					//买卡功能(1001)  
				    case  BUYCARD_id:  
						{
							memcpy( &buycard,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(buycard) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));

							readsharetask->set_buycard(buycard);
                            operationthread.pool.addTask(readsharetask);
						}
                        break;

				    //充值功能(2001) 
					case CELLULAR_id:  
						{
						    memcpy( &cellphone,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(cellphone) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						
							readsharetask->set_cellphone(cellphone);
                            operationthread.pool.addTask(readsharetask);
						}
                    	break;
                    
					//账单查询功能（本月）(3001)
					case BALLANCE_id:  
						{
						    memcpy( &balance,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(balance) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));

							readsharetask->set_balance(balance);
                            operationthread.pool.addTask(readsharetask);
						}
                    	break;

                     //历史账单查询功能（6+1个月（单月））(3002)
					case HISTORYBILL_id: 
						{
						    memcpy( &bill,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(bill) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
							
							readsharetask->set_bill(bill);
                            operationthread.pool.addTask(readsharetask);
						}
                        break;
					
					//历史详单查询功能（6+1个月（多月）(3003 )
					case HISTORYLIST_id: 
						{
							memcpy( &hislist,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(hislist) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
							
					 		readsharetask->set_hislist(hislist);
                            operationthread.pool.addTask(readsharetask);

						}
                    	break;

                    default:
						break;
				}
				//	printf("main unlock ing\n");
				//	pthread_mutex_unlock(&m_mutex);
			}		
		}
	}
	

	
	else
	{
		perror("fork error!");
		exit(-1);
	}

*/
	
/*	return 0;
}*/










