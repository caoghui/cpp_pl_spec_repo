#include <iostream>
#include <vector>
#include <algorithm>
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

class _functor
{
public:
	int operator()(int x, int y)
	{
		return x + y;
	}
};

class Tax
{
private:
	float rate;
	int base;
public:
	Tax(float r, int b) : rate(r), base(b) { }
	float operator()(float money)
	{
		return (money - base) * rate;
	}
};

vector<int> nums;

void Add(const int val)
{
	auto print = [&] {
		for (auto s : nums)
		{
			cout << s << "\t";
		}
		cout << endl;
	};

	//传统的for循环方式
	for (auto i = nums.begin(); i != nums.end(); ++i)
	{
		*i = *i + val;
	}
	print();

	//试一试for_each及内置仿函数
	for_each(nums.begin(), nums.end(), bind2nd(plus<int>(), val));
	print();

	//实际这里需要使用STL的一个变动性算法: transform
	transform(nums.begin(), nums.end(), nums.begin(), bind2nd(plus<int>(), val));
	print();

	//不过在lambda的支持下，还是可以只使用for_each
	for_each(nums.begin(), nums.end(), [=](int& i) { i += val; })
		;
	print();
}

void test1();
void test_tax();
void test_mut();
void test_add();

int main(int argc, char** argv)
{
	//test1();
	//test_tax();
	//test_mut();
	test_add();
	/*
    world w;
    person p;
    w.add_person(p);
    BOOST_TEST(w.persons_.size() == 2);
	*/
    return boost::report_errors();
}

void test_add()
{
	for (int i = 0; i < 10; ++i)
		nums.push_back(i);
	Add(10);
}

void test_mut()
{
	int val;
	//编译失败，在const的lambda中修改常量
	//auto const_val_lambda = [=]() { val = 3; };

	//非const的lambda，可以修改常量数据
	auto mutable_val_lambda = [=]() mutable { val = 3; };

	//依然是const的lambda，不过没有改动引用本身
	auto const_ref_lambda = [&] { val = 3; };

	//依然是const的lambda，通过参数传递val
	auto const_param_lambda = [&](int v) { v = 3; };
	const_param_lambda(val);
}

void test_tax()
{
	Tax high(0.4, 30000);
	Tax middle(0.25, 20000);

	cout << "tax over 3w : " << high(37500) << endl;
	cout << "tax over 2w : " << middle(27500) << endl;
	return;
}

void test1()
{
	int girls = 3;
	int boys = 4;

	//auto totalChild = [](int x, int y) -> int { return x + y; };
	//auto totalChild = [=]() -> int { return girls + boys; };
	_functor totalChild;
	cout << "lambda = " << totalChild(girls, boys) << endl;

	auto totalChild2 = [=, &girls, &boys]() -> int { boys = 5; return girls + boys; };
	cout << "totalChild2 = " << totalChild2() << endl;

	cout << "girls = " << girls << endl;
	cout << "boys = " << boys << endl;

}

#endif
