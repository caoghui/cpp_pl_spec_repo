#include <iostream>
#include <boost/config.hpp>
#ifdef BOOST_NO_CXX11_LAMBDAS
#   error "lambda functions required"
#else

#include <boost/detail/lightweight_test.hpp>
#include <vector>

using namespace std;

struct person 
{

};

struct world 
{
    void add_person(person const& a_person);
    vector<person> persons_;
};

#include <functional>
struct scope_exit
{
    scope_exit(std::function<void(void)> f) : f_(f){}
    ~scope_exit() { f_(); }
private:
    function<void(void)> f_;
};

void world::add_person(person const& a_person)
{
    bool commit = false;
    persons_.push_back(a_person);
    scope_exit on_exit([&commit, this](void){
        if(!commit)
            persons_.pop_back();
    });
    commit = true;
}

int main(int argc, char** argv)
{
    cout << "test cxx lambda" << endl;
    world w;
    person p;
    w.add_person(p);
    BOOST_TEST(w.persons_.size() == 2);
    return boost::report_errors();
}

#endif
