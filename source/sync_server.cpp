#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

using namespace std;
using namespace boost::asio;

io_service service;
ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);

class talk_to_client;

typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> Array;
Array clients;
boost::recursive_mutex cs; 

class talk_to_client : boost::enable_shared_from_this<talk_to_client>
{
	enum { max_msg = 1024 };
	char buff_[1024];
	int already_read_;
	bool clients_changed_;
	boost::posix_time::ptime last_ping;
	string username_;
	ip::tcp::socket sock_;

public:

	talk_to_client() { }

	string username() const { return username_; }
	void set_clients_changed() { clients_changed_ = true; }
	ip::tcp::socket& sock() { return sock_; }

	void answer_to_client()
	{
		try
		{
			read_request();
			process_request();
		}
		catch (boost::system::system_error&)
		{
			stop();
		}
		if (timed_out())
			stop();
	}

	bool timed_out() const
	{
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		long long ms = (now - last_ping).total_milliseconds();
		return ms > 5000;
	}

	void stop()
	{
		boost::system::error_code err;
		sock_.close(err);
	}

	void read_request()
	{
		if (sock_.available())
		{
			already_read_ += sock_.read_some(buffer(buff_ + already_read_, max_msg - already_read_));
		}
	}

	void process_request()
	{
		bool found_enter = std::find(buff_, buff_ + already_read_, '\n') < buff_ + already_read_;
		if (!found_enter)
			return;
		//处理消息

		last_ping = boost::posix_time::microsec_clock::local_time();
		size_t pos = std::find(buff_, buff_ + already_read_, '\n') - buff_;
		string msg(buff_, pos);
		std::copy(buff_ + already_read_, buff_ + max_msg, buff_);
		already_read_ -= pos + 1;
		if (msg.find("login") == 0)
			on_login(msg);
		else if (msg.find("ping") == 0)
			on_ping();
		else if (msg.find("ask_clients") == 0)
			on_clients();
		else
			cerr << "invalid msg " << msg << endl;
	}

	void on_login(const string& msg)
	{
		std::istringstream in(msg);
		in >> username_ >> username_;
		write("login ok\n");
		update_clients_changed();
	}

	void on_ping()
	{
		write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
		clients_changed_ = false;
	}

	void on_clients()
	{
		std::string msg;
		{
			boost::recursive_mutex::scoped_lock lock(cs);
			for (Array::const_iterator b = clients.begin(), e = clients.end(); b != e; ++b)
			{
				msg += (*b)->username() + " ";
			}
		}
		write("clients " + msg + "\n");
	}

	void write(const string& msg)
	{
		sock_.write_some(buffer(msg));
	}

	void update_clients_changed()
	{

	}

};



void accept_thread()
{
	ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
	while (true)
	{
		client_ptr new_(new talk_to_client);
		acceptor.accept(new_->sock());
		boost::recursive_mutex::scoped_lock lock(cs);
		clients.push_back(new_);
	}
}

void handle_clients_thread()
{
	while (true)
	{
		boost::this_thread::sleep(boost::posix_time::millisec(1));
		boost::recursive_mutex::scoped_lock lock(cs);
		for (Array::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
		{
			(*b)->answer_to_client();
		}
		clients.erase(std::remove_if(clients.begin(), clients.end(), boost::bind(&talk_to_client::timed_out, _1)), clients.end());
	}
}

int main(int argc, char** argv)
{
	boost::thread_group threads;
	threads.create_thread(accept_thread);
	threads.create_thread(handle_clients_thread);
	threads.join_all();

	return 0;
}