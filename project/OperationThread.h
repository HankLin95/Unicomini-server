#ifndef _OPERATIONTHREAD_H_
#define _OPERATIONTHREAD_H_


#include "BaseStream.h"
#include "public.h"
#include "ThreadPool.h"
#include "singleton.h"

#include <string.h>


//读取共享内存的任务类
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

	int fd;                               //文件的标识
	PACKET_HEAD_T head;                   //包头
	union message{
		      LOGIN_T log ;                        //登入
	  
			  BUYCARD_T buycard;                   //买卡
			  CELLULAR_PHONE_T cellphone;   //充值
              BALANCE_ENQUIRY_T balance;           //余额查询
			  HISTORYBILL_ENQUIRY_T bill;          //历史账单查询
			  HISTORYLIST_ENQUIRY_T hislist;   //历史详单查询
	}message1;                                     //联合体对象
	
	static unsigned long count;

	float  char_to_float(char *data);//将char转换成float

	int  char_to_int(char *data);//将char转换成int

    CSingleton *singleton;//数据库单例

    char* sql;
    char** presult;//数据库单例（返回结果集）
    int rol;//行
    int col;//列


};




///对共享数据操作的的类

class COperationThread
{
public:
	COperationThread();
	~COperationThread();
	
	CThreadPool pool;


protected:


private:
};



float  char_to_float(char *data);//将char转换成float


#endif


