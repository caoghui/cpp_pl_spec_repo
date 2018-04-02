#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/asio/streambuf.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;

io_service service;

//example1
void on_read(boost::asio::streambuf& buf, const boost::system::error_code&, size_t)
{
	istream in(&buf);
	string line;
	getline(in, line);
	cout << "first line: " << line << endl;
}

void test_files()
{
	HANDLE file = ::CreateFile("readme.txt", GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
	windows::stream_handle h(service, file);
	boost::asio::streambuf buf;
	async_read(h, buf, transfer_exactly(256), boost::bind(on_read, boost::ref(buf), _1, _2));
	service.run();
}

//example2
void func(int i)
{
	cout << "func called, i = " << i << "/" << boost::this_thread::get_id() << endl;
}

void worker_thread()
{
	service.run();
}

void test_post()
{
	for (int i = 0; i < 10; ++i)
	{
		service.post(boost::bind(func, i));
	}
	boost::thread_group threads;
	for (int i = 0; i < 3; ++i)
	{
		threads.create_thread(worker_thread);
	}
	//等待所有线程被创建完
	boost::this_thread::sleep(boost::posix_time::microsec(500));
	threads.join_all();
}

//example3
void test_post2()
{
	io_service::strand strand_one(service);
	io_service::strand strand_two(service);
	for (int i = 0; i < 5; ++i)
	{
		service.post(strand_one.wrap(boost::bind(func, i)));
	}
	for (int i = 5; i < 10; ++i)
	{
		service.post(strand_two.wrap(boost::bind(func, i)));
	}
	boost::thread_group threads;
	for (int i = 0; i < 3; ++i)
	{
		threads.create_thread(worker_thread);
	}
	//等待所有线程被创建完
	boost::this_thread::sleep(boost::posix_time::microsec(500));
	threads.join_all();
}

void run_dispatch_and_post()
{
	for (int i = 0; i < 10; i += 2)
	{
		service.dispatch(boost::bind(func, i));
		service.post(boost::bind(func, i + 1));
	}
}

void test_post3()
{
	service.post(run_dispatch_and_post);
	service.run();
}

/////////////////////////////////////////////////////
void dispatched_func_1()
{
	cout << "dispatched 1" << endl;
}

void dispatched_func_2()
{
	cout << "dispatched 2" << endl;
}

void test(boost::function<void()> func)
{
	cout << "test" << endl;
	service.dispatch(dispatched_func_1);
	func();
}

void service_run()
{
	service.run();
}

void test_service()
{
	test(service.wrap(dispatched_func_2));
	boost::thread th(service_run);
	boost::this_thread::sleep(boost::posix_time::microsec(500));
	th.join();
}

///////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	//service::post
	//test_post();
	//test_post2();
	//test_post3();
	test_service();


	return 0;
}