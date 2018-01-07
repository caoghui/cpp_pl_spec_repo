#pragma once

#ifndef THREAD_POOL_H_V2
#define THREAD_POOL_H_V2

#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <functional>
#include <condition_variable>

namespace ThreadPoolV2
{
	class ThreadPool;
}

class ThreadPoolV2::ThreadPool
{
	using Task = std::function<void()>;
private:
	//线程池
	std::vector<std::thread> _pool;
	//任务队列
	std::queue<Task> _tasks;
	//同步
	std::mutex _mutex;
	std::condition_variable _cond;
	//是否关闭提交
	std::atomic<bool> _stop;

	//获取一个待执行的任务
	Task get_one_task()
	{
		std::unique_lock<std::mutex> lock{_mutex};
		_cond.wait(lock, [this]() {return !_tasks.empty(); });
		Task task{std::move(_tasks.front())};
		_tasks.pop();
		return task;
	}

	void schedule()
	{
		while (true)
		{
			if (Task task = get_one_task())
			{
				task();
			}
			else
			{
				//done
			}
		}
	}

public:
	//构造函数
	ThreadPool(size_t size = 4) : _stop{ false }
	{
		size = size < 1 ? 1 : size;
		for (size_t i = 0; i < size; ++i)
		{
			_pool.emplace_back(&ThreadPool::schedule, this);
		}
	}
	//析构函数
	~ThreadPool()
	{
		for (auto& thd : _pool)
		{
			//thd.detach();
			thd.join();
		}
	}

	//停止任务提交
	void stop()
	{
		this->_stop.store(true);
	}
	//重启提交
	void restart()
	{
		this->_stop.store(false);
	}
	//提交一个任务
	template<class F, class... Args>
	auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
	{
		if (_stop.load())
		{
			throw std::runtime_error("task executor have closed commit.");
		}
		using ResType = decltype(f(args...));
		auto task = std::make_shared<std::packaged_task<ResType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		{
			std::lock_guard<std::mutex> lock{_mutex};
			_tasks.emplace([task]() {(*task)(); });
		}
		_cond.notify_all();
		std::future<ResType> future = task->get_future();
		return future;
	}
};

#endif
