
#include <string>
#include <iostream>
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using namespace boost::asio;

io_service service;

int main(int argc, char** argv)
{
	typedef ssl::stream<ip::tcp::socket> ssl_socket;
	ssl::context ctx(ssl::context::sslv23);
	ctx.set_default_verify_paths();

	//打开一个到指定主机的SSL socket
	io_service service;
	ssl_socket sock(service, ctx);
	ip::tcp::resolver resolver(service);
	std::string host = "www.yahoo.com";
	ip::tcp::resolver::query query(host, "https");
	connect(sock.lowest_layer(), resolver.resolve(query));

	//SSL握手
	sock.set_verify_mode(ssl::verify_none);
	sock.set_verify_callback(ssl::rfc2818_verification(host));
	sock.handshake(ssl_socket::client);

	std::string req = "GET /index.html HTTP/1.0\r\nHost: " + host + "\r\nAccept: */*\r\nConnection: close\r\n\r\n";
	write(sock, buffer(req.c_str(), req.length()));
	char buff[512];
	boost::system::error_code err;
	while (!err)
	{
		int bytes = read(sock, buffer(buff), err);
		std::cout << std::string(buff, bytes);
	}
}