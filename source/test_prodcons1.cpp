//#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <condition_variable>

using namespace std;

static const int MAX_BUFF_SIZE = 10; //Item buffer size
static const int MAX_ITEM_TO_PRODUCE = 100;   //How many items we plan to produce

struct ItemRepository
{
	int _buffer[MAX_BUFF_SIZE];  //产品缓冲区
	size_t _read_pos;            //消费者读取位置
	size_t _write_pos;           //生产者写入位置
	size_t _counter;
	mutex  _mutex;
	mutex  _mutex_counter;
	condition_variable _cond_not_full;
	condition_variable _cond_not_empty;
}gRepo;

typedef struct ItemRepository IRepo;

void produce(IRepo* repo, int item)
{
	unique_lock<mutex> lock(repo->_mutex);
	while (((repo->_write_pos + 1) % MAX_BUFF_SIZE) == repo->_read_pos)  //buffer is full ,wait
	{
		cout << "producer is waiting for an empty slot..." << endl;
		repo->_cond_not_full.wait(lock);
	}
	repo->_buffer[repo->_write_pos] = item;
	++(repo->_write_pos);

	if (repo->_write_pos == MAX_BUFF_SIZE)
		repo->_write_pos = 0;
	repo->_cond_not_empty.notify_all(); //通知消费者
}

int consume(IRepo* repo)
{
	int data;
	unique_lock<mutex> lock(repo->_mutex);
	while (repo->_read_pos == repo->_write_pos)  //buffer is empty
	{
		cout << "consumer is waiting for items..." << endl;
		repo->_cond_not_empty.wait(lock);
	}
	data = repo->_buffer[repo->_read_pos];
	++repo->_read_pos;
	if (repo->_read_pos >= MAX_BUFF_SIZE)
		repo->_read_pos = 0;
	repo->_cond_not_full.notify_all();
	return data;
}

void producer_task()
{
	for (int i = 1; i < MAX_ITEM_TO_PRODUCE; ++i)
	{
		++gRepo._counter;
		cout << "produce the [" << i << "] item" << endl;
		produce(&gRepo, i);
	}
	cout << "producer thread [" << this_thread::get_id() << "] is exiting... " << endl;
}

void consumer_task()
{
	bool ready_to_exit = false;
	while (true)
	{
		this_thread::sleep_for(std::chrono::seconds(1));
		if (gRepo._counter < MAX_ITEM_TO_PRODUCE)
		{
			int data = consume(&gRepo);
			cout << "consumer thread [" << this_thread::get_id() << "] " << "consume the [" << data << "] item" << endl;
		}
		else
		{
			ready_to_exit = true;
		}
		
		if (ready_to_exit == true)
			break;
	}
	cout << "consumer thread [" << this_thread::get_id() << "] is exiting...." << endl;
}

int main(int argc, char** argv)
{
	gRepo._read_pos = 0;
	gRepo._write_pos = 0;
	gRepo._counter = 0;

	thread producer(producer_task);
	thread consumer1(consumer_task);
	thread consumer2(consumer_task);
	thread consumer3(consumer_task);

	producer.join();
	consumer1.join();
	consumer2.join();
	consumer3.join();

	return 0;
}