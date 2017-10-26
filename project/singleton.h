#ifndef  _SINGLETON_H_
#define  _SINGLETON_H_

#include "sqlite3.h"
#include <stdio.h>
#include <string.h>

class xugou;

class  CSingleton
{

public:
    static CSingleton *GetInstance();
    void get_data(char* sql,char**(&presult),int &rol,int &col);
    friend class xugou;   //友元类

private:
	CSingleton();   //构造函数是私有的
	static CSingleton *m_pInstance;
	sqlite3 *db;
protected:
};



class xugou
{
public:
    xugou();
    ~xugou();

private:
	static xugou xugou1;
protected:
};



#endif
