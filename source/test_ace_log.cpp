#include <iostream>
#include "ace/OS_main.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Record.h"
#include "ace/Log_Msg_Callback.h"
#include "ace/OS_NS_stdio.h"

using namespace std;

class Logger : public ACE_Log_Msg_Callback
{
public:
	//Constructor sets whether we are testing recursive callbacl logging
	Logger(int be_recursive = 1) : recursive_(be_recursive) { }

	virtual void log(ACE_Log_Record& log_record);
	void verbose(int be_verbose)
	{
		this->verbose_logging_ = be_verbose;
	}

private:
	int verbose_logging_;
	int recursive_;
};

void Logger::log(ACE_Log_Record& log_record)
{
	int use_log_msg = 0;
	if (this->recursive_)
	{
		this->recursive_ = 0;
		use_log_msg = 1;
	}

	if (!this->verbose_logging_)
	{
		if (use_log_msg)
		{
			ACE_DEBUG((LM_DEBUG, "Logger::log -> %s", log_record.msg_data()));
		}
		else
		{
			ACE_OS::printf("recursive Logger callback = %s", ACE_TEXT_ALWAYS_CHAR(log_record.msg_data()));
		}
	}
	else
	{
		ACE_TCHAR verbose_msg[ACE_Log_Record::MAXVERBOSELOGMSGLEN];
		int result = log_record.format_msg(ACE_LOG_MSG->local_host(), ACE_LOG_MSG->flags(), verbose_msg, ACE_Log_Record::MAXVERBOSELOGMSGLEN);
		if (result == 0)
		{
			if (use_log_msg)
			{
				ACE_DEBUG((LM_DEBUG, "Logger::log -> %s", verbose_msg));
			}
			else
			{
				ACE_OS::printf("Recursive Logger callback = %s", ACE_TEXT_ALWAYS_CHAR(verbose_msg));
			}
		}
	}
	if (use_log_msg)
		this->recursive_ = 1;
}

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
	//this message should show up in stderr
	ACE_DEBUG((LM_DEBUG, "(%t) first message \n"));
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR);

	//this message should not show up anywhere since we disabled STDERR
	ACE_DEBUG((LM_DEBUG, "(%t) second message \n"));
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::MSG_CALLBACK);

	//this message should not show up anywhere since no callback object has been specified
	ACE_DEBUG((LM_DEBUG, "(%t) third message \n"));

	//Create a callback object and make it verbose
	Logger logger;
	logger.verbose(1);
	//set the callback object
	ACE_LOG_MSG->msg_callback(&logger);

	//This message should show up via the Logger callback
	ACE_DEBUG((LM_DEBUG, "(%t) fourth message \n"));
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::VERBOSE_LITE);

	ACE_DEBUG((LM_DEBUG, "(%t) fifth message \n"));
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::VERBOSE);

	//this message should show up via the logger callback
	ACE_DEBUG((LM_DEBUG, "(%t) sixth message \n"));

	logger.verbose(0);

	ACE_DEBUG((LM_DEBUG, "(%t) seventh message \n"));
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::STDERR);
	ACE_DEBUG((LM_DEBUG, "(%t) eighth message \n"));


	return 0;
}
