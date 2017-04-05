/*************************************************************************
	> File Name: util.h
	> Author: 
	> Mail: 
	> Created Time: 2017年03月27日 星期一 16时51分03秒
 ************************************************************************/

#ifndef _UTIL_H
#define _UTIL_H

#include <string>
#include <sstream>

using namespace std;

//help function
template <class T> void to_string(string& result, const T& t)
{
	ostringstream oss;
	oss << t;
	result = oss.str();
}

template<typename out_type, typename in_type> out_type convert(const in_type& t)
{
	stringstream ss;
	ss << t;
	out_type result;
	ss >> result;
	return result;
}

template<class C, class T> int my_count(const C& v, T val)
{
	typename C::const_iterator iter = find(v.begin(), v.end(), val);
	int n = 0;
	while(iter != v.end())
	{
		++n;
		++iter;
		iter = find(iter, v.end(), val);
	}
	return n;
}

#endif
