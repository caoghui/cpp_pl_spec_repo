/*************************************************************************
	> File Name: chapter_21.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年03月30日 星期四 14时44分50秒
 ************************************************************************/

#include <iostream>
#include <ostream>
#include <string>
#include <functional>
#include <algorithm>
#include <list>

using namespace std;

//父类定义纯虚函数，提供接口
class My_Base
{
protected:
	string _name;
	
public:
	My_Base(string name = ""):_name(name) {}
	virtual ostream& put(ostream& s) const = 0; //将*this写入s
	virtual ~My_Base() { cout<<"My_Base destroy call"<<endl; }
	void destroy() { delete this; }
	virtual void work() = 0;
};

ostream& operator<<(ostream& s, const My_Base& b)
{
	return b.put(s);
}

class SubType : public My_Base
{
public:
	SubType(string name) : My_Base(name) {}
	virtual ~SubType() { cout<<"SubType destroy call"<<endl; }
	ostream& put(ostream& s) const
	{
		return s<<_name;
	}
	void work()
	{
		cout<<"SubType do work"<<endl;
	}
};
//子类
class Son1 : public My_Base
{
public:
    Son1() : My_Base("Son1") {}
    virtual ~Son1() { cout<<"Son1 destroy call"<<endl; } 
	ostream& put(ostream& s)const
	{
		return s<<_name;
	}
	void work()
	{
		cout<<"Son1 do work"<<endl;
	}
};

class Son2 : public My_Base
{
public:
	Son2() : My_Base("Son2") {}
	virtual ~Son2() { cout<<"Son2 destroy call"<<endl; }
	ostream& put(ostream& s) const
	{
		return s<<_name;
	}
	void work()
	{
		cout<<"Son2 do work"<<endl;
	}
};

void f(const My_Base& b, Son1& s1, Son2& s2)
{
	cout<<"My_Base& : "<<b<<endl;
	cout<<"Son1 : "<<s1<<endl;
	cout<<"Son2 : "<<s2<<endl;
}

template<class T> void iocopy(istream& is, ostream& os)
{
	T buf;
	while(is>>buf)
	{
		os<<buf<<endl;
	}
}

void test_1();
void test_2();
void test_3();

int main(int argc, char** argv)
{
    cout<<"test chapter 21 case!"<<endl;
	//test_1();
	//test_2();
	test_3();
	
	//test get
	//char ch;
	//while(cin.get(ch))
	//	cout.put(ch);	
	
	//char buf[5];
	//cin.get(buf, sizeof(buf), '\n');
	//cout<<buf<<endl;
    return 0;
}

void work(My_Base* ptr)
{
	ptr->work();
}


void test_3()
{
	list<My_Base*> lst;
	lst.push_back(new SubType("SubType"));
	lst.push_back(new Son1());
	lst.push_back(new Son2());
	
	//for_each(lst.begin(), lst.end(), work);
	for_each(lst.begin(), lst.end(), mem_fun(&My_Base::work));
	
	
	for_each(lst.begin(), lst.end(), mem_fun(&My_Base::destroy));
	lst.clear();
}

void test_2()
{
	//iocopy<char>(cin, cout);
	iocopy<int>(cin, cout);
}

void test_1()
{
	const My_Base& b1 = Son1();
	My_Base* b2 = new Son2();
	cout<<"b1 : "<<b1<<endl;
	cout<<"b2 : "<<(*b2)<<endl;
	Son1 s1;
	Son2 s2;
	f(s1, s1, s2);
		
	delete b2;
	
	const My_Base& b3 = SubType("Sub type 1");
	cout<<"b3 : "<<b3<<endl;	
}

