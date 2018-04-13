#include <iostream>
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_time.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_main.h"
#include "ace/Task.h"
#include "ace/Service_Config.h"
#include "ace/Containers.h"
#include "ace/Synch.h"
#include "ace/SString.h"
#include "ace/Method_Request.h"
#include "ace/Future.h"
#include "ace/Activation_Queue.h"
#include "ace/Condition_T.h"

using namespace std;

class Worker;

class IManager
{
public:
	virtual ~IManager() {}
	virtual int return_to_work(Worker* worker) = 0;
};

class Worker : public ACE_Task<ACE_MT_SYNCH>
{
private:
	IManager* manager_;
	ACE_Thread_ID thread_id_;

	void process_message(ACE_Message_Block* mb)
	{
		ACE_TRACE("Worker::process_message");
		int msgId;
		ACE_OS::memcpy(&msgId, mb->rd_ptr(), sizeof(int));
		mb->release();

		//处理消息
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Started processing message %d\n"), msgId));
		ACE_OS::sleep(3);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Finished processing message %d\n"), msgId));
	}

public:
	Worker(IManager* manager) : manager_(manager) {}

	//线程函数
	virtual int svc(void)
	{
		ACE_Thread_ID id;
		thread_id_ = id;
		while (1)
		{
			ACE_Message_Block* mb = 0;
			if (this->getq(mb) == -1)
			{
				ACE_ERROR_BREAK((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("getq")));
			}

			//处理退出消息
			if (mb->msg_type() == ACE_Message_Block::MB_HANGUP)
			{
				ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) Shutting down\n")));
				mb->release();
				break;
			}

			//处理消息
			process_message(mb);
			this->manager_->return_to_work(this);
		}
		return 0;
	}

	const ACE_Thread_ID& thread_id()
	{
		return this->thread_id_;
	}
};

class Manager : public ACE_Task<ACE_MT_SYNCH>, private IManager
{
private:
	int shutdown_;
	ACE_Thread_Mutex workers_lock_;
	ACE_Condition<ACE_Thread_Mutex> workers_cond_;
	ACE_Unbounded_Queue<Worker*> workers_;

	int create_worker_pool()
	{
		ACE_GUARD_RETURN(ACE_Thread_Mutex, worker_mon, this->workers_lock_, -1);
		for (int i = 0; i < POOL_SIZE; ++i)
		{
			Worker* worker = 0;
			ACE_NEW_RETURN(worker, Worker(this), -1);
			this->workers_.enqueue_tail(worker);
			worker->activate();
		}
		return 0;
	}

	int done() { return (shutdown_ == 1); }
public:
	enum { POOL_SIZE = 5, MAX_TIMEOUT = 5};

	Manager() : shutdown_(0), workers_lock_(), workers_cond_(workers_lock_)
	{}

	int svc()
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Manager started\n")));
		create_worker_pool();

		while (!done())
		{
			ACE_Message_Block* mb = 0;
			ACE_Time_Value tv((long)MAX_TIMEOUT);
			tv += ACE_OS::time(0);

			//Get a message request
			if (this->getq(mb, &tv) < 0)
			{
				shut_down();
				break;
			}

			//选择工作者线程
			Worker* worker = 0;
			{
				ACE_GUARD_RETURN(ACE_Thread_Mutex, worker_mon, this->workers_lock_, -1);
				while (this->workers_.is_empty())
				{
					workers_cond_.wait();
				}
				this->workers_.dequeue_head(worker);
			}
			//分配任务
			worker->putq(mb);
		}

		return 0;
	}
	int shut_down();

	const ACE_Thread_ID& thread_id(Worker* worker)
	{
		return worker->thread_id();
	}

	virtual int return_to_work(Worker* worker)
	{
		ACE_GUARD_RETURN(ACE_Thread_Mutex, worker_mon, this->workers_lock_, -1);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Worker %t returning to work \n")));
		this->workers_.enqueue_tail(worker);
		this->workers_cond_.signal();
		return 0;
	}
};

int Manager::shut_down()
{
	ACE_TRACE("Manager::shut_down");
	ACE_Unbounded_Queue<Worker*>::ITERATOR iter = this->workers_.begin();
	Worker** worker_ptr = 0;
	do
	{
		iter.next(worker_ptr);
		Worker* worker = (*worker_ptr);
		ACE_Thread_ID id = thread_id(worker);

		char buf[65];
		id.to_string(buf);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Attempting shutdown of %C\n"), buf));

		//send the hangup message
		ACE_Message_Block* mb = 0;
		ACE_NEW_RETURN(mb, ACE_Message_Block(0, ACE_Message_Block::MB_HANGUP), -1);
		worker->putq(mb);

		//wait for the exit
		worker->wait();

		ACE_ASSERT(worker->msg_queue()->is_empty());
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Worker %C shut down. \n"), buf));
		delete worker;

	} while (iter.advance());
	shutdown_ = 1;
	return 0;
}

int test_task()
{
	Manager tp;
	tp.activate();

	//wait for a moment every time you send a message
	ACE_Time_Value tv;
	tv.msec(100);

	ACE_Message_Block* mb = 0;
	for (int i = 0; i < 30; ++i)
	{
		ACE_NEW_RETURN(mb, ACE_Message_Block(sizeof(int)), -1);
		ACE_OS::memcpy(mb->wr_ptr(), &i, sizeof(int));
		ACE_OS::sleep(tv);

		tp.putq(mb);
	}
	ACE_Thread_Manager::instance()->wait();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////

class Thread_Pool : public ACE_Task<ACE_MT_SYNCH>
{
public:
	Thread_Pool(ACE_Thread_Manager* thr_mgr, int n_threads) : ACE_Task<ACE_MT_SYNCH>(thr_mgr)
	{
		if (this->activate(THR_NEW_LWP, n_threads) == -1)
		{
			ACE_ERROR((LM_ERROR, "%p\n", "activate failed"));
		}
	}
	~Thread_Pool() {}

	virtual int svc();

	virtual int put(ACE_Message_Block* mb, ACE_Time_Value* tv = 0)
	{
		return this->putq(mb, tv);
	}
private:
	virtual int close()
	{
		ACE_DEBUG((LM_DEBUG, "(%t) worker thread closing down\n"));
		return 0;
	}
};

int Thread_Pool::svc()
{
	//Note that the <ACE_Task::svc_run> method automatically adds us to the Thread_Manager when the thread begins
	int count = 1;
	//Keep looping, reading a message out of the queue, until we get a message with a length == 0, which signals us to quit
	for (;; count++)
	{
		ACE_Message_Block* mb = 0;
		ACE_DEBUG((LM_DEBUG, "(%t) in iteration %d before getq()\n", count));
		if (this->getq(mb) == -1)
		{
			ACE_ERROR((LM_ERROR, "(%t) in iteration %d, got result -1, exiting\n", count));
			break;
		}
		size_t length = mb->length();
		if (length > 0)
		{
			ACE_DEBUG((LM_DEBUG, "(%t) in iteration %d, length = %d, text = \"%*s\"\n", count, length, length - 1, mb->rd_ptr()));
		}
		//we are responsible for deallocating this
		mb->release();

		if (length == 0)
		{
			ACE_DEBUG((LM_DEBUG, "(%t) in iteration %d, got NULL message, exiting\n", count));
			break;
		}
	}

	return 0;
}

// Default number of iterations to run the test.
static int n_iterations = 100;

// Controls whether the input is generated "manually" or automatically.
static int manual = 0;

static void producer(Thread_Pool& thread_pool)
{
	ACE_DEBUG((LM_DEBUG, "(%t) producer start, generating data for this Thread_Pool \n"));
	for (int n;;)
	{
		//allocate a new message
		ACE_Message_Block* mb = 0;
		ACE_NEW(mb, ACE_Message_Block(BUFSIZ));
		if (manual)
		{
			ACE_DEBUG((LM_DEBUG, "(%t) enter a new message for the task pool..."));
			n = ACE_OS::read(ACE_STDIN, mb->rd_ptr(), mb->size());
		}
		else
		{
			static int count = 0;
			ACE_OS::sprintf(mb->rd_ptr(), "%d\n", count);
			n = ACE_Utils::truncate_cast<int>(ACE_OS::strlen(mb->rd_ptr()));
			if (count == n_iterations)
				n = 1;
			else
				count++;

			if (count == 0 || (count % 20 == 0))
				ACE_OS::sleep(1);
		}
		if (n > 1)
		{
			//send a normal message to the waiting threads and continue producing
			mb->wr_ptr(n);
			//pass the message to the Thread_Pool
			if (thread_pool.put(mb) == -1)
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put"));
		}
		else
		{
			ACE_DEBUG((LM_DEBUG, "\n(%t) start loop, dump of task:\n"));
			//send a shutdown message to the waiting threads and exit
			for (size_t i = thread_pool.thr_count(); i > 0; i--)
			{
				ACE_DEBUG((LM_DEBUG, "(%t) EOF, enqueueing NULL block for thread = %d\n", i));

				//enqueue a NULL message to flag each consumer to shutdown
				ACE_Message_Block* mb = 0;
				ACE_NEW(mb, ACE_Message_Block);
				if (thread_pool.put(mb) == -1)
				{
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put"));
				}
			}
			ACE_DEBUG((LM_DEBUG, "\n(%t) end loop"));
			break;
		}
	}
}

int test_thread_pool(int argc, ACE_TCHAR* argv[])
{
	int n_threads = argc > 1 ? ACE_OS::atoi(argv[1]) : ACE_DEFAULT_THREADS;
	n_iterations  = argc > 2 ? ACE_OS::atoi(argv[2]) : n_iterations;
	manual        = argc > 3 ? 1 : 0;

	ACE_DEBUG((LM_DEBUG, "(%t) argc = %d, threads = %d\n", argc, n_threads));
	//Create the worker tasks
	Thread_Pool thread_pool(ACE_Thread_Manager::instance(), n_threads);

	//Create work for the worker tasks to process in their own threads
	producer(thread_pool);

	ACE_DEBUG((LM_DEBUG, "(%t) waiting for threads to exit in Thread_Pool destructor...\n"));
	//Wait for all the threads to reach their exit point
	if (thread_pool.wait() == -1)
		ACE_ERROR_RETURN((LM_DEBUG, "(%t) wait() failed\n"), 1);
	
	ACE_DEBUG((LM_DEBUG, "(%t) destroying worker tasks and exiting...\n"));
	return 0;
}

int ACE_TMAIN(int argc, ACE_TCHAR**argv)
{
	//test_task();
	test_thread_pool(argc, argv);
	return 0;
}