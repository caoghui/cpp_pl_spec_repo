#include <iostream>
#include <string>
#include <cctype>
using namespace std;

//测试类型转换
void test_convert();
int fact(int val);
int fact(const int val);

int main(int argc, char* argv[])
{
    cout<<"This is a basic data type test !"<<endl;
    //test_convert();
    string s;
    //cin>>s;
    while(getline(cin, s))
    {
        if(!s.empty())
        {
            for(decltype(s.size()) index = 0; index != s.size() &&  !isspace(s[index]); ++index)
            {
                s[index] = toupper(s[index]);
            }
            /*
            for(auto &c : s)
            {
                cout<<c<<endl;
                c = toupper(c);
            }
            */
            cout<<s<<endl;  
        }
    }
    cout<<fact(5)<<endl;
    return 0;
    //test_convert();
    string str;
    while(getline(cin, str) && str != "quit")
        cout<<"input : "<<str<<endl;

    return 0;
}

int fact(int val)
{
    int ret = 1;
    while(val > 1)
    {
        ret *= val--;
    }
    return ret;
}

void test_convert()
{
    bool b = 34;  //b为真
    int  i = b;   //i的值为1
    cout<<"bool to init : i = "<<i<<endl;
    //i = 3.14;
    double pi = i;
    //unsigned char c = -1;
    unsigned char c = -2;
    //signed char c2 = 256;
    unsigned u = -1;
    cout<<"bool b = "<<b<<endl;
    cout<<"int i = "<<i<<endl;
    cout<<"double pi = "<<pi<<endl;
    cout<<"unsigned char c = "<<(int)c<<endl;
    //cout<<"signed char c2 = "<<(int)c2<<endl;
    cout<<"(-1)%256 = "<<((-1)%256)<<endl;
    cout<<"unsigned u = "<<u<<endl;
}

