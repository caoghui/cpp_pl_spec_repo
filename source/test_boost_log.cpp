#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>

using namespace std;
using namespace boost;
using namespace boost::unit_test;
namespace logging = boost::log;

void init()
{
	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}

void test_log();
void test_token();
void test_token2();

#ifdef BOOST_TEST_MODULE

void free_test_function()
{
	BOOST_TEST(true);
}

namespace utf = boost::unit_test;
const bool io_implemented = true;
const bool db_implemented = false;

BOOST_AUTO_TEST_SUITE(suite1, *utf::disabled())

BOOST_AUTO_TEST_CASE(test_1)
{
	BOOST_TEST(1 != 1);
}

BOOST_AUTO_TEST_CASE(test_2, *utf::enabled())
{
	BOOST_TEST(2 != 2);
}

BOOST_AUTO_TEST_CASE(test_io, *utf::enable_if<io_implemented>())
{
	BOOST_TEST(3 != 3);
}

BOOST_AUTO_TEST_CASE(test_db, *utf::enable_if<db_implemented>())
{
	BOOST_TEST(4 != 4);
}

BOOST_AUTO_TEST_SUITE_END()

/*
test_suite* init_unit_test_suite(int, char**)
{
	for (int i = 0; i < 1000; ++i)
	{
		framework::master_test_suite().add(BOOST_TEST_CASE(&free_test_function));
	}
	return 0;
}

BOOST_AUTO_TEST_CASE(first_test)
{
	int i = 1;
	BOOST_TEST(i);
	BOOST_TEST( i == 2);
}
*/
#else
///*
int main(int argc, char** argv)
{
	//test_token();
	test_token2();
	return 0;
}
//*/
#endif
void test_token2()
{
	string str = ";;Hello|world||-foo--bar;yow;baz|";
	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
	boost::char_separator<char> sep("-;|");
	tokenizer tokens(str, sep);
	for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
	{
		cout << "<" << *tok_iter << ">";
	}
	cout << endl;
}

void test_token()
{
	string s = "This is , a test";
	tokenizer<> tok(s);
	for (tokenizer<>::iterator beg = tok.begin(); beg != tok.end(); ++beg)
	{
		cout << *beg << endl;
	}
}

void test_log()
{
	init();

	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	BOOST_LOG_TRIVIAL(info) << "A infor severity message";
	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	BOOST_LOG_TRIVIAL(error) << "An error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

}