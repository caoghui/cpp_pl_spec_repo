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
//#include <ace/OS.h>
//#include <ace/Log_Msg.h>
using namespace std;


void test_value();
void test_limits();
void test_auto();

int main(int argc, char** argv)
{
    cout<<"chapter 04 test case"<<endl;
    //test_limits();
    //test_value();
    //ACE_TRACE(ACE_TEXT ("main"));
	test_auto();
    return 0;
}

void test_auto()
{
	vector<int> ivec{1,2,3,4,5};
	auto len = ivec.size();
	cout << "auto len = " << typeid(len).name() <<endl;

}

void test_limits()
{
	cout<<"largest float == "<<numeric_limits<float>::max()<<" , char is signed == "<<numeric_limits<char>::is_signed<<endl;
}

void test_value()
{
	char ch;
	cin>>ch;
	cout<<"the value of "<<ch<<" is : "<<int(ch)<<endl;
	cout<<"the value \'a\'+1 : "<<('a' + 1)<<endl;
	cout<<"sizeof(int) : "<<sizeof(int)<<endl;
}
