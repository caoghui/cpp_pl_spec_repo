#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>



namespace ThreadPool
{

#define MAX_THREAD_NUM 256
	/*
	�̳߳أ������ύ��κ�����lambda����������ִ�У����Ի�ȡִ�з���ֵ
	��֧�����Ա������֧���ྲ̬��Ա������ȫ�ֺ���
	*/

	std::mutex g_mutex;
	template<class T>
	void print(T& t, bool e = false)
	{
		std::lock_guard<std::mutex> lock{ g_mutex };
		std::cout << " [" << std::this_thread::get_id() << " ] : " << t;
		if (e) std::cout << std::endl;
	}
	class ThreadPool
	{
		using Task = std::function<void()>;
		//�̳߳�
		std::vector<std::thread> _pool;
		//�������
		std::queue<Task> _tasks;
		//ͬ����������
		std::mutex _mutex;
		std::condition_variable _cond;
		//�Ƿ�ر��ύ
		std::atomic<bool> _stoped;
		//�����߳�����
		std::atomic<int> _idlnum;

		//�������

	public:
		inline ThreadPool(unsigned short size = 4) : _stoped{ false }
		{
			_idlnum = size < 1 ? 1 : size;
			for (int i = 0; i < _idlnum; ++i)
			{
				_pool.emplace_back(
					[this] 
					{
						//�����߳�
						while(!this->_stoped)
						{
							std::function<void()> task;
							{
								//��ȡһ����ִ�е�task
								std::unique_lock<std::mutex> lock{this->_mutex}; //unique_lock�ĺô���������ʱunlock() lock()
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
			_cond.notify_all(); //���������߳�ִ��
			for (auto& thd : _pool)
			{
				//method 1      //���߳���������
				//thd.detach(); 
				//method 2      //�ȴ��������
				if (thd.joinable())
				{
					thd.join();
				}
			}
		}
	public:
		/*
		�ύһ������
		����get()��ȡ����ֵ��ȴ�����ִ���꣬��ȡ����ֵ
		�����ַ�������ʵ�ֵ������Ա
		1 ʹ��bind   :  .commit(std::bind(&Dog::say_hello, &dog));
		2 ʹ��mem_fn :  .commit(std::mem_fn(&Dog::say_hello), &dog);
		*/
		template<class F, class... Args>
		auto commit(F&& f, Args... args) -> std::future<decltype(f(args...))>
		{
			if (_stoped.load())
				throw std::runtime_error("commit on ThreadPool is topped.");
			using RetType = decltype(f(args...)); //typename std::result_of<F(Args...)>::type;  ����f�ķ���ֵ����
			auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
			std::future<RetType> future = task->get_future();
			{
				//������񵽶���
				std::lock_guard<std::mutex> lock{_mutex}; //�Ե�ǰ���������
				_tasks.emplace([task]() { (*task)(); });
			}
			_cond.notify_one(); //����һ���߳�
			return future;
		}
		//�����߳�����
		int  idle() { return _idlnum; }
		void stop() { _stoped.store(true); }
	};
}


#endif