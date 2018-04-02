#include <iostream>
#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer/timer.hpp>

template <class T>
class bounded_buffer
{
public:
	typedef boost::circular_buffer<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

	explicit bounded_buffer(size_type capacity) : m_unread(0), m_container(capacity) {}

	void push_front(typename boost::call_traits<value_type>::param_type item)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_full.wait(lock, boost::bind(&bounded_buffer<value_type>::is_not_full, this));
		m_container.push_front(item);
		++m_unread;
		lock.unlock();
		m_not_empty.notify_one();
	}

	void pop_back(value_type* pItem)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_empty.wait(lock, boost::bind(&bounded_buffer<value_type>::is_not_empty, this));
		*pItem = m_container[--m_unread];
		lock.unlock();
		m_not_full.notify_one();
	}

private:
	bounded_buffer(const bounded_buffer&);  //disabled copy constructor
	bounded_buffer& operator =(const bounded_buffer&);  // disabled assign operator

	bool is_not_empty() const { return m_unread > 0; }
	bool is_not_full() const { return m_unread < m_container.capacity(); }
	size_type m_unread;
	container_type m_container;
	boost::mutex m_mutex;
	boost::condition m_not_empty;
	boost::condition m_not_full;
};

const unsigned long queue_size = 1000L;
const unsigned long total_elements = queue_size * 1000L;

template<class Buffer>
class Producer
{
	typedef typename Buffer::value_type value_type;
	Buffer* m_container;

public:
	Producer(Buffer* buffer) : m_container(buffer)
	{}

	void operator()()
	{
		for (unsigned long i = 0L; i < total_elements; ++i)
		{
			m_container->push_front(value_type());
		}
	}
};

template<class Buffer>
class Consumer
{
	typedef typename Buffer::value_type value_type;
	Buffer* m_container;
	value_type m_item;

public:
	Consumer(Buffer* buffer) : m_container(buffer)
	{}

	void operator()()
	{
		for (unsigned long i = 0L; i < total_elements; ++i)
		{
			m_container->pop_back(&m_item);
		}
	}
};

template<class Buffer>
void fifo_test(Buffer* buffer)
{
	//start of timing
	boost::timer::auto_cpu_timer progress;
	//Initialize the buffer with some values before launching producer and consumer threads
	for (unsigned long i = queue_size / 2L; i > 0; --i)
	{
		buffer->push_front(BOOST_DEDUCED_TYPENAME Buffer::value_type());
	}

	//construct the threads
	Consumer<Buffer> consumer(buffer);
	Producer<Buffer> producer(buffer);

	//start the threads
	boost::thread consume(consumer);
	boost::thread produce(producer);

	//wait for completion
	consume.join();
	produce.join();

	//End of timing
	//destructor of boost::timer::auto_cpu_timer will output the time to std::out

}

int main(int argc, char** argv)
{
	//construct a bounded_buffer to hold the chosen type, here int
	bounded_buffer<int> bb_int(queue_size);
	std::cout << "Testing bounded_buffer<int> " << std::endl;
	//start the fifo test
	fifo_test(&bb_int);
	//destructor of boost::timer::auto_cpu_timer will output the time to std::out

	return 0;
}