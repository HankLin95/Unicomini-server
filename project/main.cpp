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


#define MESSAGE_SHARE_SHM (key_t)0x1111    //���ڴ�����������û��ĵط�

#define MESSAGE_WRITE_SHM (key_t)0x1112    //���ڴ�����������û��ĵط�


void* message_write_addr;//��¼�ͻ��˷��͹���������


void* message_share_addr;//��¼�����������ݿ������֮����Ϣд��ȥ������ �Ĺ��� �ڴ�


CSem sem;//����һ��pv�ź�����



//��װһ�����������������Ǹ�epoll������ӵ��û����������١�
//�����Ǹ�epid_onlineuser[31]={0};������û���������//�������ٵ��±�Ϳ�����
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



//������
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
			  memcpy( &fd, (char*)message_write_addr+write_used*1030,sizeof(fd)); //�ҵ��ͻ���fd
	
              memcpy(&length,(char*)message_write_addr+write_used*1030+sizeof(fd),sizeof(length));//�ҵ�Ҫд�ؿͻ��˵ĳ���
			 
			  memcpy(buff,(char*)message_write_addr+write_used*1030+sizeof(fd)+sizeof(length),length);//�ҵ�Ҫд�ؿͻ��˵�����
			 
			  write(fd,buff,length);//д�ͻ���
			  
              write_used--;

			  memcpy(message_write_addr,&write_used,sizeof(write_used));
			  
		}
    }	

	return NULL;
}






//������
int main()
{



	int epid_onlineuser[30]={0};////epid_onlineuser[0]���ڴ�������û���epid_onlineuser[1]~epid_onlineuser[30]���ڴ��ÿ��epoll�Ķ�Ӧ���ӵ��û��ĸ���

	//�򿪣��ƶ�id���Ĺ����ڴ棬����Ѿ����ڣ��������������Ȼ���ڿ���shmid��ͬ�Ĺ����ڴ�
	int message_share_shm=shmget(MESSAGE_SHARE_SHM,0,0);
	if (message_share_shm>0)
	{shmctl(MESSAGE_SHARE_SHM,IPC_RMID,NULL);}

	message_share_shm=shmget(MESSAGE_SHARE_SHM,130*75,IPC_CREAT|0766);//��һ��������ź������õ��Ĺ����ڴ�Ŀ�����
	if (message_share_shm < 0)
	{
		perror("shmget error");
		return -1;
	}

	//ӳ���ַ��
	message_share_addr=shmat(message_share_shm,NULL,0);
	if (message_share_addr==NULL)
	{
		perror("shmat error");
		return -1;
	}
    int temp = 0;
	memcpy(message_share_addr,&temp,sizeof(temp));






	//�򿪣��ƶ�id���Ĺ����ڴ棬����Ѿ����ڣ��������������Ȼ���ڿ���shmid��ͬ�Ĺ����ڴ�
	//����д�ؿͻ��˵Ĺ����ڴ�

	int message_write_shm=shmget(MESSAGE_WRITE_SHM,0,0);
	if (message_write_shm>0)
	{shmctl(MESSAGE_WRITE_SHM,IPC_RMID,NULL);}

	message_write_shm=shmget(MESSAGE_WRITE_SHM,1030*10,IPC_CREAT|0766);//��һ��������ź������õ��Ĺ����ڴ�Ŀ�����
	if (message_write_shm < 0)
	{
		perror("shmget error");
		return -1;
	}

	//ӳ���ַ��
	message_write_addr=shmat(message_write_shm,NULL,0);
	if (message_write_addr==NULL)
	{
		perror("shmat error");
		return -1;
	}
	temp = 0;
	memcpy(message_write_addr,&temp,sizeof(temp));

  
	CTcpServer server;//����socket��Ȼ���fd
	server.Bind(CHostAddress("0.0.0.0",8888));
 	server.Listen(100);


    //����һ��pv�ź����� �����������źŽ��и���ֵ
    sem.sem_create((key_t)0x1235, 2);
    sem.sem_setval(0,0);
    sem.sem_setval(1,0);
	sem.sem_open((key_t)0x1235);
	

	int pid=fork();//��������


	while (1)
	{

		if (pid>0)//�ӽ���
		{
		

			 pthread_t m_tid; //����һ���߳�������ͻ��˷�����Ϣ����Ҫ��һ�� pv������sem��1,0����

			 pthread_create(&m_tid, NULL,handler,NULL);    //�����߳�


			 CEpollSecond epollsecond[30];    //����30��epoll���������ͻ�����������֮���

			 int first_epfd=epoll_create(1000); //��һ��epoll��������socket��fd��
			 if (first_epfd < 0)
			 {
				 perror("epoll create error");
				 exit(-1);
			 }

			//���ӿ͵�fd�ŵ�first_epfd������������Ȼ����м����ͽ���
			struct epoll_event event;
 			memset(&event, 0, sizeof(event));
			event.events = EPOLLIN;              //���ü����ɶ��¼�
			event.data.fd = server.get_m_fd();                  //����fd
			epoll_ctl(first_epfd, EPOLL_CTL_ADD, server.get_m_fd(), &event);


			struct epoll_event store_event[100]; //������Ų����¼��ļ���
			struct sockaddr_in remoteAddr;       //������¼�ͻ�����Ϣ�Ķ���
			socklen_t len = sizeof(remoteAddr);  //������¼�ͻ�����Ϣ�Ķ���ĳ���
			int ev_fd=0;                         //�¼���fd
			int num=0;                           //���ڴ���Ǹ�epid_onlineuser[i]��ֵ���� ���±�� 
			int ncount=0;                        //���Դ�ż������ϵķ���ֵ�����������ĸ�����
			int acid=0;                          //���Դ��С�õ�acid
			
			while(1)
			{
				memset(&remoteAddr, 0, sizeof(remoteAddr));

				//��������
				ncount = epoll_wait(first_epfd, store_event, 100, -1);//1,2,3�룬-1��ʾ��������
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
					//�Ƿ��ǽӿ͵��ļ�������
					if (store_event[i].data.fd == server.get_m_fd())
					{
						//�ȴ�����������ӣ�����һ���ļ�������
						int acid = accept(server.get_m_fd(), (struct sockaddr*)&remoteAddr, &len);
						printf("new connection ip:%s port:%d\n",inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));
						//��С�ü����������
				
						for (int j=0;j<30;j++)
						{
							epid_onlineuser[j]=epollsecond[j].get_lis_fd_sum();
						}
               			num=min_epid_onlineuser(epid_onlineuser,30);     //��epid_onlineuser����������С��Ԫ��
                
						epollsecond[num].AddEvent(acid,EPOLLIN);
						epollsecond[num].add_list_fd_sum();

					}		
				}	
			}
		}


		if(pid==0)//������
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

	else if(pid==0)//������
	{

		 Packet_Head_T head;                   //��ͷ
		 LOGIN_T log ;                        //����
	     REGISTER_T register1;                //ע��
	     BUYCARD_T buycard;                   //��
		 CELLULAR_PHONE_T cellphone;          //��ֵ
		 BALANCE_ENQUIRY_T balance;           //����ѯ
		 HISTORYBILL_ENQUIRY_T bill;          //��ʷ�˵���ѯ
		 HISTORYLIST_ENQUIRY_T hislist;       //��ʷ�굥��ѯ
         
		
		 COperationThread operationthread;    //�Թ����ڴ�������ࡣ

		 //operationthread.pool.start(1);        //�����̳߳�

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

			///����ʱ��Ҫ�ǵü�����
        	memcpy(&used_sum,message_share_addr,sizeof(used_sum));
            if(used_sum==0)
			{continue;}

			else
			{
			//	pthread_mutex_t m_mutex;

             //  	pthread_mutex_lock(&m_mutex);


			//	printf("main lock ing\n");
				
				char buff[1024];

			     readshmtask *readsharetask=new readshmtask;           //��ȡ�����ڴ���ŵ�������

				memcpy(&fd,(char*)message_share_addr+used_sum*sizeof(buff),sizeof(fd));

			
				memcpy( &head,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd),sizeof(head));
				readsharetask->set_fd(fd);
				readsharetask->set_head(head);

				

				switch(head.type)
				{				
                    //��������(4001)
                    case HEARTBEAT_id:  //��ʱ�����¿���
						{operationthread.pool.addTask(readsharetask);}
                    	break;   
                    
					//���빦��(0001) 
					case LOGIN_id:  
						{
							memcpy( &log,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(log) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						
							readsharetask->set_log(log);
                            operationthread.pool.addTask(readsharetask);
						}
                    	break;
                    
					//ע�Ṧ��(0002)

					case REGISTER_id: 
						{
							memcpy( &register1,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(register1) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
       
							readsharetask->set_register1(register1);
                            operationthread.pool.addTask(readsharetask);
						}
                    	break;
                    
					//�򿨹���(1001)  
				    case  BUYCARD_id:  
						{
							memcpy( &buycard,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(buycard) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));

							readsharetask->set_buycard(buycard);
                            operationthread.pool.addTask(readsharetask);
						}
                        break;

				    //��ֵ����(2001) 
					case CELLULAR_id:  
						{
						    memcpy( &cellphone,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(cellphone) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
						
							readsharetask->set_cellphone(cellphone);
                            operationthread.pool.addTask(readsharetask);
						}
                    	break;
                    
					//�˵���ѯ���ܣ����£�(3001)
					case BALLANCE_id:  
						{
						    memcpy( &balance,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(balance) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));

							readsharetask->set_balance(balance);
                            operationthread.pool.addTask(readsharetask);
						}
                    	break;

                     //��ʷ�˵���ѯ���ܣ�6+1���£����£���(3002)
					case HISTORYBILL_id: 
						{
						    memcpy( &bill,(char*)message_share_addr+used_sum*sizeof(buff)+sizeof(fd)+sizeof(head), sizeof(bill) );
						    used_sum--;
                        	memcpy(message_share_addr,&used_sum,sizeof(used_sum));
							
							readsharetask->set_bill(bill);
                            operationthread.pool.addTask(readsharetask);
						}
                        break;
					
					//��ʷ�굥��ѯ���ܣ�6+1���£����£�(3003 )
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










