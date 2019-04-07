#include <iostream>
#include <string>
#include <thread>
#include <functional>
#include <future>
#include <chrono>
#include <memory>

using namespace std;

void test_promise();
void test_packaged_task();

int main(int argc, char** argv)
{
	//test_promise();
	test_packaged_task();

	return 0;
}

int* func(promise<int>& pro)
{
	this_thread::sleep_for(chrono::seconds(1));
	pro.set_value(8);
	int* ptr = new int(10);
	cout << "" << ptr << endl;
	return ptr;
}

int func2(int from, int to)
{
	this_thread::sleep_for(chrono::seconds(3));
	return (from - to);
}

void test_packaged_task()
{
	std::packaged_task<int(int, int)> task(func2);
	future<int> ret = task.get_future();

	thread t(move(task), 10, 5);
	cout << ret.get() << endl;
	t.join();

	promise<int> pro;
	packaged_task<int*(promise<int>&)> task2(func);
	future<int*> ret2 = task2.get_future();

	//thread thd2(move(task2), ref(pro));
	thread(move(task2), ref(pro)).detach();
	
	chrono::milliseconds span(100);
	while (ret2.wait_for(span) == future_status::timeout)
		cout << ".";

	shared_ptr<int> sfptr(ret2.get());
	cout << "addr = " << sfptr.get() << " value = " << *sfptr << " " << pro.get_future().get() << endl;
	
	//thd2.join();
	
}

void test_promise()
{
	promise<int> pro;
	
	thread t(func, std::ref(pro));

	future<int> fut = pro.get_future();
	cout << fut.get() << endl;
	t.join();

}