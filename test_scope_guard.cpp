#include <iostream>
#include <functional>
#include <algorithm>
#include "ScopeGuard.h"

using namespace std;

void test_scope_guard()
{
    std::function<void()> f = [] { cout << "cleanup form unnormal exit" <<endl; };
    //正常退出
    {
        auto gd = MakeGuard(f);
        ///
        gd.Dismiss();
    }

    //异常退出
    {
        auto gd = MakeGuard(f);
        //
        throw 1;
    }

    //非正常退出
    {
        auto gd = MakeGuard(f);
        return;
    }
}

int main(int argc, char** argv)
{
std::cout << "dz" << 'u';
    cout << "test scope guard" <<endl;
    try
    {
        test_scope_guard();
    }
    catch(...)
    {
        cout << "catch an exception" << endl;
    }
    
    ON_SCOPE_EXIT([&] { cout << "release resource" << endl; });

    return 0;
}




