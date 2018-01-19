#include <string>
#include <typeinfo>
#include <iostream>

using namespace std;

class Screen
{
public:
    typedef std::string::size_type pos;
    
    Screen():cursor(0), height(0), width(0) 
    {
        contents = "this is contents";
    }

    char get_cursor() const 
    {
        cout << "get_cursor() called..." << endl;
        return contents[cursor]; 
    }
    char get() const { return '0';}
    char get(pos ht, pos wd) const
    {
        cout << "get(pos,pos) called..." << endl;
        return '2';
    }

    //移动接口
    Screen& home() 
    {
        cout << "Screen::home called..." << endl; 
        return *this; 
    }
    Screen& forward() 
    { 
        cout << "Screen::forward called..." << endl;
        return *this; 
    }
    Screen& back() 
    {
        cout << "Screen::back called..." << endl;
        return *this;
    }
    Screen& up() 
    {
        cout << "Screen::up called..." << endl;
        return *this;
    }
    Screen& down() 
    {
        cout << "Screen::down called..." << endl;
        return *this;
    }

    using Action = Screen& (Screen::*)();
    enum Directions { HOME, FORWARD, BACK, UP, DOWN };
    Screen& move(Directions cm) 
    {
        return (this->*Menu[cm])();
    }

    //定义静态成员，返回成员指针.
    static const std::string Screen::*data()
    {
        return &Screen::contents;
    }

private:
    std::string contents;
    pos cursor;
    pos height;
    pos width;
    static Action Menu[];
    //因为contents是私有的，所以需要将函数声明为友元函数
    //friend void test_mem_data();
};

Screen::Action Screen::Menu[] = {
    &Screen::home,
    &Screen::forward,
    &Screen::back,
    &Screen::up,
    &Screen::down,
};

//Action是一种可以指向Screen成员函数的指针，它接受两个pos实参，返回一个char
using Action = char (Screen::*)(Screen::pos, Screen::pos) const;

//测试函数
void test_mem_data();
void test_mem_fn();
void test_fn_table();

int main(int argc, char** argv)
{
    cout << "test member pointer running...." << endl;
    //test_mem_data();
    //test_mem_fn();
    test_fn_table();
    return 0;
}

void test_fn_table()
{
    Screen screen;
    screen.move(Screen::HOME);
}

void test_mem_fn()
{
    char (Screen::*pmf)() const;
    pmf = &Screen::get_cursor;
    //auto pmf = &Screen::get_cursor;
    //cout << "typeid(pmf) => " << typeid(pmf).name() << endl;
    char (Screen::*pmf2)(Screen::pos, Screen::pos) const;
    pmf2 = &Screen::get;

    Screen myScreen, *pScreen = &myScreen;

    char c1 = (pScreen->*pmf)();
    cout << "c1 => " << c1 << endl;
    char c2 = (myScreen.*pmf2)(0, 0);
    cout << "c2 => " << c2 << endl;

    Action get = &Screen::get;
    char c3 = (myScreen.*get)(0,0);
    cout << "c3 => " << c3 << endl;
}

void test_mem_data()
{
    //const string Screen::*ptr_data;
    //ptr_data = &Screen::contents;
    //auto ptr_data = &Screen::contents;
    auto ptr_data = Screen::data();

    Screen myScreen, *pScreen = &myScreen;

    auto s1 = myScreen.*ptr_data;
    cout << ".*  => " << s1 << endl;
    auto s2 = pScreen->*ptr_data;
    cout << "->* => " << s2 << endl;

}

