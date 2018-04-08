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
#include <string>
//#include <ace/OS.h>
//#include <ace/Log_Msg.h>
using namespace std;


void test_value();
void test_limits();
void test_auto();

class Sales_data
{
	//为Sales_data的非成员函数所做的友元声明
	friend Sales_data add(const Sales_data&, const Sales_data&);
	friend istream& read(istream&, Sales_data&);
	friend ostream& print(ostream&, const Sales_data&);

public:
	Sales_data() = default;
	Sales_data(const string& s, unsigned n, double p): bookNo(s), units_sold(n), revenue(p * n) {}
	Sales_data(const string& s) : bookNo(s) {}
	Sales_data(istream&);

	string isdn() const { return bookNo; }
	Sales_data& combine(const Sales_data&);
	
private:
	double avg_price() const
	{
		return units_sold ? revenue / units_sold : 0;
	}
	string bookNo;
	unsigned units_sold = 0;
	double revenue = 0.0;
};

Sales_data add(const Sales_data&, const Sales_data&);
ostream& print(ostream&, const Sales_data&);
istream& read(istream&, Sales_data&);

int main(int argc, char** argv)
{
    cout<<"chapter 04 test case"<<endl;
	const Sales_data s;
	s.isdn();
    //test_limits();
    //test_value();
    //ACE_TRACE(ACE_TEXT ("main"));
	//test_auto();
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
