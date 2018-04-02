#include <string>
#include <iostream>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>

using namespace std;
using namespace boost::asio;


io_service service;

void handle_connections()
{
	char buff[1024];
	ip::udp::socket sock(service, ip::udp::endpoint(ip::udp::v4(), 8001));
	while (true)
	{
		ip::udp::endpoint sender_ep;
		int bytes = sock.receive_from(buffer(buff), sender_ep);
		string msg(buff, bytes);
		sock.send_to(buffer(msg), sender_ep);
	}
}

int main(int argc, char** argv)
{
	handle_connections();
	return 0;
}