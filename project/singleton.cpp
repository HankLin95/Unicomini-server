#include "singleton.h"

//�Ծ�̬��Ա�ĳ�ʼ��
CSingleton* CSingleton::m_pInstance=NULL;
xugou xugou::xugou1;

/*****************************************/
/*      �����Ĺ��캯��  */
/******************************************/
CSingleton::CSingleton()
 {
	int iresult;	
	iresult=sqlite3_open("project.db",&(this->db));            //�����ݿ�
	if(iresult!=SQLITE_OK)
	{
		printf(" %d %s",sqlite3_errcode(db) , sqlite3_errmsg(db));
		//exit(0);
	}
	else
	{ printf("open the sqlite3\n");}   
}



/*****************************************/
/* ���ص�����ָ�� */
/*********************************************/
CSingleton* CSingleton::GetInstance()
{
	if(m_pInstance==NULL)   //�ж��Ƿ��һ�ε���
	{m_pInstance=new CSingleton();} 
	return m_pInstance;
}

/*************************************************/
/*   ���ݿ�����ӿڣ������ݿ��������ɾ���ġ��飩*/
/**************************************************/
void CSingleton::get_data(char* sql,char**(&presult),int &rol,int &col)
{	
	char *errmsg;
	int result;
	result=sqlite3_get_table(this->db,sql,&presult,&rol,&col,&errmsg);
    if(result!=0)
    {
        printf("%s\n",sqlite3_errmsg(db));
    }
}

//��Ե��Ĺ��캯��
xugou::xugou()
{}

xugou::~xugou()
{
	if(CSingleton::m_pInstance)
	{
		int iresult;
		iresult=sqlite3_close( CSingleton::m_pInstance->db);

        if(iresult==SQLITE_OK)  
		{printf("�ر����ݿ�\n");}
		else
		{printf("���ݿ�ر�ʧ��\n");}
		
		delete CSingleton::m_pInstance;
	    printf("�ͷŵ���\n");
	}
	
}



