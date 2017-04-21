/*************************************************************************
	> File Name: chapter_16.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年04月19日 星期三 17时24分52秒
 ************************************************************************/

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <deque>
#include <queue>
#include <utility>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>

using namespace std;

template<class C> typename C::value_type sum(const C& c)
{
	typename C::value_type s = 0;
	typename C::const_iterator iter = c.begin();
	while(iter != c.end())
	{
		s += *iter;
		++iter;
	}
	return s;
}

template<class In> typename iterator_traits<In>::difference_type dist_helper(In first, In last, input_iterator_tag)
{
	typename iterator_traits<In>::difference_type d = 0;
	while(first++ != last )
		d++;
	return d;
}

template<class Ran> typename iterator_traits<Ran>::difference_type dist_helper(Ran first, Ran last, random_access_iterator_tag)
{
	return last - first;
}

template<class In> typename iterator_traits<In>::difference_type distance(In first, In last)
{
	return dist_helper(first, last, iterator_traits<In>::iterator_category());
}

template<class In, class T> typename iterator_traits<In>::difference_type count(In first, In last, const T& val)
{
	typename iterator_traits<In>::difference_type res = 0;
	while(first != last)
	{
		if(*first++ == val)
			++res;
	}
	return res;
}
class Nocase  //不区分大小写的比较
{
public:
	bool operator()(const string& x, const string& y) const
	{
		string::const_iterator p = x.begin();
		string::const_iterator q = y.begin();
		while(p != x.end() && q != y.end() && toupper(*p) == toupper(*q))
		{
			++p;
			++q;
		}
		if(p == x.end())
			return q != y.end();
		if(q == y.end())
			return false;
		return toupper(*p) < toupper(*q);
	}
};

//test function
void test_sum();
void test_cmp();
void test_lst();
void test_stk();

int main(int argc, char** argv)
{
    cout<<"test chapter 16"<<endl;
    
	//test_sum();
	//test_cmp();
	//test_lst();
	test_stk();
	
    return 0;
}

void test_stk()
{
	vector<string> fruit;
	fruit.push_back("apple");
	fruit.push_back("pear");
	fruit.push_back("lemon");	

	stack<string, vector<string> > fruits(fruit);
	while(fruits.size())
	{
		cout<<fruits.top()<<endl;
		fruits.pop();
	}
}

void test_lst()
{
	list<string> fruit;
	fruit.push_back("apple");
	fruit.push_back("pear");
	cout<<"fruit : "<<endl;
	copy(fruit.begin(), fruit.end(), ostream_iterator<string>(cout, "\n"));
	
	list<string> citrus;
	citrus.push_back("orange");
	citrus.push_back("grapefruit");
	citrus.push_back("lemon");
	cout<<"citrus : "<<endl;
	copy(citrus.begin(), citrus.end(), ostream_iterator<string>(cout, "\n"));
	
	//list<string>::iterator p = find_if(fruit.begin(), fruit.end(), ('p'));
	//fruit.splice(p, citrus, citrus.begin());
	cout<<"=====after merge : "<<endl;
	fruit.merge(citrus);
	copy(fruit.begin(), fruit.end(), ostream_iterator<string>(cout, "\n"));
	
	fruit.remove("orange");
	cout<<"======after remove : ======="<<endl;
	copy(fruit.begin(), fruit.end(), ostream_iterator<string>(cout, "\n"));
	
	fruit.reverse();
	cout<<"======after reverse : "<<endl;
	copy(fruit.begin(), fruit.end(), ostream_iterator<string>(cout, "\n"));
}

void test_cmp()
{
	vector<string> fruit;
	fruit.push_back("apple");
	fruit.push_back("pear");
	fruit.push_back("Apple");
	fruit.push_back("Pear");
	fruit.push_back("lemon");
	
	cout<<"before call sort : "<<endl;
	copy(fruit.begin(), fruit.end(), ostream_iterator<string>(cout, "\n"));
	//sort(fruit.begin(), fruit.end(), Nocase());
	sort(fruit.begin(), fruit.end());
	cout<<"after call sort : "<<endl;
	copy(fruit.begin(), fruit.end(), ostream_iterator<string>(cout, "\n"));
	
}

void test_sum()
{
	ifstream is("read.txt");	
	istream_iterator<int> ii(is);
	istream_iterator<int> eos;
	
	//用文件内容初始化vector
	vector<int> vec_ints(ii, eos);
	
	cout<<"print vector :"<<endl;
	copy(vec_ints.begin(), vec_ints.end(), ostream_iterator<int>(cout, "\n"));
	copy(vec_ints.rbegin(), vec_ints.rend(), ostream_iterator<int>(cout, "\n"));
	cout<<"vector sum : "<<sum(vec_ints)<<endl;
	
    //重置文件头指针到开始
	is.clear();
	is.seekg(0, ios::beg);
	
	//用文件内容初始化list
	istream_iterator<int> ii2(is);
	list<int> lst_ints(ii2, eos);
	
	cout<<"print list :"<<endl;	
	copy(lst_ints.begin(), lst_ints.end(), ostream_iterator<int>(cout, "\n"));
	cout<<"list sum : "<<sum(lst_ints)<<endl;
}



