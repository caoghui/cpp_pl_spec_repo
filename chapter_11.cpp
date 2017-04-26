/*************************************************************************
	> File Name: chapter_11.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年04月25日 星期二 09时11分57秒
 ************************************************************************/

#include <iostream>
#include <sstream>

using namespace std;

class complex
{
	double re;
	double im;
	
public:
    //构造函数
	complex(double r = 0, double i = 0) : re(r), im(i) { }
	complex(double r) : re(r), im(0) { }
	
	//复制构造函数
	complex(const complex& c) : re(c.re), im(c.im) { }
	
	double real() const { return re; }
	double imag() const { return im; }
	
	//运算符重载
	complex operator+(complex);
	complex operator*(complex);
	complex& operator+=(complex a);
	complex& operator+=(double  a);
};

inline complex& complex::operator+=(complex a)
{
	re += a.re;
	im += a.im;
	return *this;
}

inline complex& complex::operator+=(double a)
{
	re += a;
	return *this;
}

complex operator+(complex, complex);
complex operator+(complex, double);
complex operator+(double , complex);
bool operator==(complex, complex);
bool operator!=(complex, complex);

istream& operator>>(istream&, complex);
ostream& operator<<(ostream&, complex);

//主函数
int main(int argc, char** argv)
{
	complex b = double(3);
	cout<<b<<endl;
	return 0;
}


complex operator+(complex a, complex b)
{
	complex r = a;
	return r += b;   //通过+=访问表示
}

complex operator+(complex a, double b)
{
	complex r = a;
	return r += b;
}

complex operator+(double a, complex b)
{
	complex r = b;
	return r += a;
}

bool operator==(complex a, complex b)
{
	return a.real() == b.real() && a.imag() == b.imag();
}

bool operator!=(complex a, complex b)
{
	return a.real() != b.real() || a.imag() != b.imag();
}

istream& operator>>(istream& is, complex a)
{
	return is;
}

ostream& operator<<(ostream& os, complex a)
{
	return os <<"( "<<a.real()<<" , "<<a.imag()<<" )";
}


