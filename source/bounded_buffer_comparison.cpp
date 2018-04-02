
#include <boost/thread/condition.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/call_traits.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>
#include <list>
#include <deque>
#include <string>
#include <iostream>

using namespace std;

const unsigned long QUEUE_SIZE = 1000L;
const unsigned long TOTAL_ELEMENTS = QUEUE_SIZE * 1000L;

template<class T>
class bounded_buffer
{
public:
	typedef boost::circular_buffer<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

	explicit bounded_buffer(size_type capacity) : m_unread(0), m_container(capacity) {}

	void push_front(param_type item)
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
	bounded_buffer(const bounded_buffer&);
	bounded_buffer& operator=(const bounded_buffer&);

	bool is_not_empty()const { return m_unread > 0; }
	bool is_not_full() const { return m_unread < m_container.capacity(); }

	size_type m_unread;
	container_type m_container;
	boost::mutex m_mutex;
	boost::condition m_not_empty;
	boost::condition m_not_full;
};

template<class T>
class bounded_buffer_space_optimized
{
public:
	typedef boost::circular_buffer_space_optimized<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

	explicit bounded_buffer_space_optimized(size_type capacity) : m_container(capacity) {}

	void push_front(param_type item)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_full.wait(lock, boost::bind(&bounded_buffer_space_optimized<value_type>::is_not_full, this));
		m_container.push_front(item);
		lock.unlock();
		m_not_empty.notify_one();
	}

	void pop_back(value_type* pItem)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_empty.wait(lock, boost::bind(&bounded_buffer_space_optimized<value_type>::is_not_empty, this));
		*pItem = m_container.back();
		m_container.pop_back();
		lock.unlock();
		m_not_full.notify_one();
	}

private:
	bounded_buffer_space_optimized(const bounded_buffer_space_optimized&);
	bounded_buffer_space_optimized& operator=(const bounded_buffer_space_optimized&);

	bool is_not_empty() const { return m_container.size() > 0; }
	bool is_not_full() const { return m_container.size() < m_container.capacity(); }

	container_type m_container;
	boost::mutex m_mutex;
	boost::condition m_not_empty;
	boost::condition m_not_full;
};

template<class T>
class bounded_buffer_deque_based
{
public:
	typedef std::deque<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

	explicit bounded_buffer_deque_based(size_type capacity) : m_capacity(capacity) {}

	void push_front(param_type item)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_full.wait(lock, boost::bind(&bounded_buffer_deque_based<value_type>::is_not_full, this));
		m_container.push_front(item);
		lock.unlock();
		m_not_empty.notify_one();
	}

	void pop_back(value_type* pItem)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_empty.wait(lock, boost::bind(&bounded_buffer_deque_based<value_type>::is_not_empty, this));
		*pItem = m_container.back();
		m_container.pop_back();
		lock.unlock();
		m_not_full.notify_one();
	}

private:
	bounded_buffer_deque_based(const bounded_buffer_deque_based&);
	bounded_buffer_deque_based& operator=(const bounded_buffer_deque_based&);

	bool is_not_empty() const { return m_container.size() > 0; }
	bool is_not_full() const { return m_container.size() < m_capacity; }

	const size_type m_capacity;
	container_type m_container;
	boost::mutex m_mutex;
	boost::condition m_not_empty;
	boost::condition m_not_full;
};

template<class T>
class bounded_buffer_list_based
{
public:
	typedef std::list<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

	explicit bounded_buffer_list_based(size_type capacity) : m_capacity(capacity) {}

	void push_front(param_type item)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_full.wait(lock, boost::bind(&bounded_buffer_list_based<value_type>::is_not_full, this));
		m_container.push_front(item);
		lock.unlock();
		m_not_empty.notify_one();
	}

	void pop_back(value_type* pItem)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_empty.wait(lock, boost::bind(&bounded_buffer_list_based<value_type>::is_not_empty, this));
		*pItem = m_container.back();
		m_container.pop_back();
		lock.unlock();
		m_not_full.notify_one();
	}

private:
	bounded_buffer_list_based(const bounded_buffer_list_based&);
	bounded_buffer_list_based& operator=(const bounded_buffer_list_based&);

	bool is_not_empty() const { return m_container.size() > 0; }
	bool is_not_full() const { return m_container.size() < m_capacity; }

	const size_type m_capacity;
	container_type m_container;
	boost::mutex m_mutex;
	boost::condition m_not_empty;
	boost::condition m_not_full;
};

template <class Buffer>
class Consumer
{
	typedef typename Buffer::value_type value_type;
	Buffer* m_container;
	value_type m_item;
public:
	Consumer(Buffer* buffer) :m_container(buffer) {}

	void operator()()
	{
		for (unsigned long i = 0L; i < TOTAL_ELEMENTS; ++i)
		{
			m_container->pop_back(&m_item);
		}
	}
};

template <class Buffer>
class Producer
{
	typedef typename Buffer::value_type value_type;
	Buffer* m_container;

public:
	Producer(Buffer* buffer) : m_container(buffer) {}

	void operator()()
	{
		for (unsigned long i = 0L; i < TOTAL_ELEMENTS; ++i)
		{
			m_container->push_front(value_type());
		}
	}
};

template <class Buffer>
void fifo_test(Buffer* buffer)
{
	//start of measurement
	boost::progress_timer progress;
	for (unsigned long i = QUEUE_SIZE / 2L; i > 0; --i)
	{
		buffer->push_front(BOOST_DEDUCED_TYPENAME Buffer::value_type());
	}

	Consumer<Buffer> consumer(buffer);
	Producer<Buffer> producer(buffer);

	boost::thread consume(consumer);
	boost::thread produce(producer);

	//wait for completion
	consume.join();
	produce.join();
}

int main(int argc, char** argv)
{
	bounded_buffer<int> bb_int(QUEUE_SIZE);
	std::cout << "bounded_buffer<int> :" << std::endl;
	fifo_test(&bb_int);

	bounded_buffer_space_optimized<int> bb_space_optimized_int(QUEUE_SIZE);
	cout << "bounded_buffer_space_optimized<int> :" << endl;
	fifo_test(&bb_space_optimized_int);

	bounded_buffer_deque_based<int> bb_deque_based_int(QUEUE_SIZE);
	cout << "bounded_buffer_deque_based<int> :" << endl;
	fifo_test(&bb_deque_based_int);

	bounded_buffer_list_based<int> bb_list_based_int(QUEUE_SIZE);
	cout << "bounded_buffer_list_based<int> :" << endl;
	fifo_test(&bb_list_based_int);


	bounded_buffer<string> bb_str(QUEUE_SIZE);
	cout << "bounded_buffer<string> :" << endl;
	fifo_test(&bb_str);

	bounded_buffer_space_optimized<string> bb_space_optimized_str(QUEUE_SIZE);
	cout << "bounded_buffer_space_optimized<string> :" << endl;
	fifo_test(&bb_space_optimized_str);

	bounded_buffer_deque_based<string> bb_deque_based_str(QUEUE_SIZE);
	cout << "bounded_buffer_deque_based<string> :" << endl;
	fifo_test(&bb_deque_based_str);

	bounded_buffer_list_based<string> bb_list_based_str(QUEUE_SIZE);
	cout << "bounded_buffer_list_based<string> :" << endl;
	fifo_test(&bb_list_based_str);

	return 0;
}