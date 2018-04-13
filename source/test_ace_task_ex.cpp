#include <iostream>
#include "ace/Basic_Types.h"
#include "ace/ACE.h"
#include "ace/OS_main.h"
#include "ace/Task_Ex_T.h"
#include "ace/Log_Msg.h"
#include "ace/Auto_Ptr.h"

using namespace std;

const ACE_INT32 PRODUCER_THREADS_NO = 5;
const ACE_INT32 CONSUMER_THREADS_NO = 5;
const ACE_INT32 NUMBER_OF_MSGS      = 100;

class User_Defined_Msg
{
public:
	User_Defined_Msg(ACE_INT32 id) : msg_id_(id) {}
	ACE_INT32 msg_id() const { return msg_id_; }
private:
	ACE_INT32 msg_id_;
};

class Consumer : public ACE_Task_Ex<ACE_MT_SYNCH, User_Defined_Msg>
{
public:
	int open(void* arg)
	{
		if (this->activate(THR_NEW_LWP | THR_JOINABLE, CONSUMER_THREADS_NO) == -1)
		{
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Consumer::open Error spanwing thread %p\n"), "err="), -1);
		}
		ACE_DEBUG((LM_DEBUG, "(%t) consumer thread is running\n"));
		return 0;
	}

	virtual int svc()
	{
		User_Defined_Msg* pMsg = 0;
		while (this->getq(pMsg) != -1)
		{
			ACE_TEST_ASSERT(pMsg != 0);
			auto_ptr<User_Defined_Msg> pAuto(pMsg);
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Consumer::svc got msg id = %d \n"), pMsg->msg_id()));
			if (pMsg->msg_id() == NUMBER_OF_MSGS - 1)
				break;
		}
		ACE_DEBUG((LM_INFO, ACE_TEXT("Consumer::svc ended thread %t\n")));
		return 0;
	}
};

//producer function produces user defined messages
ACE_THR_FUNC_RETURN producer(void* arg)
{
	Consumer* c = static_cast<Consumer*>(arg);
	ACE_TEST_ASSERT(c != 0);

	if (c == 0)
	{
		ACE_ERROR((LM_ERROR, ACE_TEXT("producer error casting to consumer\n")));
		return (ACE_THR_FUNC_RETURN) -1;
	}
	for (int i = 0; i != NUMBER_OF_MSGS; ++i)
	{
		User_Defined_Msg* pMsg = 0;
		ACE_NEW_NORETURN(pMsg, User_Defined_Msg(i));
		if (pMsg == 0)
		{
			return (ACE_THR_FUNC_RETURN) -1;
		}
		if (c->putq(pMsg) == -1)
		{
			return (ACE_THR_FUNC_RETURN)-1;
		}
	}

	return 0;
}

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
	Consumer c;
	if (c.open(0) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("main error opening consumer\n")), -1);
	}

	int result = ACE_Thread_Manager::instance()->spawn_n(PRODUCER_THREADS_NO, ACE_THR_FUNC(producer), static_cast<void*>(&c));
	if (result == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("main error spawning threads %p\n"), "err="), -1);
	}

	//wait all threads
	result = ACE_Thread_Manager::instance()->wait();
	if (result == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("mian error Thread_Manager->wait %p\n"), "err="), -1);
	}

	return 0;
}
