#ifndef _OPERATIONTHREAD_H_
#define _OPERATIONTHREAD_H_


#include "BaseStream.h"
#include "public.h"
#include "ThreadPool.h"
#include "singleton.h"

#include <string.h>


//��ȡ�����ڴ��������
class readshmtask : public CTask
{
public:
	readshmtask();
	~readshmtask();

	void set_fd(int fd );

	void set_head(	PACKET_HEAD_T head);

	void set_log( LOGIN_T log );



	void set_buycard(BUYCARD_T buycard );
	
	void set_cellphone(CELLULAR_PHONE_T cellphone );

	void set_balance(BALANCE_ENQUIRY_T balance );

	void set_bill(HISTORYBILL_ENQUIRY_T bill );

	void set_hislist(HISTORYLIST_ENQUIRY_T hislist );
	
   
	int run();

private:

	int fd;                               //�ļ��ı�ʶ
	PACKET_HEAD_T head;                   //��ͷ
	union message{
		      LOGIN_T log ;                        //����
	  
			  BUYCARD_T buycard;                   //��
			  CELLULAR_PHONE_T cellphone;   //��ֵ
              BALANCE_ENQUIRY_T balance;           //����ѯ
			  HISTORYBILL_ENQUIRY_T bill;          //��ʷ�˵���ѯ
			  HISTORYLIST_ENQUIRY_T hislist;   //��ʷ�굥��ѯ
	}message1;                                     //���������
	
	static unsigned long count;

	float  char_to_float(char *data);//��charת����float

	int  char_to_int(char *data);//��charת����int

    CSingleton *singleton;//���ݿⵥ��

    char* sql;
    char** presult;//���ݿⵥ�������ؽ������
    int rol;//��
    int col;//��


};




///�Թ������ݲ����ĵ���

class COperationThread
{
public:
	COperationThread();
	~COperationThread();
	
	CThreadPool pool;


protected:


private:
};



float  char_to_float(char *data);//��charת����float


#endif


