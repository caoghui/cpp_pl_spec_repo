#include <string>
#include <iostream>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace boost::asio;

io_service service;

size_t read_complete(char* buff, const error_code& err, size_t bytes)
{
	if (err)
		return 0;
	bool found = std::find(buff, buff + bytes, '\n') < buff + bytes;
	return found ? 0 : 1;
}

void handle_connections()
{
	ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
	char buff[1024];
	while (true)
	{
		ip::tcp::socket sock(service);
		acceptor.accept(sock);
		int bytes = read(sock, buffer(buff), boost::bind(read_complete, buff, _1, _2));
		string msg(buff, bytes);
		sock.write_some(buffer(msg));
		sock.close();
	}
}

int main(int argc, char** argv)
{
	handle_connections();
	return 0;
}