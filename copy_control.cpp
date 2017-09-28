
#include <iostream>
#include <cstdio>
#include <string>
using namespace std;

class Sales_data
{
private:
    string bookNo;
    int units_sold = 0;
    double revenue = 0.0;
public:
    //与合成的拷贝构造函数等价的拷贝构造函数声明
    Sales_data(const Sales_data&);
};

Sales_data::Sales_data(const Sales_data& orig):bookNo(orig.bookNo), 
                                               units_sold(orig.units_sold), 
                                               revenue(orig.revenue)
{

}

void test();

int main()
{
    cout<<"copy control test"<<endl;
    

    return 0;
}

void test()
{
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