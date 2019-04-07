/*************************************************************************
	> File Name: chapter_10.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年03月28日 星期二 14时14分01秒
 ************************************************************************/

#include <string>
#include <iostream>
#include <ostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <cassert>
#include <algorithm>
#include <functional>

using namespace std;

void PrettyFormat(int i, string& s)
{
	ostringstream temp;
	
	temp << i;
	s = temp.str();
}

void hello()
{
	while (1)
	{
		cout << "Hello from thread" << this_thread::get_id() << endl;
		_sleep(1000);
	}
		
}

template<typename T>
class Buffer
{
	string _name;
	size_t _size;
	unique_ptr<T[]> _buffer;
public:
	Buffer(): _size(16), _buffer(new T[16]) {}
	Buffer(const string& name, size_t size) : _name(name), _size(size), _buffer(new T[size]) {}
	Buffer(const Buffer& copy) :_name(copy._name), _size(copy._size), _buffer(new T[copy._size])
	{
		T* source = copy._buffer.get();
		T* dest = _buffer.get();
		std::copy(source, source + copy._size, dest);
	}
	Buffer& operator=(const Buffer& copy)
	{
		if (this != &copy)
		{
			_name = copy._name;
			if (_size != copy._size)
			{
				_buffer = nullptr;
				_size = copy._size;
				_buffer = _size > 0 ? new T[_size] : nullptr;
			}
			T* source = copy._buffer.get();
			T* dest = _buffer.get();
			std::copy(source, source + copy._size, dest);
		}
		return *this;
	}
	Buffer(Buffer&& temp) : _name(move(temp._name)), _size(temp._size), _buffer(move(temp._buffer))
	{
		temp._buffer = nullptr;
		temp._size = 0;
	}
	Buffer& operator=(Buffer&& temp)
	{
		assert(this != &temp);
		_buffer = nullptr;
		_size = temp._size;
		_buffer = move(temp._buffer);
		_name = move(temp._name);
		temp._buffer = nullptr;
		temp._size = 0;
		return *this;
	}
};

template<typename T>
Buffer<T> getBuffer(const string& name)
{
	Buffer<T> b(name, 128);
	return b;
}

template<typename T, size_t Size>
class Vector
{
	static_assert(Size < 52, "Size is too small");
	T _points[Size];
};

template<typename T1, typename T2>
auto add(T1 t1, T2 t2) -> decltype(t1 + t2)
{
	return t1 + t2;
}

void test_thread();
void test_array();
void test_smartptr();
void test_lambda();
void test_buff();

int main(int argc, char** argv)
{
	//test_array();
	//test_smartptr();
	//test_lambda();
	test_buff();

    return 0;
}

void test_buff()
{
	Buffer<int> b1;
	Buffer<int> b2("buf2", 64);
	Buffer<int> b3 = b2;
	Buffer<int> b4 = getBuffer<int>("buf4");
	b1 = getBuffer<int>("buf5");
}

void test_lambda()
{
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	for_each(begin(v), end(v), [](int n) { cout << n << endl; });

	auto is_odd = [](int n) { return n % 2 == 1; };
	auto pos = find_if(v.begin(), v.end(), is_odd);
	if (pos != v.end())
	{
		cout << *pos << endl;
	}

	function<int(int)> fib = [&fib](int n) { return n < 2 ? 1 : fib(n - 1) + fib(n - 2); };

	cout << "fib = " << fib(4) << endl;

	Vector<int, 16> v1;
	//Vector<double, 2> v2;
}



void test_smartptr()
{

}

void test_array()
{
	int arr[] = {1,2,3,4,5,6};
	for (int& e : arr)
		cout << e << " = " << e * e << endl;
}

void test_thread()
{
	vector<thread> threads;
	for (int i = 0; i < 5; ++i)
	{
		threads.push_back(thread([]()
		{
			this_thread::sleep_for(chrono::seconds(1));
			cout << "hello from " << this_thread::get_id() << endl;
		}));
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
}