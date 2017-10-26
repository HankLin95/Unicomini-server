#include "ThreadPool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

CThreadPool::CThreadPool(int maxCount, int waittime)  
{
	//设置最大线程数量
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

	//初始化互斥量和条件变量
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
}

void CThreadPool::lock()
{
	//互斥量加锁
	
	if( pthread_mutex_lock(&m_mutex) != 0)
		perror("Lock error");
		//printf("L\n");
}

void CThreadPool::unlock()
{
	//互斥量解锁
	if( pthread_mutex_unlock(&m_mutex) != 0)
		perror("Unlock error");
	//printf("U\n");
}

int CThreadPool::wait()
{
	//设置添加变量等待
	//条件变量等待一定要在互斥锁区域内
	//lock();
	int ret = pthread_cond_wait(&m_cond, &m_mutex);
	//unlock();
	return ret;
}

int CThreadPool::waittime(int sec)
{
	//设置添加变量等待时间sec秒
	//条件变量等待一定要在互斥锁区域内
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
	//发送条件变量信号
	return pthread_cond_signal(&m_cond);
}

int CThreadPool::broatcast()
{
	//广播条件变量信号
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
	//如果没有线程等待，并且线程的数量小于最大数，线程池不是停止
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
 
//线程池线程运行函数
void* CThreadPool::rontine(void * threadData)  
{
	//////////////////////////////////////////////////////////////////////////
	//posix线程执行有两种状态joinable状态和unjoinable状态，
	//在joinable状态，当线程函数自己返回或pthread_exit时
	//不会释放线程所占用堆栈和线程描述符（总计8K多）。
	//只有当你调用了pthread_join之后这些资源才会被释放。
	//若是unjoinable状态的线程，这些资源在线程函数退出时或pthread_exit时自动会被释放。
	//unjoinable属性可以在pthread_create时指定，或在线程创建后在线程中pthread_detach自己, 
	//如：pthread_detach(pthread_self())，将状态改为unjoinable状态，确保资源的释放。
	//或者将线程置为 joinable,然后适时调用pthread_join.
	//其实简单的说就是在线程函数头加上 pthread_detach(pthread_self())的话，
	//线程状态改变为unjoinable，在pthread_exit线程就会在退出时自动释放资源。
	pthread_detach(pthread_self());
	//////////////////////////////////////////////////////////////////////////

	CThreadPool *pool = (CThreadPool *)threadData;

	while(1)
	{
		pool->lock();
		if (pool->m_bStop)
		{
			//线程池停止，线程退出，线程id从列表中移出
			break;
		}
		
		//如果没有任务，线程进入等待状态
		if (pool->m_taskList.size() == 0)
		{

			//等待的线程计数加一
			pool->m_waitCount++;
			int ret = pool->waittime(pool->m_waittime);

			//线程被唤醒，等待的线程计数减一
			pool->m_waitCount--;

			//判断是否是等待超时
			if (ret == ETIMEDOUT)
			{
				break;
			}
		}

		//如果有任务
		if (pool->m_taskList.size() > 0)
		{
			//获取任务
			  			
			CTask *task = pool->m_taskList.front();
			pool->m_taskList.pop_front();
		//	cout << "current task count : " << pool->m_taskList.size() << endl;
			pool->unlock();			
		
			//要在运行任务之前解锁，以免锁被占用
			
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
	//判断线程池是否已经停止
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

	//如果有线程在等待，发送信号唤醒一个线程去处理
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
	//设置退出标识为true
	m_bStop = true;

	//清空任务列表
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

	//广播信号，然后等待的线程解除阻塞
	
	broatcast();

	//sleep(1);

	wait();

	return 0;

}
