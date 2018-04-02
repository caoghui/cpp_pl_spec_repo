#include <iostream>
#include <string>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace boost::asio;

io_service service;
ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);


struct talk_to_server
{
private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024};
	int already_read_;
	char buff_[max_msg];
	bool started_;
	string username_;

public:

	talk_to_server(const string& username) : sock_(service), started_(true), username_(username) {}
	
	void connect(ip::tcp::endpoint ep)
	{
		sock_.connect(ep);
	}

	void loop()
	{
		write("login " + username_ + "\n");
		read_answer();
		while (started_)
		{
			write_request();
			read_answer();
			boost::this_thread::sleep(boost::posix_time::millisec(rand() % 7000));
		}
	}

	void write_request()
	{
		write("ping\n");
	}

	void read_answer()
	{
		already_read_ = 0;
		read(sock_, buffer(buff_), boost::bind(&talk_to_server::read_complete, this, _1, _2));
		process_msg();
	}

	void process_msg()
	{
		string msg(buff_, already_read_);
		if (msg.find("login ") == 0)
			on_login();
		else if (msg.find("ping") == 0)
			on_ping(msg);
		else if (msg.find("clients ") == 0)
			on_clients(msg);
		else
			cerr << "invalid msg " << msg << endl;
	}

	void on_login() { do_ask_clients(); }
	void on_ping(const string& msg)
	{
		std::istringstream in(msg);
		std::string answer;
		in >> answer >> answer;
		if (answer == "client_list_changed")
			do_ask_clients();
	}
	void on_clients(const std::string& msg)
	{
		std::string clients = msg.substr(8);
		std::cout << username_ << ", new client list :" << clients;
	}

	void do_ask_clients()
	{
		write("ask_clients\n");
		read_answer();
	}

	void write(const std::string& msg)
	{
		sock_.write_some(buffer(msg));
	}

	size_t read_complete(const boost::system::error_code& err, size_t byte)
	{
		return 0;
	}

	string username() const { return username_; }
};

void run_client(const string& client_name)
{
	talk_to_server client(client_name);
	try
	{
		client.connect(ep);
		client.loop();
	}
	catch (boost::system::system_error& err)
	{
		cout << "client terminated " << endl;
	}
}

int main(int argc, char** argv)
{


	return 0;
}
