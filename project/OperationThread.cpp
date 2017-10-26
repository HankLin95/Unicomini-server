#include "OperationThread.h"

#include "public.h"
#include "sem.h"

extern void* message_write_addr;

extern CSem sem;//����һ��pv�ź�����

//��ȡ��������������
unsigned long readshmtask::count = 0;
readshmtask::readshmtask()
:CTask()
{
	
   this->singleton=CSingleton::GetInstance();//���ݿⵥ��
   sql = new char[512];
}
readshmtask::~readshmtask()
{}
   

//���ö�ȡ���ļ�
void readshmtask::set_fd(int fd )
{this->fd=fd;}

//���ð�ͷ
void readshmtask::set_head(	PACKET_HEAD_T head)
{this->head=head;}

//���ð���
void readshmtask::set_log( LOGIN_T log )
{this->message1.log =log;}


void readshmtask::set_buycard(BUYCARD_T buycard )
{this->message1.buycard =buycard;}
	
void readshmtask::set_cellphone(CELLULAR_PHONE_T cellphone )
{this->message1.cellphone =cellphone;}

void readshmtask::set_balance(BALANCE_ENQUIRY_T balance )
{this->message1.balance =balance;}

void readshmtask::set_bill(HISTORYBILL_ENQUIRY_T bill )
{this->message1.bill =bill;}

void readshmtask::set_hislist(HISTORYLIST_ENQUIRY_T hislist )
{this->message1.hislist =hislist;}

/////������ĸ��ֲ�����	//���ݰ�ͷ����Ȼ������Ӧ�Ĳ�����
int readshmtask::run()
{
	char buff[1024];
	memset(buff,0,1024);

    switch(this->head.type) {
   	case LOGIN_id:
	{
			
		    int length =0;//���ڼ�¼д�ؿͻ��˵ĳ���

			int used_sum=0;//���ڼ�¼���ù����ڴ�Ŀ���
			
            memcpy(&used_sum,message_write_addr,sizeof(used_sum));


            if (used_sum<9)
            {
				used_sum++;	

				sprintf(this->sql,"select * from tbl_user where username=%s and userpwd=%s",this->message1.log.username,this->message1.log.userpwd );
				this->singleton->get_data(this->sql,this->presult,this->rol,this->col);
                LOGIN_ANSWER_T ret_log;
				
				if (this->rol==1)
				{ret_log.dev_status=1;}
				else
				{ret_log.dev_status=0;}
	
				memcpy(buff,&this->head,sizeof(this->head));
				memcpy(buff+sizeof(this->head),&ret_log,sizeof(ret_log));
				length=sizeof(this->head)+sizeof(ret_log);

                memcpy((char*)message_write_addr+used_sum*1030,&this->fd,sizeof(this->fd));
				memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd),&length,sizeof(length));
				
                memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd)+sizeof(length), buff, length);
		    	
				memcpy(message_write_addr,&used_sum,sizeof(used_sum));
				sem.sem_v(1,1);
            }
		
	}
	break;
                    
	case BUYCARD_id:
	{

		int length =0;//���ڼ�¼д�ؿͻ��˵ĳ���

		int used_sum=0;//���ڼ�¼���ù����ڴ�Ŀ���
			
        memcpy(&used_sum,message_write_addr,sizeof(used_sum));
        if (used_sum<9)
        {
			used_sum++;

			if (this->message1.buycard.ask_buy==0)//���������
			{
				sprintf(this->sql,"select * from tbl_card where card_money=%d and card_state=0",this->message1.buycard.card_maney);
			    this->singleton->get_data(this->sql,this->presult,this->rol,this->col);

                BUYCARD_ANSWER_NUM_T ret_num;
				if (this->rol < this->message1.buycard.card_count)
				{ret_num.card_count=this->rol;}
				else
				{ret_num.card_count=this->message1.buycard.card_count;}
                
				memcpy(buff,&this->head,sizeof(this->head));
				memcpy(buff+sizeof(this->head),&ret_num,sizeof(ret_num));
				length=sizeof(this->head)+sizeof(ret_num);

                memcpy((char*)message_write_addr+used_sum*1030,&this->fd,sizeof(this->fd));
				memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd),&length,sizeof(length));
				
                memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd)+sizeof(length), buff, length);
		    	
				memcpy(message_write_addr,&used_sum,sizeof(used_sum));
				sem.sem_v(1,1);
				
			}
		

			if (this->message1.buycard.ask_buy==1)//ȷ���������
			{
				this->head.number=this->message1.buycard.card_count;
				
				memcpy(buff,&this->head,sizeof(this->head));
				length=sizeof(this->head);
				
				sprintf(this->sql,"select card_id from tbl_card where card_money=%d and card_state=0",this->message1.buycard.card_maney);
			    this->singleton->get_data(this->sql,this->presult,this->rol,this->col);

                
				BUYCARD_ANSWER_PWD_T ret_pwd;
                
				char** presult11;//���ݿⵥ�������ؽ������

				for (int i=0;i<this->message1.buycard.card_count;i++)
				{
	                strcpy(ret_pwd.card_id,this->presult[i+1]);
                    memcpy(buff+length, &ret_pwd, sizeof(ret_pwd));
					length+=sizeof(ret_pwd);

					sprintf(this->sql,"update tbl_card set card_state=1, trans_flowid=%s where card_id=%s",this->head.trans_flowid,this->presult[i+1]);
			        this->singleton->get_data(this->sql,presult11,this->rol,this->col);
				}

                memcpy((char*)message_write_addr+used_sum*1030,&this->fd,sizeof(this->fd));
				memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd),&length,sizeof(length));
				
                memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd)+sizeof(length), buff, length);
		    	
				memcpy(message_write_addr,&used_sum,sizeof(used_sum));
				sem.sem_v(1,1);
				
			}


		}
	}
	break;

	case CELLULAR_id:
	{
		int length =0;//���ڼ�¼д�ؿͻ��˵ĳ���

		int used_sum=0;//���ڼ�¼���ù����ڴ�Ŀ���
			
        memcpy(&used_sum,message_write_addr,sizeof(used_sum));


        if (used_sum<9)
        {
			used_sum++;

			CELLULAR_PHONE_ANSWER_T reg_cell;

            sprintf(this->sql,"select card_state,card_money from tbl_card where card_id=%s",this->message1.cellphone.card_id);
			this->singleton->get_data(this->sql,this->presult,this->rol,this->col);
            
			if (this->rol==0)
			{reg_cell.issuccess=2;}//�������벻��

			if (this->rol==1)
			{
				int state;
				state=this->char_to_int(this->presult[2]);
				if (state==0)
				{reg_cell.issuccess=2;}//�������벻��

				if (state==2)
				{ reg_cell.issuccess=3;}//�Ѿ���ֵ��

				if (state==1)//δ��ֵ
				{
					////////////////////�޸�2016/07/13
					int card_money=this->char_to_int(this->presult[3]);//��ס���Ľ��
                    ////////////////////

					sprintf(this->sql,"select balance from tbl_balance where tel=%s",this->message1.cellphone.tel);
					this->singleton->get_data(this->sql,this->presult,this->rol,this->col);

					
					if (this->rol==1)//�������
					{
						float old_momey=this->char_to_float(this->presult[1]); //��ס��ʷ���

	                    sprintf(this->sql,"update tbl_balance set balance=%f where tel=%s", old_momey+card_money,this->message1.cellphone.tel);
						this->singleton->get_data(this->sql,this->presult,this->rol,this->col);//��������

						
						sprintf(this->sql,"update tbl_card set card_state=2 where card_id=%s",this->message1.cellphone.card_id);
						this->singleton->get_data(this->sql,this->presult,this->rol,this->col);//�����򿨱�

                       // sprintf(this->sql,"insrt into tbl_charge values(%s,%s,%day,"1001",%s)",this->message1.cellphone.tel,this->message1.cellphone.card_id,"2016/06/09",this->head.trans_flowid);
						//this->singleton->get_data(this->sql,this->presult,this->rol,this->col);//����ֵ���������
                        
						reg_cell.issuccess=0;//��ֵ�ɹ�
					}
					
					else
					{reg_cell.issuccess=1;}//���벻����
				}
				
			}
			

			memcpy(buff,&this->head,sizeof(this->head));
			memcpy(buff+sizeof(this->head),&reg_cell,sizeof(reg_cell));
			length=sizeof(this->head)+sizeof(reg_cell);

            memcpy((char*)message_write_addr+used_sum*1030,&this->fd,sizeof(this->fd));
			memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd),&length,sizeof(length));
				
            memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd)+sizeof(length), buff, length);
		    	
	    	memcpy(message_write_addr,&used_sum,sizeof(used_sum));
			sem.sem_v(1,1);
				
		}
							
	}
	break;

	case BALLANCE_id:
	{
		int length =0;//���ڼ�¼д�ؿͻ��˵ĳ���

		int used_sum=0;//���ڼ�¼���ù����ڴ�Ŀ���
			
        memcpy(&used_sum,message_write_addr,sizeof(used_sum));


        if (used_sum<9)
        {
			used_sum++;
			
			BALANCE_ENQUIRY_ANSWER_T ret_balance;

			sprintf(this->sql,"select balance from tbl_balance where tel=%s",this->message1.balance.tel);
			this->singleton->get_data(this->sql,this->presult,this->rol,this->col);//
			if (this->rol==1)
			{
				ret_balance.error_type=0;//�ɹ�
				ret_balance.balance=this->char_to_float(this->presult[1]);
			}
            
			else
			{ret_balance.error_type=1;}//���벻����
			
			memcpy(buff,&this->head,sizeof(this->head));
			memcpy(buff+sizeof(this->head),&ret_balance,sizeof(ret_balance));
			length=sizeof(this->head)+sizeof(ret_balance);

            memcpy((char*)message_write_addr+used_sum*1030,&this->fd,sizeof(this->fd));
			memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd),&length,sizeof(length));
				
            memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd)+sizeof(length), buff, length);
		    	
	    	memcpy(message_write_addr,&used_sum,sizeof(used_sum));
			sem.sem_v(1,1);
				
		}
						    	
	}
	break;



	case HISTORYBILL_id:
	{
		int length =0;//���ڼ�¼д�ؿͻ��˵ĳ���

		int used_sum=0;//���ڼ�¼���ù����ڴ�Ŀ���
			
        memcpy(&used_sum,message_write_addr,sizeof(used_sum));


        if (used_sum<9)
        {
			used_sum++;

			HISTORYBILL_ENQUIRY_ANSWER_T ret_bill;
             
			sprintf(this->sql,"select * from tbl_bill where tel=%s and bill_month=%s",this->message1.bill.tel,this->message1.bill.month);
			this->singleton->get_data(this->sql,this->presult,this->rol,this->col);//
           if (this->rol==1)
           {
			   ret_bill.local_tel=this->char_to_float(this->presult[9]);
			   ret_bill.message1 =this->char_to_float(this->presult[10]);
			   ret_bill.gprs=this->char_to_float(this->presult[11]);
			   ret_bill.increment=this->char_to_float(this->presult[12]);		   
			   ret_bill.sp=this->char_to_float(this->presult[13]);
			   
			   //////////////////2016/07/24�޸�
               ret_bill.error_type=0;
			   ///////////////////////
           }
		   else
		   {ret_bill.error_type=1;}

            /////(2016/07/10�޸�)
			this->head.number=1;
			//////

			memcpy(buff,&this->head,sizeof(this->head));
			memcpy(buff+sizeof(this->head),&ret_bill,sizeof(ret_bill));
			length=sizeof(this->head)+sizeof(ret_bill);

            memcpy((char*)message_write_addr+used_sum*1030,&this->fd,sizeof(this->fd));
			memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd),&length,sizeof(length));
				
            memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd)+sizeof(length), buff, length);
		    	
	    	memcpy(message_write_addr,&used_sum,sizeof(used_sum));
			sem.sem_v(1,1);
				
		}
							  
	}
	break;

	
	//��ʷ�굥��ѯ

	case HISTORYLIST_id:
	{
		
		int length =0;//���ڼ�¼д�ؿͻ��˵ĳ���

		int used_sum=0;//���ڼ�¼���ù����ڴ�Ŀ���
			
        memcpy(&used_sum,message_write_addr,sizeof(used_sum));

        if (used_sum<9)
        {
			used_sum++;

			HISTORYLIST_ENQUIRY_ANSWER_T ret_list;

            sprintf(this->sql,"select * from tbl_detail where (act_tel=%s or net_tel=%s) and (bill_month between %s and %s)",this->message1.hislist.tel,this->message1.hislist.tel,this->message1.hislist.month_begin,this->message1.hislist.month_end );
			this->singleton->get_data(this->sql,this->presult,this->rol,this->col);
           

			if (this->rol>0)//�������
			{
				this->head.number=this->rol;

				memcpy(buff,&this->head,sizeof(this->head));
				length=sizeof(this->head);
                 
				for (int i=0;i<this->rol;i++)
				{
					ret_list.error_type=0;
					strcpy(ret_list.act_tel,this->presult[7*i+8]);
					
					strcpy(ret_list.net_tel,this->presult[7*i+9]);

					ret_list.tel_type=this->char_to_int(this->presult[7*i+10]);
					
					ret_list.tel_time=this->char_to_int(this->presult[7*i+11]);
					
					ret_list.tel_money=this->char_to_float(this->presult[7*i+12]);

					strcpy(ret_list.remark,this->presult[7*i+13]);
                    
					memcpy(buff+length,&ret_list,sizeof(ret_list));
				    length+=sizeof(ret_list);
				}

			}

			else
			{	
				
				//////////////////////////2016/07/15���
				this->head.number=1;

				memcpy(buff,&this->head,sizeof(this->head));
				length=sizeof(this->head);
				ret_list.error_type=1;//���벻����
				
				memcpy(buff+length,&ret_list,sizeof(ret_list));
				length+=sizeof(ret_list);
               ///////////////////////////
			}

            memcpy((char*)message_write_addr+used_sum*1030,&this->fd,sizeof(this->fd));
			memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd),&length,sizeof(length));
				
            memcpy((char*)message_write_addr+used_sum*1030+sizeof(this->fd)+sizeof(length), buff, length);
		    	
	    	memcpy(message_write_addr,&used_sum,sizeof(used_sum));
			sem.sem_v(1,1);
				
		}					
							 
	}
	break;

	default:
		break;

    }

return 0;
}





COperationThread::COperationThread()
{this->pool.start(30);}

COperationThread::~COperationThread()
{}





float  readshmtask::char_to_float(char *data)//��charת����float
{
	int len;
	len=strlen(data);
	int node_pos=0;
	for (int i=0;i<len;i++)
	{
		if (data[i]!='.')
		{continue;}
		if (data[i]=='.')
		{
			node_pos=i;
			break;
		}
	}

   float pre=0;//���С����ǰ�����ֵ

   float next=0;//С����������ֵ

   float sum=0;

   for (int i=0;i<node_pos;i++)
   {
	   pre=pre*10+data[i]-'0';
   }

   for (int i=len-1;i>node_pos;i--)
   {
      next=next*0.1+data[i]-'0';
   }

   sum=pre+next*0.1;
   
   return sum;

}


int  readshmtask::char_to_int(char *data)//��charת����int
{
	int len;
	len=strlen(data);
	int sum=0;
	for (int i=0;i<len;i++)
	{sum=sum*10+data[i]-'0';}
	return sum;
}
