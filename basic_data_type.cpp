#include <iostream>

using namespace std;

//测试类型转换
void test_convert();

int main(int argc, char* argv[])
{
    cout<<"This is a basic data type test !"<<endl;
    test_convert();
    return 0;
}


void test_convert()
{
    bool b = 34;  //b为真
    int  i = b;   //i的值为1
    cout<<"bool to init : i = "<<i<<endl;
    i = 3.14;
    double pi = i;
    //unsigned char c = -1;
    unsigned char c = -2;
    signed char c2 = 256;
    unsigned u = -1;
    cout<<"bool b = "<<b<<endl;
    cout<<"int i = "<<i<<endl;
    cout<<"double pi = "<<pi<<endl;
    cout<<"unsigned char c = "<<(int)c<<endl;
    cout<<"signed char c2 = "<<(int)c2<<endl;
    cout<<"(-1)%256 = "<<((-1)%256)<<endl;
    cout<<"unsigned u = "<<u<<endl;
}

