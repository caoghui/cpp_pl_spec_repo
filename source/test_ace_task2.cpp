#include <iostream>
#include "ace/ACE.h"
#include "ace/OS_main.h"
#include "ace/Task.h"
#include "ace/Thread_Hook.h"
#include "ace/Atomic_Op.h"
#include "ace/Barrier.h"

using namespace std;

static ACE_Atomic_Op<ACE_Thread_Mutex, int> close_cleanups(0);

class My_Thread_Hook : public ACE_Thread_Hook
{
public:
	virtual ACE_THR_FUNC_RETURN start(ACE_THR_FUNC func, void* arg)
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) starting the thread!\n")));
		return (func)(arg);
	}
};

class Barrier_Task : public ACE_Task<ACE_MT_SYNCH>
{
private:
	ACE_Barrier barrier_;
	int n_iterations_;
public:
	Barrier_Task(ACE_Thread_Manager* thr_mgr, int n_threads, int n_iterations) : ACE_Task<ACE_MT_SYNCH>(thr_mgr), barrier_(n_threads), n_iterations_(n_iterations)
	{
		if(this->activate(THR_NEW_LWP, n_threads) == -1)
			ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("activate failed")));
	}

	virtual int close(u_long flags = 0)
	{
		if (ACE_OS::thr_equal(ACE_Thread::self(), this->last_thread()))
		{
			++close_cleanups;
		}
		return 0;
	}
	virtual int svc()
	{
		for (int iterations = 1; iterations <= this->n_iterations_; ++iterations)
		{
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) in iteration %d\n"), iterations));

			//block until all other threads have waited, then continue
			this->barrier_.wait();
		}
		return 0;
	}
};

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
	ACE_Thread_Hook::thread_hook(new My_Thread_Hook);
	int n_threads = 5;
	int n_iterations = 10;

	Barrier_Task barrier_task(ACE_Thread_Manager::instance(), n_threads, n_iterations);
	ACE_Thread_Manager::instance()->wait();

	//only one of the threads should see a cleanup
	if (close_cleanups != 1)
	{
		ACE_ERROR((LM_ERROR, ACE_TEXT("%d threads saw cleanup indication; should be 1\n"), close_cleanups.value()));
	}
	delete ACE_Thread_Hook::thread_hook();

	return 0;
}
