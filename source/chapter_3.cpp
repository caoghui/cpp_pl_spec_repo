/*************************************************************************
	> File Name: chapter_3.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年03月27日 星期一 13时58分59秒
 ************************************************************************/

#include <windows.h>
#include <ppl.h>
#include <concurrent_vector.h>
#include <array>
#include <tuple>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <list>
#include <map>
#include <utility>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "util.h"

using namespace std;
using namespace concurrency;

//calls the provided work function and returns the number of milliseconds that it takes to call that function
template<class Function>
__int64 time_call(Function&& f)
{
	__int64 begin = GetTickCount();
	f();
	return GetTickCount() - begin;
}

int fibonacci(int n)
{
	if (n < 2)
		return n;
	return fibonacci(n - 1) + fibonacci(n - 2);
}


class Entry
{
public:
	//构造函数
	Entry(): _name(""), _number(0) {}
	Entry(string name, int number): _name(name), _number(number) {}
	
	//复制构造函数
	Entry(const Entry& entry)
	{
		//cout<<"复制构造函数被调用!"<<endl;
		_name = entry.name();
		_number = entry.number();
	}
	
	//赋值操作符
	Entry& operator=(const Entry& entry)
	{	
		//cout<<"赋值操作符函数被调用!"<<endl;
		if(this != &entry)
		{
			_name = entry.name();
			_number = entry.number();
		}
		return *this;
	}
	
	//运算符重载，为了支持sort, unique_copy
	int operator<(const Entry& entry)
	{
		return _name < entry.name();
	}
	int operator==(const Entry& entry)
	{
		return _name == entry.name();
	}
	
	//访问函数
	string name() const    { return _name; }
	void   name(string n)  { _name = n;}
	int    number() const  { return _number; }
	void   number(int num) { _number = num; }
	
private:
	string _name;
	int _number;
};

//输出运算符重载
ostream& operator<<(ostream& s, const Entry& entry)
{
	return s<<"("<<entry.name()<<" , "<<entry.number()<<")";
}

template <class T> class Vec : public vector<T>
{
public:
	Vec() : vector<T>() {}
	Vec(int s) : vector<T>(s) {}
	
	T& operator[](int i) { return this->at(i); } 
	const T& operator[](int i) const { return this->at(i); }
};

Vec<Entry>       phone_book(10);
vector<Entry>    phone_book_vec;
list<Entry>      phone_book_lst;
map<string, int> phone_book_map;
/*
void print_entry(list<Entry>& lst)
{
	copy(lst.begin(), lst.end(), ostream_iterator<Entry>(cout, "\n"));
}

void print_entry(vector<Entry>& vec)
{
	copy(vec.begin(), vec.end(), ostream_iterator<Entry>(cout, "\n"));
}
//*/
///*
template <class C> void print(const C& t, ostream& s = cout)
{
	copy(t.begin(), t.end(), ostream_iterator<typename C::value_type>(s, "\n"));
}
//*/


//测试函数 begin
void test_map();
int  test_rwf();
void test_alg();
void test_lst();
void test_vec();
void test_int();
void test_str();
void test_ppl();
//end

int main(int argc, char** argv)
{
	//cout<<convert<int>("123")<<endl;
	//cout<<convert<string>(234)<<endl;
	//test_int();
	//test_str();
	//test_vec();
	//test_lst();
	//test_alg();
	//test_rwf();
	//test_map();
	test_ppl();
    return 0;
}


void test_ppl()
{
	__int64 elapsed;
	array<int, 4> a = {24, 26, 41, 42};
	//the results of the serial computation
	vector<tuple<int, int>> results1;
	//the results of the parallel computation
	concurrent_vector<tuple<int, int>> results2;

	//use the for_each algorithm to compute the results serially
	elapsed = time_call([&] {
		for_each(begin(a), end(a), [&](int n) {
			results1.push_back(make_tuple(n, fibonacci(n)));
		});
	});
	cout << "serial time : " << elapsed << " ms" << endl;

	elapsed = time_call([&] {
		parallel_for_each(begin(a), end(a), [&](int n) {
			results2.push_back(make_tuple(n, fibonacci(n)));
		});
		//because parallel_for_each acts concurrently, the results do not have a pre-determined order
		//sort the concurrent_vector object so that the results match the serial version
		sort(begin(results2), end(results2));
	});

	cout << "parallel time : " << elapsed << " ms" << endl;

	//print the results
	for_each(begin(results2), end(results2), [](tuple<int, int>& pair) {
		cout << "fib(" << get<0>(pair) << "): " << get<1>(pair) << endl;
	});

}

map<string, int> histogram;

void record(const string& s)
{
	histogram[s]++;
}

void my_print(const pair<const string, int>& r)
{
	cout<<r.first<<" : "<<r.second<<endl;
}

bool gt_4(const pair<const string, int>& r)
{
	return r.second > 4;
}

void test_map()
{
    ifstream is("map.txt");
	istream_iterator<string> in(is);
	istream_iterator<string> eos;
	
	//统计单词
	for_each(in, eos, record);
	
	for_each(histogram.begin(), histogram.end(), my_print);	
	
	map<string, int>::const_iterator iter = find_if(histogram.begin(), histogram.end(), gt_4);
	cout<<"gt_4 : "<<(*iter).first<<endl;;
}

int test_rwf()
{
	string from("read.txt");
	string to("write.txt");
	//cin>>from>>to;
	
	ifstream is(from.c_str());          //输入流
	istream_iterator<string> ii(is);    //流的输入迭代器
	istream_iterator<string> eos;       //输入的哨兵
	
	//读入文件内容
	vector<string> b(ii, eos);    
		
	cout<<"file ["<<from<<"] has lines : "<<b.size()<<endl;
	copy(b.begin(), b.end(), ostream_iterator<string>(cout, "\n"));
	
	//排序
	sort(b.begin(), b.end());
	cout<<"after sort :"<<endl;
	copy(b.begin(), b.end(), ostream_iterator<string>(cout, "\n"));
	
	//写入文件
	ofstream os(to.c_str());
	ostream_iterator<string> oo(os, "\n");
	unique_copy(b.begin(), b.end(), oo);
	return !is.eof() || !os;
}

void test_alg()
{
	//测试copy算法
	/*
	int buf[] = {1, 2, 3, 3, 4, 4, 5, 6, 3, 78, 123, 1};
	
	cout<<"sizeof(buf) : "<<sizeof(buf)/sizeof(int)<<endl;
	
	cout<<"call copy : "<<endl;
	copy(&buf[0], &buf[sizeof(buf)/sizeof(int)], ostream_iterator<int>(cout, "\n"));
	cout<<"call unique_copy : "<<endl;
	unique_copy(&buf[0], &buf[sizeof(buf)/sizeof(int)], ostream_iterator<int>(cout, "\n"));
	
	return;
	*/
	
	phone_book_vec.push_back(Entry("1", 1));
	phone_book_vec.push_back(Entry("4", 4));
	phone_book_vec.push_back(Entry("3", 3));
	phone_book_vec.push_back(Entry("2", 2));	
	phone_book_vec.push_back(Entry("3", 2));
	cout<<"初始化后调用print_entry打印vector :"<<endl;
	//print_entry(phone_book_vec);
	print(phone_book_vec);
    //打印vector
    //ostream_iterator<Entry> oo(cout, "\n");
    cout<<"利用ostream_iterator<Entry>打印vector : "<<endl;
	copy(phone_book_vec.begin(), phone_book_vec.end(), ostream_iterator<Entry>(cout, "\n"));
	
	
	//call sort
	sort(phone_book_vec.begin(), phone_book_vec.end());
	
	cout<<"after sort, the vector :"<<endl;
	print(phone_book_vec);
	//copy(phone_book_vec.begin(), phone_book_vec.end(), ostream_iterator<Entry>(cout, "\n"));
	
	
	unique_copy(phone_book_vec.begin(), phone_book_vec.end(), back_inserter(phone_book_lst));
	
	cout<<"after call unique_copy, the list : "<<endl;
	copy(phone_book_lst.begin(), phone_book_lst.end(), ostream_iterator<Entry>(cout, "\n"));
	
	
	cout<<"count :  "<<count(phone_book_vec.begin(), phone_book_vec.end(), Entry("5", 3))<<endl;
	ofstream os("out.txt");
	print(phone_book_lst, os);
}

void test_lst()
{
	//init list
    stringstream ss;	
	for(int i = 0 ; i < 5; ++i)
	{
		Entry entry;		
		ss << i;
		//entry.name = "name" + ss.str();
		//to_string(entry.name, i);
		ss.clear();
		ss.str("");
		cout<<"name : "<<entry.name()<<endl;

		//entry.number = i;
		phone_book_lst.push_back(entry);
	}
}

void test_vec()
{	
	try
	{
		cout<<phone_book[21].name();
	}
	catch(out_of_range)
	{
		cout<<"range error"<<endl;
	}
}

void test_str()
{
	string str;
	cout<<"Enter your name: ";
	getline(cin, str);
	cout<<"your name : "<<str<<endl;
}


void test_int()
{
	const float factor = 2.54;
	float x, in, cm;
	char ch = 0;
	cout<<"enter length : ";
	
	cin >> x;
	cin >> ch;
	switch(ch)
	{
	case 'i':
		in = x;
		cm = x * factor;
		break;
	case 'c':
		in = x / factor;
		cm = x;
		break;
	default:
		in = cm = 0;
		break;
	}
	cout <<in<< " in = "<<cm<<" cm"<<endl;
}
