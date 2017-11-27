
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

class Sales_data;
istream& read(istream& is, Sales_data&);

class Sales_data
{
public:
    string bookNo;
    int units_sold = 0;
    double revenue = 0.0;
public:
    //构造函数
    Sales_data() = default;
    Sales_data(const string& isbn): bookNo(isbn){}
    Sales_data(const string& isbn, unsigned n, double price):bookNo(isbn), units_sold(n), revenue(price * n) {}
    Sales_data(istream& is);

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

Sales_data::Sales_data(istream& is)
{
    read(is, *this);
}

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

//成员接口函数定义
Sales_data& Sales_data::combine(const Sales_data& rhs)
{
    units_sold += rhs.units_sold;
    revenue += rhs.revenue;
    return *this;
}
double Sales_data::avg_price()const
{
    if(units_sold)
    {
        return revenue / units_sold;
    }
    else
    {
        return 0;
    }
}

//非成员接口函数
Sales_data add(const Sales_data& lhs, const Sales_data& rhs)
{
    Sales_data sum = lhs;
    sum.combine(rhs);
    return sum;
}

ostream& print(ostream& os, const Sales_data& item)
{
    os << item.isbn() << " " << item.units_sold << " "  << item.revenue << " ;avg_price = " << item.avg_price() << endl;
    return os;
}

istream& read(istream& is, Sales_data& item)
{
    double price;
    is >> item.bookNo >> item.units_sold >> price;
    item.revenue = price * item.units_sold;
    return is;
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