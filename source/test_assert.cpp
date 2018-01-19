#include <iostream>
#include <string>
#include "smart_assert.h"
//#include "boost/smart_assert/assert.hpp"

#include "boost/scope_exit.hpp"

using namespace std;
using namespace boost;

void print_current_val(bool val, const char* expr)
{
    cout<<"expr : " << expr << "  value = " << val <<endl;
}

#define CHECK(x) print_current_val((x), #x)

#define STR "this is a char*"

#define assert_static(e) do{\
	enum { assert_static__ = 1/(e)}; \
}while(0)


void test_smart_assert();

template<typename T, typename U> void bit_copy(T& a, U& b)
{
//	assert_static(sizeof(b) == sizeof(a));
	static_assert(sizeof(b) == sizeof(a), "the parameters of bit_copy must have same width");
	memcpy(&a, &b, sizeof(b));
}
static_assert(sizeof(int) == 4, "This 64-bit machine should follow this");

void Throw() { throw 1; }
void NoBlockThrow() { Throw(); }
void BlockThrow() noexcept { Throw(); }

void test_noexcept();

int main(int argc, char** argv)
{
	test_noexcept();

    return 0;
}

void test_noexcept()
{
	try
	{
		Throw();
	}
	catch (...)
	{
		cout << "Found throw.." << endl;
	}

	try
	{
		NoBlockThrow();
	}
	catch (...)
	{
		cout << "Throw is not blocked." << endl;
	}

	try
	{
		BlockThrow();
	}
	catch (...)
	{
		cout << "Found Throw 1." << endl;
	}
}

void test_smart_assert()
{
	cout << "test smart assert" << endl;
	string s1("Wake up,Neo");
	string s2("It's time to reload.");

	SMART_ASSERT(s1.empty() && s2.empty())(s1)(s2);

	int i = 0;
	SMART_ASSERT(i != 0)(i);

	const char* buf = STR;

	SMART_ASSERT(buf == nullptr)(buf);
	
}

