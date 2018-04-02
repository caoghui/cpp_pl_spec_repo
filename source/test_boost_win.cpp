#define BOOST_TEST_MODULE custom_main
#define BOOST_TEST_NO_MAIN
//#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/process.hpp>

#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace std;


int main(int argc, char** argv)
{

	boost::asio::io_service service;
	boost::asio::streambuf buff;


	boost::process::child c("test.exe", boost::process::std_out > buff, service);
	
	service.run();
	c.wait();

	//boost::process::child c2(boost::process::exe = "test.exe");
	//c2.wait();

}

/*
namespace utf = boost::unit_test;
namespace data = boost::unit_test::data;

int sample1[] = { 1, 2 };
int sample2[] = { 8, 9, 10 };

BOOST_DATA_TEST_CASE(test1, data::make(sample1) + sample2, var)
{
	cout << var << endl;
}

const char* arr[] = { "cat", "dog" };

BOOST_DATA_TEST_CASE(test2, data::xrange(2) ^ data::make(arr), xr, array_element)
{
	cout << "test 2 : " << xr << ", " << array_element << endl;
	BOOST_TEST(array_element != "mammoth");
}



BOOST_DATA_TEST_CASE(test2, data::xrange(3) * (data::random(data::distribution = std::uniform_real_distribution<float>(1, 2)) ^ data::xrange(2)),
	xr, random_sample, index)
{
	cout << "test 2 : " << xr << " / " << random_sample << " , " << index << endl;
	BOOST_TEST(random_sample < 1.7);
}

BOOST_AUTO_TEST_CASE(test1)
{
	BOOST_TEST(false);
}

void make_use_of(char**)
{
	cout << "Using custom entry point...." << endl;
}

int main(int argc, char** argv, char** envp)
{
	make_use_of(envp);
	return utf::unit_test_main(init_unit_test, argc, argv);
}
*/
