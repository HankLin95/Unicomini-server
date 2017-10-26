#ifndef _PUBLIC_H_ 
#define _PUBLIC_H_

#include <iostream>
using namespace std;

//包头设计
typedef struct Packet_Head_T
{
	int type;               //类型  （用来记录包体的结构体的类型）
	int number;             //个数  （用来记录包体所紧跟着的结构体对象的个数）
	char trans_flowid[25];  //交易流水；平台生成；格式：YYYYMMDDHHMISS+func_id+五位随机数			//交易流水
	
}PACKET_HEAD_T;
                 
//包体设计
//1.开机登入功能(功能号: 0001)
//请求包
typedef struct Login_T 
{
	char soft_ver[8];   //软件版本号；
	char hard_ver[8];   //硬件版本号
	int dev_id;         //设备ID   用于记录每台客户端设备的标识（在开机登入的业务功能上 会用到）					
	char username[10];  //用户名
	char userpwd[10];    //密码
}LOGIN_T;

//应答包
typedef struct Login_answer_T
{
	int dev_status;  //设备状态，登入是否成功的标志（0:失败，1:成功）  ；
}LOGIN_ANSWER_T;



//2购买电子卡(功能号:1001)
//请求包
typedef struct BuyCard_T {
	int card_maney;   //卡的面值ID，30, 50，100
	int card_count;   // 卡的数量
	int ask_buy;      //请求买卡，确定购买卡（0：请求，1：确定购买）
	                  //（客户端第一次发起请求购买，服务器响应剩余卡的张数，，客户端第二次发起确定购买卡的张数，服务器响应卡数和卡的密码即卡的序列号）
}BUYCARD_T;

//响应包（卡的剩余数量）
typedef struct BuyCard_answer_num_T 
{
//	int ask_buy;            //请求买卡，确定购买卡（0：请求，1：确定购买）
	int card_count;         //卡数量
}BUYCARD_ANSWER_NUM_T;

//响应包（卡的密码）（在包头的结构体个数下，要写上所确定购买的卡的张数，即紧跟着的几张卡的结构体的对象的个数）
typedef struct BuyCard_answer_pwd_T 
{
//	int ask_buy;               //请求买卡，确定购买卡（0：请求，1：确定购买）
	char card_id[10];         //卡的密码（序列号）唯一标识一张卡的 
}BUYCARD_ANSWER_PWD_T;



//手机充值（功能号：2001）
//请求包
typedef struct cellular_phone_T
{
	char tel[14];        //手机号码8613023874749
	char card_id[10];    //卡的密码 （序列号）唯一标识一张卡的 （根据卡的id可以确定卡的金额）
}CELLULAR_PHONE_T;

//响应包，成功与否（如果发生错误则没有该数据域）
typedef struct cellular_phone_answer_T
{
	int issuccess;          //成功与否的标志（0:成功，1:号码不存在  2:交易密码不对,3、已经充值）
}CELLULAR_PHONE_ANSWER_T;


//余额查询（功能号：3001）
//请求包
typedef struct balance_enquiry_T 
{
	char tel[14];   //手机号码8613023874749
}BALANCE_ENQUIRY_T;

//响应包     
typedef struct balance_enquiry_answer_T 
{
	int error_type;           //(0:成功   1:号码不存在)
	float balance;           //余额单位:元
}BALANCE_ENQUIRY_ANSWER_T;



//历史账单查询（功能号：3002）
//请求包
typedef struct historybill_enquiry_T 
{
	char tel[14];          //手机号码8613023874749
	char month[7];        //月份；格式：YYYYMM
}HISTORYBILL_ENQUIRY_T;

//响应包    
typedef struct historybill_enquiry_answer_T 
{
	int error_type;          //(0:成功   1:号码不存在)
	float local_tel;         //本地通话费
	float message1;           //短信息费用
	float gprs;              //GPRS上网费用
	float increment;         //增值费用
	float sp;                //第三方业务
	
}HISTORYBILL_ENQUIRY_ANSWER_T;



//历史详单查询（功能号：3003）
//请求包
typedef struct historylist_enquiry_T
{
	char tel[14];         //手机号码8613023874749
	char month_begin[7];  //月份；格式：YYYYMM
	char month_end[7];    //月份；格式：YYYYMM
}HISTORYLIST_ENQUIRY_T;

//响应包  
typedef struct historylist_enquiry_answer_T 
{
	int error_type;        //(0:成功   1:号码不存在)
	char act_tel[14];      //手机号码8613023874749
	char net_tel[14];      //手机号码8613023874749
	int tel_type;          //通话类型(0:本地通话   1:异地通话)
	int tel_time;          //通话时长(分钟数)
	float tel_money;       //金额
	char remark[21];       //备注
}HISTORYLIST_ENQUIRY_ANSWER_T;


//各种功能id的宏定义
//说明（该功能号用于区别是什么业务，type/func_id  用其中的一个）
//开机登入功能(功能号: 0001)  
// 购买电子卡(功能号:1001)
//手机充值（功能号：2001）  
//余额查询（功能号：3001） 历史账单查询（功能号：3002）历史详单查询（功能号：3003）
//心跳（功能号：4001）

#define LOGIN_id  0001      //登入功能

#define BUYCARD_id 1001     //买卡功能

#define CELLULAR_id 2001    //充值功能

#define BALLANCE_id 3001    //账单查询功能（本月）

#define HISTORYBILL_id 3002 //历史账单查询功能（6+1个月（单月））

#define HISTORYLIST_id 3003  //历史详单查询功能（6+1个月（多月））

#define HEARTBEAT_id 4001    //心跳功能



//各个结构体长度的宏定义（便于数据的读取）
#define PACKETHEAD_LENGTH sizeof(PACKET_HEAD_T)   //包头

#define LOGIN_LENGTH sizeof(LOGIN_T)  //登入请求

#define LOGIN_ANSWER_LENGTH sizeof(LOGIN_ANSWER_T)  //登入响应


#define BUYCARD_LENGTH sizeof(BUYCARD_T)  //买卡请求

#define BUYCARD_ANSWER_NUM_LENGTH sizeof(BUYCARD_ANSWER_NUM_T)  //买卡响应（数量）

#define BUYCARD_ANSWER_PWD_LENGTH sizeof(BUYCARD_ANSWER_PWD_T)   //买卡响应（密码）

#define CELLULARPHONE_LENGTH sizeof(CELLULAR_PHONE_T)    //手机充值（请求）

#define CELLULARPHONE_ANSWER_LENGTH sizeof(CELLULAR_PHONE_ANSWER_T)  //手机充值（响应）

#define BALANCE_LENGTH sizeof(BALANCE_ENQUIRY_T)    //余额查询（请求）

#define BALLANCE_ANSWER_LENGTH sizeof(BALANCE_ENQUIRY_ANSWER_T)  //账单查询（响应）

#define HISTOEYBILL_LENGTH sizeof(HISTORYBILL_ENQUIRY_T)   //历史账单查询（请求）

#define HISTOEYBILL_ANSWER_LENGTH sizeof(HISTORYBILL_ENQUIRY_ANSWER_T)   //历史账单查询（响应）

#define HISTORYLIST_LENGTH sizeof(HISTORYLIST_ENQUIRY_T)   //历史详单查询（请求）

#define HISTORYLIST_ANSWER_LENGTH sizeof(HISTORYLIST_ENQUIRY_ANSWER_T)  //历史详单查询（响应）



#endif