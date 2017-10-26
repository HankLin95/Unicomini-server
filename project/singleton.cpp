#include "singleton.h"

//对静态成员的初始化
CSingleton* CSingleton::m_pInstance=NULL;
xugou xugou::xugou1;

/*****************************************/
/*      单例的构造函数  */
/******************************************/
CSingleton::CSingleton()
 {
	int iresult;	
	iresult=sqlite3_open("project.db",&(this->db));            //打开数据库
	if(iresult!=SQLITE_OK)
	{
		printf(" %d %s",sqlite3_errcode(db) , sqlite3_errmsg(db));
		//exit(0);
	}
	else
	{ printf("open the sqlite3\n");}   
}



/*****************************************/
/* 返回单例的指针 */
/*********************************************/
CSingleton* CSingleton::GetInstance()
{
	if(m_pInstance==NULL)   //判断是否第一次调用
	{m_pInstance=new CSingleton();} 
	return m_pInstance;
}

/*************************************************/
/*   数据库操作接口（对数据库进行增、删、改、查）*/
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

//有缘类的构造函数
xugou::xugou()
{}

xugou::~xugou()
{
	if(CSingleton::m_pInstance)
	{
		int iresult;
		iresult=sqlite3_close( CSingleton::m_pInstance->db);

        if(iresult==SQLITE_OK)  
		{printf("关闭数据库\n");}
		else
		{printf("数据库关闭失败\n");}
		
		delete CSingleton::m_pInstance;
	    printf("释放单例\n");
	}
	
}



