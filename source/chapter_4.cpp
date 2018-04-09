/*************************************************************************
	> File Name: chapter_4.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年04月07日 星期五 16时05分22秒
 ************************************************************************/

#include <limits>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <sstream>
#include <string>
#include <cassert>
#include <functional>
#include <algorithm>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/boost_tuple.hpp>


using namespace std;
using namespace boost;
using namespace boost::lambda;

void test_vector();
void test_streambuf();
void test_cast(int argc, char** argv);
void test_stringize();

struct stringize_functor
{
private:
	string& result;
public:
	explicit stringize_functor(string& res): result(res)
	{
	}
	template<class T>
	void operator()(const T& v) const
	{
		result += boost::lexical_cast<string>(v);
	}
};

template<class Sequence>
string stringize(const Sequence& seq)
{
	string result;
	boost::fusion::for_each(seq, stringize_functor(result));
	return result;
}

//converting to string without dynamic memory allocation
void number_to_file(int number, FILE* file);
//converting part of the string
int convert_strings_part(const string& s, size_t pos, size_t n);

struct to_long_double_functor : boost::static_visitor<long double>
{
	template<class T>
	long double operator()(const T& v) const
	{
		return boost::lexical_cast<long double>(v);
	}
};

template<class Variant>
long double to_long_double(const Variant& v)
{
	return boost::apply_visitor(to_long_double_functor(), v);
}

void test_todouble()
{
	boost::variant<char, int, string> v1('0'), v2("10.0001"), v3(1);
	const long double sum = to_long_double(v1) + to_long_double(v2) + to_long_double(v3);
	const int ret = (sum > 11 && sum < 11.1 ? 0 : 1);
	assert(ret == 0);
	cout << sum << endl;
}

int main(int argc, char** argv)
{	
	//test_vector();
	//test_streambuf();
	//test_cast(argc, argv);
	//test_stringize();
	test_todouble();

    return 0;
}

void test_stringize()
{
	boost::tuple<char, int, char, int> decim('-', 10, 'e', 5);
	assert(stringize(decim) == "-10e5");
	cout << stringize(decim) << endl;

	pair<short, string> value_and_type(270, "Kelvin");
	assert(stringize(value_and_type) == "270Kelvin");
	cout << stringize(value_and_type) << endl;
}

void number_to_file(int number, FILE* file)
{
	//convert some number and put it to file
	typedef boost::array<char, 50> buf_t;
	buf_t buffer = boost::lexical_cast<buf_t>(number);
	std::fputs(buffer.begin(), file);
}

//take part of the string and converts it to int
int convert_strings_part(const string& s, size_t pos, size_t n)
{
	return boost::lexical_cast<int>(s.data() + pos, n);
}

void test_cast(int argc, char** argv)
{
	using boost::lexical_cast;
	using boost::bad_lexical_cast;
	vector<short> args;
	while (*++argv)
	{
		try
		{
			args.push_back(lexical_cast<short>(*argv));
		}
		catch (const bad_lexical_cast& )
		{
			args.push_back(0);
		}
	}
	for_each(args.begin(), args.end(), cout << _1 << " ");
	cout << endl;
}

void test_vector()
{
	vector<int> ivec{1,2,3,4,5};

	for_each(ivec.begin(), ivec.end(), cout << _1 << " ");
	cout << endl;
	for_each(ivec.begin(), ivec.end(), cout << _1 * 2 << " ");
	cout << endl;
}

void test_streambuf()
{
	stringstream is("ax2");
	istreambuf_iterator<char> b2(is);
	istreambuf_iterator<char> e2;

	istreambuf_iterator<char> i = find_if(b2, e2, _1 == 'x');
	cout << *i << endl;
	cout << distance(i, e2) << endl;

}
