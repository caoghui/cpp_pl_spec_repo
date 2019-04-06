#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <cstdarg>
#include <condition_variable>
#include <future>
#include <chrono>
#include <string>
#include <vector>
#include <random>
#include <queue>
#include <exception>
#include <type_traits>

using namespace std;

void thread_task(int& i, double d, const string& s)
{
	cout << "hello thread : " << i++ << "  " << d << "  " << s << endl;
}
//////////////////////////////////////////////////////
template<typename T>
class container
{
	//mutex _lock;
	recursive_mutex _lock;
	vector<T> _elements;
public:
	void add(T element)
	{
		//_lock.lock();
		lock_guard<recursive_mutex> locker(_lock);
		_elements.push_back(element);
		//_lock.unlock();
	}

	void addrange(int num, ...)
	{
		va_list arguments;
		va_start(arguments, num);
		for (int i = 0; i < num; i++)
		{
			//_lock.lock();
			lock_guard<recursive_mutex> locker(_lock);
			add(va_arg(arguments, T));
			//_lock.unlock();
		}
		va_end(arguments);
	}

	void dump()
	{
		//_lock.lock();
		lock_guard<recursive_mutex> locker(_lock);
		for (auto e : _elements)
			cout << e << endl;
		//_lock.unlock();
	}
};

void func1(container<int>& cont)
{
	//this_thread::sleep_for(std::chrono::seconds(3));
	cont.addrange(3, rand(), rand(), rand());
}

void test_cont1();
//////////////////////////////////////////////////////
mutex g_mutex;
vector<exception_ptr> g_exceptions;

void throw_function()
{
	throw exception("something wrong happened");
}

void func()
{
	try
	{
		throw_function();
	}
	catch (...)
	{
		lock_guard<mutex> lock(g_mutex);
		g_exceptions.push_back(current_exception());
	}
}

//////////////////////////////////////////////////////
mutex  g_lockprint;
mutex  g_lockqueue;
condition_variable g_queuecheck;
queue<int> g_codes;
bool g_done;
bool g_notified;

void workerfunc(int id, mt19937& generator)
{
	{
		unique_lock<mutex> locker(g_lockprint);
		cout << "[worker " << id << "]  \t running..." << endl;
	}

	//simulate work
	this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));
	//simulate error
	int errorcode = id * 100 + 1;
	{
		unique_lock<mutex> locker(g_lockprint);
		cout << "[worker " << id << "]\t an error occurred : " << errorcode << endl;
	}
	//notify error to be logged
	{
		unique_lock<mutex> locker(g_lockqueue);
		g_codes.push(errorcode);
		//g_notified = true;
		g_queuecheck.notify_one();
	}
}

void loggerfunc()
{
	{
		unique_lock<mutex> locker(g_lockprint);
		cout << "[logger]\t running...." << endl;
	}
	//loop until end is signaled
	while (!g_done)
	{
		unique_lock<mutex> locker(g_lockqueue);
		//used to avoid spurious wakeups
		g_queuecheck.wait(locker, [&]() { return !g_codes.empty(); });
		//if there are error codes in the queue process them
		while (!g_codes.empty())
		{
			unique_lock<mutex> locker(g_lockprint);
			cout << "[logger]\t processing error : " << g_codes.front() << endl;
			g_codes.pop();
		}
		//g_notified = false;
	}
}

//////////////////////////////////////////////////////
//timed_mutex mtx;
mutex mtx;
volatile int counter(0);

void fireworks()
{
	for (int i = 0; i < 10000; ++i)
	{
		if (mtx.try_lock())
		{
			++counter;
			mtx.unlock();
		}
	}
	/*
	while (!mtx.try_lock_for(chrono::milliseconds(200)))
	{
		cout << "-";
	}
	this_thread::sleep_for(chrono::microseconds(1000));
	cout << "*" << endl;
	mtx.unlock();
	*/
}


/////////////////////////////////////////////////////////
//²âÊÔº¯Êý
void test_case1();
void test_excep();
void test_mtx();
/////////////////////////////////////////////////////////


int main(int argc, char** argv)
{
	/*
	int a = 42;
	thread t(thread_task, ref(a), 12.5, ("sample"));
	t.join();
	cout << a << endl;
	*/
	//test_case1();
	//test_excep();
	//test_cont1();

	test_mtx();
	return 0;

	//initialize a random generator
	mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

	//start the logger
	thread loggerthread(loggerfunc);
	//start the working threads
	vector<thread> threads;
	for (int i = 0; i < 5; ++i)
	{
		threads.push_back(thread(workerfunc, i + 1, ref(generator)));
	}

	for (auto& t : threads)
	{
		t.join();
	}

	//notify the logger to finish and wait for it
	g_done = true;
	loggerthread.join();

	return 0;
}

void test_mtx()
{
	thread threads[10];
	for (int i = 0; i < 10; ++i)
	{
		threads[i] = thread(fireworks);
	}
	for (auto& t : threads)
	{
		t.join();
	}
	cout << counter << " successful increases of the counter" << endl;
}

void test_cont1()
{
	srand((unsigned int)time(0));

	container<int> cont;

	thread t1(func1, ref(cont));
	thread t2(func1, ref(cont));
	thread t3(func1, ref(cont));

	t1.join();
	t2.join();
	t3.join();

	cont.dump();
}

void test_case1()
{
	thread threads[10];
	//mtx.try_lock();
	for (int i = 0; i < 10; ++i)
	{
		threads[i] = thread(fireworks);
	}
	for (auto & thd : threads)
		thd.join();
}

void test_excep()
{
	g_exceptions.clear();
	thread t(func);
	t.join();

	for (auto& e : g_exceptions)
	{
		try
		{
			if (e != nullptr)
			{
				rethrow_exception(e);
			}
		}
		catch (const exception& e)
		{
			cout << e.what() << endl;
		}
	}
}