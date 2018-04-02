#include <string>
#include <iostream>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>

using namespace std;
using namespace boost::asio;

ip::udp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);

io_service service;

void sync_echo(string msg)
{
	ip::udp::socket sock(service, ip::udp::endpoint(ip::udp::v4(), 0));
	sock.send_to(buffer(msg), ep);
	char buff[1024];
	ip::udp::endpoint sender_ep;
	int bytes = sock.receive_from(buffer(buff), sender_ep);
	string copy(buff, bytes);
	cout << "server echoed our : " << msg << ": " << (copy == msg ? "OK" : "FAIL") << endl;
	sock.close();
}

int main(int argc, char** argv)
{
	char* messages[] = { "John says hi", "so does James", "Lucy got home", 0};
	boost::thread_group threads;
	for (char** message = messages; *message; ++message)
	{
		threads.create_thread(boost::bind(sync_echo, *message));
		boost::this_thread::sleep(boost::posix_time::microsec(1000));
	}
	threads.join_all();
	return 0;
}
