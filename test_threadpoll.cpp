
#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include "ThreadPool.h"
#include "ThreadPool_v2.h"
#include "ThreadPool_v1.h"
using namespace std;

void fun1(int s)
{
	cout << std::this_thread::get_id() << " hello, fun1! " << endl;
	if (s > 0)
	{
		cout << "fun1 sleep : " << s << endl;
		this_thread::sleep_for(std::chrono::milliseconds(s));
	}
}

struct gfun {
	int operator()(int n)
	{
		cout << "hello, gfun! " << this_thread::get_id() << "  ; n = " << n << endl;
		return 42;
	}
};

class A
{
public:
	static int Afun(int n = 0)
	{
		cout << n << " hello, Afun! " << this_thread::get_id() << endl;
		return n;
	}
	static std::string Bfun(int n, string str, char c)
	{
		cout << this_thread::get_id() << " hello Bfun! " << str << endl;
		return str;
	}
};

void test_threadpool_v0();
void test_threadpool_v1();
void test_threadpool_v2();

int main(int argc, char** argv)
{
	try
	{
		cout << "the hardware thread core : " << std::thread::hardware_concurrency() << endl;
		test_threadpool_v0();
		//test_threadpool_v1();
		//test_threadpool_v2();
	}
	catch (std::exception& e)
	{
		cout << "some unhappy happened..." << e.what() << endl;
	}
    return 0;
}

void test_threadpool_v0()
{
	cout << "==============main thread=============" << this_thread::get_id() << endl;
	ThreadPool::ThreadPool pool{ 4 };

	this_thread::sleep_for(std::chrono::seconds(3));

	vector<std::future<int>> results;
	for (int i = 0; i < 8; ++i)
	{
		results.emplace_back(
			pool.commit(
				[i] 
				{
					ThreadPool::print(i, true);
					this_thread::sleep_for(std::chrono::seconds(2));
					return i*i;
				})
		);
	}
	this_thread::sleep_for(std::chrono::seconds(15));

	pool.stop();
	cout << "==============commit all=============" << this_thread::get_id() << endl;

	for (auto&& r : results)
	{
		cout << r.get() << "  ";
	}
	cout << endl;
}

void test_threadpool_v1()
{
	print("test_threadpoll_v1 run...\n");
	ThreadPoolV1::ThreadPool pool(4);

	this_thread::sleep_for(std::chrono::seconds(10));



	std::vector<std::future<int>> results;
	for (int i = 0; i < 8; ++i)
	{
		results.emplace_back(pool.enqueue([i] {
			print("hello ");
			std::this_thread::sleep_for(std::chrono::seconds(1));
			print("world");			
			return i*i;
		}));
	}

	this_thread::sleep_for(std::chrono::seconds(10));

	for (auto&& result : results)
	{		
	}
	cout << endl;
}

void test_threadpool_v2()
{
	//ThreadPool::ThreadPool executor{50};
	ThreadPoolV2::ThreadPool executor{ 10 };
	//A a;
	std::future<void> ff = executor.commit(fun1, 5);
	//std::future<int>  fg = executor.commit(gfun{}, 0);
	auto fg = executor.commit(gfun{}, 0);
	std::future<int> gg = executor.commit(A::Afun, 9999);
	std::future<string> gh = executor.commit(A::Bfun, 9998, "multi args", 123);
	std::future<string> fh = executor.commit([]()->std::string { cout << "the lambda thd : " << this_thread::get_id() << endl; return "fh ret!"; });

	cout << "==============sleep=============" << endl;
	this_thread::sleep_for(std::chrono::microseconds(900));

	ff.get();
	cout << "fg = " << fg.get() << " A::Afun(9999) = " << gg.get() << endl;
	cout << "fh = " << fh.get() << endl;

	ThreadPool::ThreadPool pool{ 4 };
	vector<std::future<int>> results;
	for (int i = 0; i < 8; ++i)
	{
		results.emplace_back(
			pool.commit([i] {
			cout << "hello " << i << endl;
			this_thread::sleep_for(std::chrono::seconds(2));
			cout << "workd " << i << endl;
			return i*i;
		})
		);
	}

	cout << "==============commit all=============" << this_thread::get_id() << endl;

	for (auto&& r : results)
	{
		cout << r.get() << "  ";
	}
	cout << endl;

}