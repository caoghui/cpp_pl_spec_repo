#include <iostream>
#include <sstream>
#include <string>
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;

struct person
{
	std::string first_name;
	std::string last_name;
	int age;
};

std::ostream& operator<<(std::ostream&out, const person& p)
{
	return out << "[" << p.first_name << "," << p.last_name << "," << p.age << "]";
}

std::istream& operator >> (std::istream& in, person& p)
{
	return in >> p.first_name >> p.last_name >> p.age;
}

void write_person()
{
	boost::asio::streambuf buf;
	std::ostream out(&buf);
	person p;
	//初始化p
	out << p << endl;
	//write(sock, buf);
	//cout << &buf << endl;
}

void read_person()
{
	boost::asio::streambuf buf;
	//read_until(sock, buf, "\n");
	//std:cout << &buf << endl;
	std::istream in(&buf);
	person p;
	in >> p;
}

std::string to_string(boost::asio::streambuf& buf)
{
	std::ostringstream out;
	out << &buf;
	return out.str();
}

void to_console()
{
	boost::asio::streambuf buf;
	std::ostream out(&buf);
	out << "hi there";
	cout << &buf << endl;
}

////////////////////////////////////////////////////////////
boost::asio::streambuf buf;

bool is_vowel(char c)
{
	return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

size_t read_complete(boost::system::error_code, size_t bytes)
{
	const char* begin = buffer_cast<const char*>(buf.data());
	if (bytes == 0)
		return 1;

	while (bytes > 0)
	{
		if (is_vowel(*begin++))
			return 0;
		else
			--bytes;
	}

	return 1;
}

//read(sock, buf, read_complete);

//or 使用正则表达式
//read_until(sock, buf, boost::regex("^[aeiou]+"));

typedef buffers_iterator<boost::asio::streambuf::const_buffers_type> Iter;
std::pair<Iter, bool> match_vowel(Iter b, Iter e)
{
	while (b != e)
	{
		if (is_vowel(*b++))
			return std::make_pair(b, true);
	}
	return std::make_pair(e, false);
}
//size_t bytes = read_until(sock, buf, match_vowel);
//////////////////////////////////////////////////////////////

void echo();

int main(int argc, char** argv)
{
	//person p;
	//cin >> p;
	//cout << p << endl;
	to_console();

	return 0;
}

void echo()
{
	io_service service;
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
	ip::tcp::socket sock(service, ep);
	boost::asio::streambuf buf;
	std::ostream out(&buf);
	out << "echo" << endl;
	write(sock, buf);
}