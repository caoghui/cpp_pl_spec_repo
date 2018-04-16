#include <iostream>
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <functional>

using namespace std;

class Server
{
private:
	shared_mutex m_rwLock;
	vector<int> m_clients;
public:
	void broadcast(const void* msg, size_t len)
	{
		shared_lock<shared_mutex> lock(m_rwLock);
		for (int fd : m_clients)
		{
			send(fd, msg, len, 0);
		}		
	}
	void add_client(int fd)
	{
		unique_lock<shared_mutex> exclusive(m_rwLock);
		m_clients.push_back(fd);
	}
	void send(int fd, const void* msg, size_t len, int)
	{

	}
};

class Server2
{
private:
	struct ClientList
	{
		vector<int> clients;
	};
	atomic<ClientList*> m_currentList;

public:
	void broadcast(const void* msg, size_t len)
	{
		ClientList* list = m_currentList.load();
		for (int fd : list->clients)
		{
			send(fd, msg, len);
		}
	}

	void send(int fd, const void* msg, size_t len)
	{

	}

	void add_client(int fd)
	{
		ClientList* oldList = m_currentList.load();
		ClientList* newList = new ClientList{*oldList};
		newList->clients.push_back(fd);
		m_currentList.store(newList);
	}
};

class MemoryReclaimer
{
private:
	mutex m_mutex;
	vector<bool> m_threadWasQuiescent;
	vector<function<void()>> m_currentIntervalCallbacks;
	vector<function<void()>> m_previousIntervalCallbacks;
	size_t m_numRemaining = 0;

public:
	typedef size_t ThreadIndex;
	ThreadIndex registerThread()
	{
		lock_guard<mutex> guard(m_mutex);
		ThreadIndex id = m_threadWasQuiescent.size();
		m_threadWasQuiescent.push_back(false);
		m_numRemaining++;
		return id;
	}
	void addCallback(const function<void()>& callback)
	{
		lock_guard<mutex> guard(m_mutex);
		m_currentIntervalCallbacks.push_back(callback);
	}

	void onQuiescentState(ThreadIndex id)
	{
		lock_guard<mutex> guard(m_mutex);
		if (!m_threadWasQuiescent[id])
		{
			m_threadWasQuiescent[id] = true;
			m_numRemaining--;
			if (m_numRemaining == 0)
			{
				//end of interval, Invoke all callbacks from the previous interval
				for (const auto& callback : m_previousIntervalCallbacks)
				{
					callback();
				}
				//move current callbacks to previous interval
				m_previousIntervalCallbacks = move(m_currentIntervalCallbacks);
				m_currentIntervalCallbacks.clear();

				//reset all thread sstatus
				for (size_t i = 0; i < m_threadWasQuiescent.size(); ++i)
				{
					m_threadWasQuiescent[i] = false;
				}
				m_numRemaining = m_threadWasQuiescent.size();
			}
		}
	}
};

int main(int argc, char** argv)
{


	return 0;
}