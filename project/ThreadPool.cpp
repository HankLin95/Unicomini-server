#include "ThreadPool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

CThreadPool::CThreadPool(int maxCount, int waittime)  
{
	//��������߳�����
	m_maxCount  = maxCount;
	if (m_maxCount <= 0)
	{
		printf("[threadpool] thread max count error \n");
		exit(-1);
	}
	m_bStop		= false;
	m_waitCount = 0;
	m_count     = 0;
	m_waittime  = waittime;
	if (m_waittime <= 0)
	{
		printf("[threadpool] thread wait time error \n");
		exit(-1);
	}

	//��ʼ������������������
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
}

void CThreadPool::lock()
{
	//����������
	
	if( pthread_mutex_lock(&m_mutex) != 0)
		perror("Lock error");
		//printf("L\n");
}

void CThreadPool::unlock()
{
	//����������
	if( pthread_mutex_unlock(&m_mutex) != 0)
		perror("Unlock error");
	//printf("U\n");
}

int CThreadPool::wait()
{
	//������ӱ����ȴ�
	//���������ȴ�һ��Ҫ�ڻ�����������
	//lock();
	int ret = pthread_cond_wait(&m_cond, &m_mutex);
	//unlock();
	return ret;
}

int CThreadPool::waittime(int sec)
{
	//������ӱ����ȴ�ʱ��sec��
	//���������ȴ�һ��Ҫ�ڻ�����������
	//lock();
	struct timespec abstime;
	clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_sec += sec;
	int ret = pthread_cond_timedwait(&m_cond, &m_mutex, &abstime);
	//unlock();
	return ret;
}

int CThreadPool::signal()
{
	//�������������ź�
	return pthread_cond_signal(&m_cond);
}

int CThreadPool::broatcast()
{
	//�㲥���������ź�
	return pthread_cond_broadcast(&m_cond);
}

int CThreadPool::start(int threadcount)
{
	if (threadcount <= 0)
	{
		printf("create thread count error\n");
		return -1;
	}

	threadcount = threadcount > m_maxCount ? m_maxCount : threadcount;

	lock();
	for (int i = 0; i < threadcount; i++)
	{
		createThread();
	}

	unlock();

}

void CThreadPool::createThread()
{
	//���û���̵߳ȴ��������̵߳�����С����������̳߳ز���ֹͣ
	if( m_count < m_maxCount && !m_bStop)
	{
		pthread_t tid = 0;  
		if( pthread_create(&tid,NULL,rontine,(void *)this) != 0)
		{
			perror("create thread error : ");
			printf("current thread count %d max count %d\n", m_count, m_maxCount);
		}
		else
		{
			printf("create thread id 0x%x\n",tid);
			m_count++;
		}
	}
}
 
//�̳߳��߳����к���
void* CThreadPool::rontine(void * threadData)  
{
	//////////////////////////////////////////////////////////////////////////
	//posix�߳�ִ��������״̬joinable״̬��unjoinable״̬��
	//��joinable״̬�����̺߳����Լ����ػ�pthread_exitʱ
	//�����ͷ��߳���ռ�ö�ջ���߳����������ܼ�8K�ࣩ��
	//ֻ�е��������pthread_join֮����Щ��Դ�Żᱻ�ͷš�
	//����unjoinable״̬���̣߳���Щ��Դ���̺߳����˳�ʱ��pthread_exitʱ�Զ��ᱻ�ͷš�
	//unjoinable���Կ�����pthread_createʱָ���������̴߳��������߳���pthread_detach�Լ�, 
	//�磺pthread_detach(pthread_self())����״̬��Ϊunjoinable״̬��ȷ����Դ���ͷš�
	//���߽��߳���Ϊ joinable,Ȼ����ʱ����pthread_join.
	//��ʵ�򵥵�˵�������̺߳���ͷ���� pthread_detach(pthread_self())�Ļ���
	//�߳�״̬�ı�Ϊunjoinable����pthread_exit�߳̾ͻ����˳�ʱ�Զ��ͷ���Դ��
	pthread_detach(pthread_self());
	//////////////////////////////////////////////////////////////////////////

	CThreadPool *pool = (CThreadPool *)threadData;

	while(1)
	{
		pool->lock();
		if (pool->m_bStop)
		{
			//�̳߳�ֹͣ���߳��˳����߳�id���б����Ƴ�
			break;
		}
		
		//���û�������߳̽���ȴ�״̬
		if (pool->m_taskList.size() == 0)
		{

			//�ȴ����̼߳�����һ
			pool->m_waitCount++;
			int ret = pool->waittime(pool->m_waittime);

			//�̱߳����ѣ��ȴ����̼߳�����һ
			pool->m_waitCount--;

			//�ж��Ƿ��ǵȴ���ʱ
			if (ret == ETIMEDOUT)
			{
				break;
			}
		}

		//���������
		if (pool->m_taskList.size() > 0)
		{
			//��ȡ����
			  			
			CTask *task = pool->m_taskList.front();
			pool->m_taskList.pop_front();
		//	cout << "current task count : " << pool->m_taskList.size() << endl;
			pool->unlock();			
		
			//Ҫ����������֮ǰ��������������ռ��
			
			task->run();
			delete task;
			//pool->lock();
			
		}
		else
		{
			pool->unlock();
		}
	}
	cout<< "thread exit" << endl;
	pool->m_count--;
	printf("thread count %d task count %d\n",pool->m_count,pool->m_taskList.size());

	if (pool->m_count <= 0 && pool->m_bStop )
	{
		pool->signal();
	}

	pool->unlock();

	pthread_exit(NULL);
	//return NULL;
}

int CThreadPool::addTask(CTask *task)  
{
	//�ж��̳߳��Ƿ��Ѿ�ֹͣ
	lock();
	if ( !m_bStop )
	{
		m_taskList.push_back(task);
	}
	else
	{
		unlock();
		return -1;
	}

	//������߳��ڵȴ��������źŻ���һ���߳�ȥ����
	if (m_waitCount > 0)
	{
		signal();
	}
	else
	{
		createThread();
	}

	unlock();
	return 0;  
}

int CThreadPool::destroy()
{
	//�����˳���ʶΪtrue
	m_bStop = true;

	//��������б�
	lock();
	list<CTask *>::iterator it = m_taskList.begin();
	for (; it != m_taskList.end(); it++)
	{
		CTask *task = *it;
		delete task;
		task = NULL;
	}
	m_taskList.clear();
	unlock();

	//�㲥�źţ�Ȼ��ȴ����߳̽������
	
	broatcast();

	//sleep(1);

	wait();

	return 0;

}
