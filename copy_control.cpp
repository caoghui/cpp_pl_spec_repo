
#include <iostream>
#include <cstdio>
#include <string>
#include <bitset>
using namespace std;

struct NoCopy
{
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete;
    NoCopy& operator=(const NoCopy&) = delete;
    ~NoCopy() = default;
};

class Sales_data
{
private:
    string bookNo;
    int units_sold = 0;
    double revenue = 0.0;
public:
    //构造函数
    Sales_data() = default;
    //与合成的拷贝构造函数等价的拷贝构造函数声明
    Sales_data(const Sales_data&);
    //等价于合成拷贝赋值运算符
    Sales_data& operator=(const Sales_data&);
    //移动构造函数
    Sales_data(Sales_data&& s) noexcept;
    //析构函数
    ~Sales_data()
    {

    }

    //接口部分
    string isbn() const { return bookNo; }
    Sales_data& combine(const Sales_data& );
    double avg_price() const;
};

Sales_data::Sales_data(const Sales_data& orig):bookNo(orig.bookNo), 
                                               units_sold(orig.units_sold), 
                                               revenue(orig.revenue)
{

}

Sales_data& Sales_data::operator=(const Sales_data& rhs)
{
    bookNo     = rhs.bookNo;          //调用string::operator=
    units_sold = rhs.units_sold;      //使用内置int赋值
    revenue    = rhs.revenue;         //使用内置double赋值
    return (*this);                   //返回左侧运算对象的引用
}

//非成员接口函数
Sales_data& add(const Sales_data& s1, const Sales_data& s2)
{

}

ostream& print(ostream& o, const Sales_data& s)
{
    o << s.isbn() << endl;
    return o;
}

istream& read(istream& i, Sales_data& s)
{
    return i;
}

int main()
{
    cout<<"copy control test"<<endl;
    
    return 0;
}

/*
void test()
{

    int n  = 9;
    int i=0;
    bitset<32> bs(n);
    cout<<n<<" => "<<bs<<endl;
    while(n)
    {
        i++;
        n = n & (n-1);
    }
    cout<<i<<endl;
    unsigned char i = 0x80;
    printf("0x%x\n", ~i>>3+1);

    char* c[] = {"ENTER", "NEW", "POINT", "FIRST"};
    char** cp[] = {c+3, c+2, c+1,c};
    char***cpp = cp;
    printf("%s\n", **++cpp);
    printf("%s\n", *--*++cpp+3);
    printf("%s\n", *cpp[-2]+3);
    printf("%s\n", cpp[-1][-1]+1);
}

*/