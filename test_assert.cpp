#include <iostream>
#include <string>
#include "smart_assert.h"
//#include "boost/smart_assert/assert.hpp"

#include <boost/scope_exit.hpp>


using namespace std;
using namespace boost;

void print_current_val(bool val, const char* expr)
{
    cout<<"expr : " << expr << "  value = " << val <<endl;
}

#define CHECK(x) print_current_val((x), #x)

#define STR "this is a char*"

int main(int argc, char** argv)
{
    cout << "test smart assert" << endl;
    string s1("Wake up,Neo");
    string s2("It's time to reload.");

    SMART_ASSERT(s1.empty()&&s2.empty())(s1)(s2);

    int i = 0;
    SMART_ASSERT(i!=0)(i);

    const char* buf = STR;

    SMART_ASSERT(buf==nullptr)(buf);

    BOOST_SCOPE_EXIT(buf)
    {
        { cout<<"exit"<<endl;}
    }BOOST_SCOPE_EXIT_END

    return 0;
}
