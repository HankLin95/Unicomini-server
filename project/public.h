#ifndef _PUBLIC_H_ 
#define _PUBLIC_H_

#include <iostream>
using namespace std;

//��ͷ���
typedef struct Packet_Head_T
{
	int type;               //����  ��������¼����Ľṹ������ͣ�
	int number;             //����  ��������¼�����������ŵĽṹ�����ĸ�����
	char trans_flowid[25];  //������ˮ��ƽ̨���ɣ���ʽ��YYYYMMDDHHMISS+func_id+��λ�����			//������ˮ
	
}PACKET_HEAD_T;
                 
//�������
//1.�������빦��(���ܺ�: 0001)
//�����
typedef struct Login_T 
{
	char soft_ver[8];   //����汾�ţ�
	char hard_ver[8];   //Ӳ���汾��
	int dev_id;         //�豸ID   ���ڼ�¼ÿ̨�ͻ����豸�ı�ʶ���ڿ��������ҵ������ ���õ���					
	char username[10];  //�û���
	char userpwd[10];    //����
}LOGIN_T;

//Ӧ���
typedef struct Login_answer_T
{
	int dev_status;  //�豸״̬�������Ƿ�ɹ��ı�־��0:ʧ�ܣ�1:�ɹ���  ��
}LOGIN_ANSWER_T;



//2������ӿ�(���ܺ�:1001)
//�����
typedef struct BuyCard_T {
	int card_maney;   //������ֵID��30, 50��100
	int card_count;   // ��������
	int ask_buy;      //�����򿨣�ȷ�����򿨣�0������1��ȷ������
	                  //���ͻ��˵�һ�η��������򣬷�������Ӧʣ�࿨�����������ͻ��˵ڶ��η���ȷ�����򿨵���������������Ӧ�����Ϳ������뼴�������кţ�
}BUYCARD_T;

//��Ӧ��������ʣ��������
typedef struct BuyCard_answer_num_T 
{
//	int ask_buy;            //�����򿨣�ȷ�����򿨣�0������1��ȷ������
	int card_count;         //������
}BUYCARD_ANSWER_NUM_T;

//��Ӧ�����������룩���ڰ�ͷ�Ľṹ������£�Ҫд����ȷ������Ŀ����������������ŵļ��ſ��Ľṹ��Ķ���ĸ�����
typedef struct BuyCard_answer_pwd_T 
{
//	int ask_buy;               //�����򿨣�ȷ�����򿨣�0������1��ȷ������
	char card_id[10];         //�������루���кţ�Ψһ��ʶһ�ſ��� 
}BUYCARD_ANSWER_PWD_T;



//�ֻ���ֵ�����ܺţ�2001��
//�����
typedef struct cellular_phone_T
{
	char tel[14];        //�ֻ�����8613023874749
	char card_id[10];    //�������� �����кţ�Ψһ��ʶһ�ſ��� �����ݿ���id����ȷ�����Ľ�
}CELLULAR_PHONE_T;

//��Ӧ�����ɹ�����������������û�и�������
typedef struct cellular_phone_answer_T
{
	int issuccess;          //�ɹ����ı�־��0:�ɹ���1:���벻����  2:�������벻��,3���Ѿ���ֵ��
}CELLULAR_PHONE_ANSWER_T;


//����ѯ�����ܺţ�3001��
//�����
typedef struct balance_enquiry_T 
{
	char tel[14];   //�ֻ�����8613023874749
}BALANCE_ENQUIRY_T;

//��Ӧ��     
typedef struct balance_enquiry_answer_T 
{
	int error_type;           //(0:�ɹ�   1:���벻����)
	float balance;           //��λ:Ԫ
}BALANCE_ENQUIRY_ANSWER_T;



//��ʷ�˵���ѯ�����ܺţ�3002��
//�����
typedef struct historybill_enquiry_T 
{
	char tel[14];          //�ֻ�����8613023874749
	char month[7];        //�·ݣ���ʽ��YYYYMM
}HISTORYBILL_ENQUIRY_T;

//��Ӧ��    
typedef struct historybill_enquiry_answer_T 
{
	int error_type;          //(0:�ɹ�   1:���벻����)
	float local_tel;         //����ͨ����
	float message1;           //����Ϣ����
	float gprs;              //GPRS��������
	float increment;         //��ֵ����
	float sp;                //������ҵ��
	
}HISTORYBILL_ENQUIRY_ANSWER_T;



//��ʷ�굥��ѯ�����ܺţ�3003��
//�����
typedef struct historylist_enquiry_T
{
	char tel[14];         //�ֻ�����8613023874749
	char month_begin[7];  //�·ݣ���ʽ��YYYYMM
	char month_end[7];    //�·ݣ���ʽ��YYYYMM
}HISTORYLIST_ENQUIRY_T;

//��Ӧ��  
typedef struct historylist_enquiry_answer_T 
{
	int error_type;        //(0:�ɹ�   1:���벻����)
	char act_tel[14];      //�ֻ�����8613023874749
	char net_tel[14];      //�ֻ�����8613023874749
	int tel_type;          //ͨ������(0:����ͨ��   1:���ͨ��)
	int tel_time;          //ͨ��ʱ��(������)
	float tel_money;       //���
	char remark[21];       //��ע
}HISTORYLIST_ENQUIRY_ANSWER_T;


//���ֹ���id�ĺ궨��
//˵�����ù��ܺ�����������ʲôҵ��type/func_id  �����е�һ����
//�������빦��(���ܺ�: 0001)  
// ������ӿ�(���ܺ�:1001)
//�ֻ���ֵ�����ܺţ�2001��  
//����ѯ�����ܺţ�3001�� ��ʷ�˵���ѯ�����ܺţ�3002����ʷ�굥��ѯ�����ܺţ�3003��
//���������ܺţ�4001��

#define LOGIN_id  0001      //���빦��

#define BUYCARD_id 1001     //�򿨹���

#define CELLULAR_id 2001    //��ֵ����

#define BALLANCE_id 3001    //�˵���ѯ���ܣ����£�

#define HISTORYBILL_id 3002 //��ʷ�˵���ѯ���ܣ�6+1���£����£���

#define HISTORYLIST_id 3003  //��ʷ�굥��ѯ���ܣ�6+1���£����£���

#define HEARTBEAT_id 4001    //��������



//�����ṹ�峤�ȵĺ궨�壨�������ݵĶ�ȡ��
#define PACKETHEAD_LENGTH sizeof(PACKET_HEAD_T)   //��ͷ

#define LOGIN_LENGTH sizeof(LOGIN_T)  //��������

#define LOGIN_ANSWER_LENGTH sizeof(LOGIN_ANSWER_T)  //������Ӧ


#define BUYCARD_LENGTH sizeof(BUYCARD_T)  //������

#define BUYCARD_ANSWER_NUM_LENGTH sizeof(BUYCARD_ANSWER_NUM_T)  //����Ӧ��������

#define BUYCARD_ANSWER_PWD_LENGTH sizeof(BUYCARD_ANSWER_PWD_T)   //����Ӧ�����룩

#define CELLULARPHONE_LENGTH sizeof(CELLULAR_PHONE_T)    //�ֻ���ֵ������

#define CELLULARPHONE_ANSWER_LENGTH sizeof(CELLULAR_PHONE_ANSWER_T)  //�ֻ���ֵ����Ӧ��

#define BALANCE_LENGTH sizeof(BALANCE_ENQUIRY_T)    //����ѯ������

#define BALLANCE_ANSWER_LENGTH sizeof(BALANCE_ENQUIRY_ANSWER_T)  //�˵���ѯ����Ӧ��

#define HISTOEYBILL_LENGTH sizeof(HISTORYBILL_ENQUIRY_T)   //��ʷ�˵���ѯ������

#define HISTOEYBILL_ANSWER_LENGTH sizeof(HISTORYBILL_ENQUIRY_ANSWER_T)   //��ʷ�˵���ѯ����Ӧ��

#define HISTORYLIST_LENGTH sizeof(HISTORYLIST_ENQUIRY_T)   //��ʷ�굥��ѯ������

#define HISTORYLIST_ANSWER_LENGTH sizeof(HISTORYLIST_ENQUIRY_ANSWER_T)  //��ʷ�굥��ѯ����Ӧ��



#endif