#include <iostream>
#include <thread>
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/asio.hpp>
#include <boost/thread.hpp>
using namespace std;
using namespace boost::asio;

//�ź�
void signal_handler(const boost::system::error_code& err, int signal);
void test_signal();

//��ʱ��
bool read_flag = false;
void deadline_handler(const boost::system::error_code&)
{
	cout << (read_flag ? "read successfully" : "read failed") << endl;
}

void read_handler(const boost::system::error_code&)
{
	read_flag = true;
}
//io_service service;
//ip::tcp::socket sock(service);

//��һ��io_serviceʵ����һ�������̵߳ĵ��߳�����
void single_thread();
//��һ��io_serviceʵ���Ͷ�������̵߳Ķ��߳�����
void multi_thread();
//�ж��io_serviceʵ���Ͷ�������̵߳Ķ��߳�����
void multi_thread2();

//boost.asio basic test
void test_basic();
void test_hostname();

//�ͻ���
void tcp_client();

//Ex1 ��һ��TCP�׽����Ͻ���ͬ����д
void tcp_sync_read();
//Ex2 ��һ��UDP�׽����Ͻ���ͬ����д
void udp_sync_read();

//��һ��UDP�����׽������첽��ȡ����
io_service service;
ip::udp::socket sock(service);
ip::udp::endpoint sender_ep;

char buff[512];
void on_read(const boost::system::error_code& err, size_t read_bytes)
{
	cout << "read " << read_bytes << endl;
	sock.async_receive_from(buffer(buff), sender_ep, on_read);
}

int main(int argc, char** argv)
{
	ip::udp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
	sock.open(ep.protocol());
	sock.set_option(ip::udp::socket::reuse_address(true));
	sock.bind(ep);
	sock.async_receive_from(buffer(buff, 512), sender_ep, on_read);
	service.run();

	//����һ��io_service��һ�������̵߳ĵ��߳�
	//single_thread();
	//����һ��io_service�Ͷ�������̵߳Ķ��߳�
	//multi_thread();
	//���Զ��io_service�Ͷ�������̵߳Ķ��߳�
	//multi_thread2();

	//test boost.asio basic
	//test_basic();
	//test_hostname();
	//tcp_client();
	//test_signal();
	return 0;
}

void tcp_client()
{
	io_service service;
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 80);
	ip::tcp::socket sock(service);
	sock.open(ip::tcp::v4());
	sock.connect(ep);
	sock.write_some(buffer("Get /index.html\r\n"));
	char buff[1024];

	//sock.receive(buffer(buff), ip::tcp::socket::message_peek);
	//memset(buff, 2014, 0);
	//sock.receive(buffer(buff));

	sock.read_some(buffer(buff, 1024));
	sock.shutdown(ip::tcp::socket::shutdown_receive);
	sock.close();
}

void test_basic()
{
	cout << ip::host_name() << endl;
	ip::address addr = ip::address::from_string("127.0.0.1");
	cout << addr.to_string() << endl;
	try
	{
		ip::address addr2 = ip::address::from_string("www.yahoo.com");
		cout << addr2.to_string() << endl;
	}
	catch (...)
	{
		cout << "catch exception" << endl;
	}
	//server port
	ip::tcp::endpoint server(ip::tcp::v4(), 80);
	//client port
	ip::tcp::endpoint client(ip::address::from_string("127.0.0.1"), 80);

	io_service service;
	ip::udp::socket sock(service);
	sock.set_option(ip::udp::socket::reuse_address(true));
	
}

void test_hostname()
{
	io_service service;
	ip::tcp::resolver resolver(service);
	ip::tcp::resolver::query query("www.baidu.com", "80");
	ip::tcp::resolver::iterator iter = resolver.resolve(query);
	ip::tcp::endpoint ep = *iter;
	cout << ep.address().to_string() << ":" << ep.port() << endl;
}

void tcp_sync_read()
{
	io_service service;
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 80);
	ip::tcp::socket sock(service);
	sock.connect(ep);
	sock.write_some(buffer("GET /index.html\r\n"));
	cout << "bytes available " << sock.available() << endl;
	char buff[512];
	size_t read = sock.read_some(buffer(buff));
}

void udp_sync_read()
{
	io_service service;
	ip::udp::socket sock(service);
	sock.open(ip::udp::v4());
	ip::udp::endpoint receiver_ep(ip::address::from_string("87.248.112.181"), 80);
	sock.send_to(buffer("testing\n"), receiver_ep);
	char buff[512];
	ip::udp::endpoint sender_ep;
	sock.receive_from(buffer(buff), sender_ep);
}

//connect ������
void connect_handler(const boost::system::error_code& err)
{
	if (err)
		return;
	cout << "connect handler call" << endl;
}

//��ʱ������
void timeout_handler(const boost::system::error_code&)
{
	cout << "time out" << endl;
}

//��һ��io_serviceʵ����һ�������̵߳ĵ��߳�����
void single_thread()
{
	io_service service; //����socket��������serice������
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
	ip::tcp::socket sock1(service); //all the socket operations are handled by service
	ip::tcp::socket sock2(service);
	sock1.async_connect(ep, connect_handler);
	sock2.async_connect(ep, connect_handler);
	deadline_timer t(service, boost::posix_time::seconds(5));
	t.async_wait(timeout_handler);
	service.run();
}

//��һ��io_serviceʵ���Ͷ�������̵߳Ķ��߳�����
io_service service1;
void run_service()
{
	cout << this_thread::get_id() << " run_service running " << endl;
	service1.run();
}
void multi_thread()
{	
	thread tasks[5];
	ip::tcp::socket sock1(service);
	ip::tcp::socket sock2(service);
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
	sock1.async_connect(ep, connect_handler);
	sock2.async_connect(ep, connect_handler);
	deadline_timer t(service, boost::posix_time::seconds(5));
	t.async_wait(timeout_handler);
	for (int i = 0; i < 5; ++i)
	{
		//boost::thread([&]() { cout << "thread running" << endl; service.run(); });
		//boost::thread(run_service);
		tasks[i] = move(thread{ run_service });
	}
	cin.get();
}

//�ж��io_serviceʵ���Ͷ�������̵߳Ķ��߳�����
io_service service2[2];

void run_service2(int idx)
{
	cout << "run_service2 running...." << endl;
	service2[idx].run();
}

void multi_thread2()
{
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
	ip::tcp::socket sock1(service2[0]);
	ip::tcp::socket sock2(service2[1]);
	sock1.async_connect(ep, connect_handler);
	sock2.async_connect(ep, connect_handler);
	deadline_timer t(service2[0], boost::posix_time::seconds(5));
	t.async_wait(timeout_handler);
	for (int i = 0; i < 2; ++i)
	{
		boost::thread(boost::bind(run_service2, i));
	}
	cin.get();
}

void signal_handler(const boost::system::error_code& err, int signal)
{
	cout << "signal_handler is called" << endl;
}

void test_signal()
{
	io_service service;
	signal_set signal(service, SIGINT, SIGTERM);
	signal.async_wait(signal_handler);
}