#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>



namespace ThreadPool
{

#define MAX_THREAD_NUM 256
	/*
	线程池，可以提交变参函数或lambda的匿名函数执行，可以获取执行返回值
	不支持类成员函数，支持类静态成员函数或全局函数
	*/

	std::mutex g_mutex;
	template<class T>
	void print(T& t, bool e = false)
	{
		std::lock_guard<std::mutex> lock{ g_mutex };
		std::cout << " [" << this_thread::get_id() << " ] : " << t;
		if (e) std::cout << endl;
	}
	class ThreadPool
	{
		using Task = std::function<void()>;
		//线程池
		std::vector<std::thread> _pool;
		//任务队列
		std::queue<Task> _tasks;
		//同步：互斥锁
		std::mutex _mutex;
		std::condition_variable _cond;
		//是否关闭提交
		std::atomic<bool> _stoped;
		//空闲线程数量
		std::atomic<int> _idlnum;

		//任务调度

	public:
		inline ThreadPool(unsigned short size = 4) : _stoped{ false }
		{
			_idlnum = size < 1 ? 1 : size;
			for (int i = 0; i < _idlnum; ++i)
			{
				_pool.emplace_back(
					[this] 
					{
						//工作线程
						while(!this->_stoped)
						{
							std::function<void()> task;
							{
								//获取一个待执行的task
								std::unique_lock<std::mutex> lock{this->_mutex}; //unique_lock的好处：可以随时unlock() lock()
								print(" running....\n");
								this->_cond.wait(lock, [this] {return this->_stoped.load() || !this->_tasks.empty(); });
								if (this->_stoped && this->_tasks.empty())
								{
									print("none task , thread exit\n");
									return;
								}
									
								task = std::move(this->_tasks.front());
								this->_tasks.pop();
							}
							_idlnum--;
							print(" handle task...\n");
							task();
							_idlnum++;
						}
					}				
				);
			}
		}
		inline ~ThreadPool()
		{
			_stoped.store(true);
			_cond.notify_all(); //唤醒所有线程执行
			for (auto& thd : _pool)
			{
				//method 1      //让线程自生自灭
				//thd.detach(); 
				//method 2      //等待任务结束
				if (thd.joinable())
				{
					thd.join();
				}
			}
		}
	public:
		/*
		提交一个任务
		调用get()获取返回值会等待任务执行完，获取返回值
		有两种方法可以实现调用类成员
		1 使用bind   :  .commit(std::bind(&Dog::say_hello, &dog));
		2 使用mem_fn :  .commit(std::mem_fn(&Dog::say_hello), &dog);
		*/
		template<class F, class... Args>
		auto commit(F&& f, Args... args) -> std::future<decltype(f(args...))>
		{
			if (_stoped.load())
				throw std::runtime_error("commit on ThreadPool is topped.");
			using RetType = decltype(f(args...)); //typename std::result_of<F(Args...)>::type;  函数f的返回值类型
			auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
			std::future<RetType> future = task->get_future();
			{
				//添加任务到队列
				std::lock_guard<std::mutex> lock{_mutex}; //对当前块的语句加锁
				_tasks.emplace([task]() { (*task)(); });
			}
			_cond.notify_one(); //唤醒一个线程
			return future;
		}
		//空闲线程数量
		int  idle() { return _idlnum; }
		void stop() { _stoped.store(true); }
	};
}


#endif