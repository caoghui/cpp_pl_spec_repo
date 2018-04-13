#include <iostream>
#include <signal.h>
#include "ace/Log_Msg.h"
#include "ace/Reactor.h"
#include "ace/Timer_Queue.h"
#include "ace/Event_Handler.h"

using namespace std;

#define NUMBER_TIMERS 10

static int done = 0;
static int count = 0;

class MyEventHandler : public ACE_Event_Handler
{
public:
	int handle_signal(int signum, siginfo_t*, ucontext_t*)
	{
		switch (signum)
		{
		case SIGINT:
			ACE_DEBUG((LM_DEBUG, "You passed SIGINT \n"));
			exit(0);
			break;
		}
		return 0;
	}

	virtual int handle_timeout(const ACE_Time_Value& tv, const void* arg)
	{
		long current_count = long(arg);

		ACE_DEBUG((LM_DEBUG, "Timer #%d timed out at %d!\n", current_count, tv.sec()));

		//increment count
		//count ++;
		//ACE_DEBUG((LM_DEBUG, "handle_timeout\n"));
		return 0;
	}
};

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	cout << "Test ACE Reactor is running...." << endl;
	MyEventHandler* eh = new MyEventHandler;

	int timer_id[NUMBER_TIMERS];

	for (int i = 0; i < NUMBER_TIMERS; ++i)
	{
		timer_id[i] = ACE_Reactor::instance()->schedule_timer(eh, (const void*)i, ACE_Time_Value(2 * i + 1));
	}

	ACE_Reactor::instance()->register_handler(SIGINT, eh);
	while (true)
	{
		ACE_Reactor::instance()->handle_events();
	}

	return 0;
}