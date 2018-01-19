#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <future>
#include <functional>
#include <condition_variable>

using namespace std;

mutex mtx;
condition_variable cond;
bool ready = false;
int  ready_cnt = 0;

void print_id(int id)
{
	unique_lock<mutex> lock(mtx);
	while (!ready)
	{
		cout << "thread [" << this_thread::get_id() << "] waiting..." << id << endl;
		ready_cnt++;
		cond.wait(lock);
	}
	cout << "thread [" << this_thread::get_id() << "] id = " << id << endl;
}

void print_int(future<int>& fut)
{
	int x = fut.get();
	cout << "thread:[" << this_thread::get_id() << "] value : " << x << endl;
}

void go()
{
	unique_lock<mutex> lock(mtx);
	cond.wait(lock, [] { return ready_cnt == 10; });
	ready = true;
	cond.notify_all();
}

int countdown(int from, int to)
{
	for (int i = from; i != to; --i)
	{
		cout << i << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
	cout << "Lift off!" << endl;
	return from - to;
}

bool is_prime(int x)
{
	for (int i = 2; i < x; ++i)
	{
		if (x % i == 0)
			return false;
	}
	return true;
}

void test_cond();
void test_future();
void test_promise();
void test_packaged();

int main(int argc, char** argv)
{
	//test_future();
	//test_promise();
	test_packaged();

	return 0;
}

void test_packaged()
{
	packaged_task<int(int, int)> task(countdown);
	future<int> ret = task.get_future();
	thread thd(move(task), 10, 0);
	int value = ret.get();
	cout << "the countdown lasted for " <<value << " seconds." << endl;
	thd.join();
}

void test_promise()
{
	promise<int> prom;
	future<int> fut = prom.get_future();
	thread thd(print_int, ref(fut));
	prom.set_value(10);
	thd.join();
}

void test_future()
{
	int n = 700020007;
	future<bool> fut = async(is_prime, n);
	cout << "checking, please wait..." << endl;
	chrono::milliseconds span(100);
	while (fut.wait_for(span) == future_status::timeout)
		cout << ".";

	bool x = fut.get();

	cout << n << (x ? "is" : " is not") << " prime" << endl;
}

void test_cond()
{
	cout << "Test std::condition_variable" << endl;

	thread threads[10];
	for (int i = 0; i < 10; ++i)
	{
		threads[i] = thread(print_id, i);
	}

	cout << "10 threads ready to race ...." << endl;
	this_thread::sleep_for(std::chrono::seconds(5));
	go();

	for (auto& thd : threads)
	{
		thd.join();
	}
}