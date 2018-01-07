#pragma once

#ifndef THREAD_POOL_H_V1
#define THREAD_POOL_H_V1

#include <mutex>
#include <queue>
#include <vector>
#include <memory>
#include <thread>
#include <future>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <condition_variable>

std::mutex g_mutex;

template<class T>
void print(T& t)
{
	std::lock_guard<std::mutex> lock{ g_mutex };
	std::cout << " [" << this_thread::get_id << " ] : " << t;
}

namespace ThreadPoolV1
{
	class ThreadPool
	{
	public:
		ThreadPool(size_t);
		template<class F, class... Args>
		auto enqueue(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
		{
			using return_type = typename std::result_of<F(Args...)>::type;
			auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
			std::future<return_type> res = task->get_future();
			{
				std::unique_lock<std::mutex> lock{ _mutex };
				if (_stop)
				{
					throw std::runtime_error("enqueue on stopped ThreadPool");
				}
				_tasks.emplace([task]() { (*task)(); });
			}
			_cond.notify_one();
			return res;
		}
		~ThreadPool();
	private:
		//need to keep track of threads so we can join them
		std::vector<std::thread> _workers;
		//the task queue
		std::queue<std::function<void()>> _tasks;
		//synchronization
		std::mutex _mutex;
		std::condition_variable _cond;
		bool _stop;
	};
}

inline ThreadPoolV1::ThreadPool::ThreadPool(size_t threads) : _stop{false}
{
	for (size_t i = 0; i < threads; ++i)
	{
		_workers.emplace_back(
			[this]
			{
				while(true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock{ this->_mutex };
						
						this->_cond.wait(lock, [this] { return this->_stop || !this->_tasks.empty(); });
						print(" a thread is running...\n");
						if (this->_stop && this->_tasks.empty())
							return;
						task = std::move(this->_tasks.front());
						this->_tasks.pop();
					}
					task();
				}
			}
		);
	}
}

inline ThreadPoolV1::ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock{_mutex};
		_stop = true;
	}
	_cond.notify_all();
	for (auto& thd : _workers)
	{
		thd.join();
	}
}

/*
template<class F, class...Args>
auto ThreadPoolV1::ThreadPool::enqueue(F&& f, Args... args) ->std::future<typename std::result_of<F(Args...)>::type>
{
using return_type = typename std::result_of<F(Args...)>::type;
auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
std::future<return_type> res = task->get_future();
{
std::unique_lock<std::mutex> lock{_mutex};
if (stop)
{
throw std::runtime_error("enqueue on stopped ThreadPool");
}
_tasks.emplace([task]() { (*task)(); });
}
_cond.notify_one();
return res;
}
*/

#endif
