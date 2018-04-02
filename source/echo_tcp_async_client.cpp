#include <string>
#include <iostream>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>

using namespace std;
using namespace boost::asio;

#define MEM_FN(x)        boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x, y)    boost::bind(&self_type::x, shared_from_this(), y)
#define MEM_FN2(x, y, z) boost::bind(&self_type::x, shared_from_this(), y, z)

io_service service;

class talk_to_server : public boost::enable_shared_from_this<talk_to_server>, boost::noncopyable
{
private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024};
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	string message_;

	typedef talk_to_server self_type;
	talk_to_server(const string& message) : sock_(service), started_(true), message_(message)
	{

	}
	void start(ip::tcp::endpoint ep)
	{
		sock_.async_connect(ep, MEM_FN1(on_connect, _1));
	}
public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_server> ptr;

	static ptr start(ip::tcp::endpoint ep, const string& message)
	{
		ptr new_(new talk_to_server(message));
		new_->start(ep);
		return new_;
	}

	void stop()
	{
		if (!started_)
			return;
		started_ = false;
		sock_.close();
	}

	bool started() { return started_; }

	void do_read()
	{
		async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
	}

	void do_write(const string& msg)
	{
		if (!started())
			return;
		std::copy(msg.begin(), msg.end(), write_buffer_);
		sock_.async_write_some(buffer(write_buffer_, msg.size()), MEM_FN2(on_write, _1, _2));
	}

	size_t read_complete(const boost::system::error_code& err, size_t bytes)
	{
		return 0;
	}

	void on_connect(const error_code& err)
	{
		if (!err)
			do_write(message_ + "\n");
		else
			stop();
	}

	void on_read(const error_code& err, size_t bytes)
	{
		if (!err)
		{
			string copy(read_buffer_, bytes - 1);
			cout << "server echoed our : " << message_ << ":" << (copy == message_ ? "OK" : "FAIL") << endl;
		}
		stop();
	}

	void on_write(const error_code& err, size_t bytes)
	{
		do_read();
	}

};

int main(int argc, char** argv)
{
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
	char* messages[] = { "John says hi", "so does James", "Lucy got home", 0};
	for (char** message = messages; *message; ++message)
	{
		talk_to_server::start(ep, *message);
		boost::this_thread::sleep(boost::posix_time::microsec(1000));
	}
	service.run();
	return 0;
}