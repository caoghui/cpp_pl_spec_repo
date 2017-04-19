/*************************************************************************
	> File Name: chapter_16.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年04月19日 星期三 17时24分52秒
 ************************************************************************/

#include <iostream>
#include <vector>
#include <list>
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

int main(int argc, char** argv)
{
    cout<<"test chapter 16"<<endl;
    
	ifstream is("read.txt");
	
	istream_iterator<int> ii(is);
	istream_iterator<int> eos;
	
	cout<<"print vector :"<<endl;
	vector<int> vec_ints(ii, eos);
	copy(vec_ints.begin(), vec_ints.end(), ostream_iterator<int>(cout, "\n"));
	cout<<"vector sum : "<<sum(vec_ints)<<endl;
	
	cout<<"print list :"<<endl;
	
	//ifstream is2("read.txt");
	
	//istream_iterator<int> ii2(is2);
	//istream_iterator<int> eos2;
	is.clear();
	is.seekg(0, ios::beg);
	
	list<int> lst_ints(ii, eos);
	
	copy(lst_ints.begin(), lst_ints.end(), ostream_iterator<int>(cout, "\n"));
	
	
	
	
    return 0;
}
