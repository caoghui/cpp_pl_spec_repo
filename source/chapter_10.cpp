/*************************************************************************
	> File Name: chapter_10.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年03月28日 星期二 14时14分01秒
 ************************************************************************/

#include <string>
#include <iostream>
#include <ostream>
#include <sstream>

using namespace std;

int leapyear(int year)
{
	return 1;
}

class Date
{
public:
	enum Month { jan = 1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec};
private:
	int d, y;
	Month m;
	static Date default_date;
public:  //公共界面
	
	class Bad_date {};
	Date(int dd = 0, Month mm = Month(0), int yy = 0);
	//检查Date的函数
	int day() const { return d; }
	Month month() const { return m; }
	int year() const { return y; }
	
	string string_rep() const;
	void char_rep(char s[]) const;
	static void set_default(int, Month, int);
	
	//修改Date的函数
	Date& add_year(int n)
	{
		return *this;
	}
	Date& add_month(int n)
	{
		return *this;
	}
	Date& add_day(int n)
	{
		return *this;
	}

    bool operator==(Date a)
	{
   		return a.day() == d && a.month() == m && a.year() == y;
	}
};

//运算符重载
bool operator==(Date a, Date b)
{
   	return a.day() == b.day() && a.month() == b.month() && a.year() == b.year();
}

Date operator+(Date a, int i)
{
	Date d;	
	
	return d;
}	

ostream& operator<<(ostream& os, Date d)
{
	return os;
}

Date Date::default_date(22, jan, 1901);

Date::Date(int dd, Month mm, int yy)
{
	if (yy == 0) yy = default_date.year();
	if (mm == 0) mm = default_date.month();
	if (dd == 0) dd = default_date.day();
	int max;
	switch(mm)
	{
	case feb:
		max = 28 + leapyear(yy);
		break;
	case apr:
	case jun:
	case sep:
	case nov:
	    max = 30;
	    break;
	case jan:
	case mar:
	case may:
	case jul:
	case aug:
	case oct:
	case dec:
	    max = 31;
	    break;
	default:
		throw Bad_date();
	}
	if(dd < 1 || max < dd)
	{
		throw Bad_date();
	}
	y = yy;
	m = mm;
	d = dd;
}

bool midnight()
{
	return true;
}

void f(Date& d)
{
	Date lvb_day = Date(16, Date::dec, d.year());
	cout<<lvb_day<<endl;
	if(d.day() == 29 && d.month() == Date::feb)
	{
	}
	if(midnight())
	{
		d.add_day(1);
	}
	cout<<"day after : "<<d + 1<<endl;
}

class lbuffer
{
	string buf;
public:
	lbuffer() { cin >> buf; }
};

void PrettyFormat(int i, string& s)
{
	ostringstream temp;
	
	temp << i;
	s = temp.str();
}

int main(int argc, char** argv)
{
	int i = 30;
	string s;
	PrettyFormat(i, s);
	cout << s << endl;
    
    return 0;
}
