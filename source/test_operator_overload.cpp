#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class Sales_data
{
private:
    int _isbn;

public:
    Sales_data& operator++()
    {
        cout << "call pre inc operator++" <<endl;
        return *this;
    }

    Sales_data operator++(int)
    {
        Sales_data tmp = *this;
        cout << "call post inc operator++" << endl;
        ++*this;
        return tmp;
    }

    friend ostream& operator<<(ostream& os, const Sales_data&);
    friend istream& operator>>(istream& is, Sales_data&);
};

// 输入输出运算符
ostream& operator<<(ostream& os, const Sales_data& data)
{
    os << data._isbn;
    return os;
}

istream& operator>>(istream& is, Sales_data& data)
{
    is >> data._isbn;
    if(!is)
    {
        data._isbn = -1;
    }
    return is;
}

void test_io();
void test_inc();

int main(int argc, char** argv)
{
    cout << "test operator overload.... "<< endl;
    

    test_inc();

    return 0;
}

void test_inc()
{
    Sales_data data;
    data++;
    ++data;
}

void test_io()
{
    Sales_data data;
    cin >> data;
    //data._isbn = "0000000-1111";
    cout << data << endl;
}