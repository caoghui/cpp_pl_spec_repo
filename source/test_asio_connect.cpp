#include <string>
#include <iostream>
#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace boost::asio;

io_service service;

struct connection : boost::enable_shared_from_this<connection>
{
private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024};
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<connection> ptr;

	connection() :sock_(service), started_(true) {}

	void start(ip::tcp::endpoint ep)
	{
		sock_.async_connect(ep, boost::bind(&connection::on_connect, shared_from_this(), _1));
	}

	void stop()
	{
		if (!started_)
			return;
		started_ = false;
		sock_.close();
	}
	bool started() { return started_; }

private:
	void on_connect(const error_code& err)
	{
		//��������������������ʲô����ȡ��д��
		if (!err)
			do_read();
		else
			stop();
	}

	void on_read(const error_code& err, size_t bytes)
	{
		if (!started())
			return;
		string msg(read_buffer_, bytes);
		if (msg == "can_login")
			do_write("access_data");
		else if (msg.find("data") == 0)
			process_data(msg);
		else if (msg == "login_fail")
			stop();
	}

	void on_write(const error_code& err, size_t bytes)
	{
		do_read();
	}

	void do_read()
	{
		sock_.async_read_some(buffer(read_buffer_), boost::bind(&connection::on_read, shared_from_this(), _1, _2));
	}

	void do_write(const string& msg)
	{
		if (!started())
			return;
		//ע�⣺��Ϊ��������һ��async_read����֮ǰ����Ҫ���Ͷ����Ϣ��������Ҫ���д��buffer
		copy(msg.begin(), msg.end(), write_buffer_);
		sock_.async_write_some(buffer(write_buffer_, msg.size()), boost::bind(&connection::on_write, shared_from_this(), _1, _2));
	}
	
	void process_data(const string& msg)
	{
		//���������������ݣ�Ȼ����������һ��д�����
	}
};

int main(int argc, char** argv)
{
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
	connection::ptr(new connection)->start(ep);
	return 0;
}