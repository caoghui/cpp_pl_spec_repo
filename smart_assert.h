#ifndef SMART_ASSERT_H
#define SMART_ASSERT_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <typeinfo>
#include <sstream>
#include <cxxabi.h>

using namespace std;

std::string demangle(const std::string& name)
{
	size_t funcnamesize = 1;
	char* funcname = static_cast<char*>(malloc(funcnamesize));
	int status;
	char* ret = abi::__cxa_demangle(name.c_str(), funcname, &funcnamesize, &status);
    if (0 == status)
    {
    	//demangle success
    	std::string result(ret);
    	free(ret);
    	return result;
    }
    else
    {
    	//demangle fail
    	free(funcname);
    	return name;
    }
}

class Assert
{
    std::string m_expr;
public:
    explicit Assert(const char* expr):  m_expr(expr), SMART_ASSERT_A(*this), SMART_ASSERT_B(*this)
    {
    }
    Assert& SMART_ASSERT_A;
    Assert& SMART_ASSERT_B;
    //whatever member functions
    template<typename T>
    Assert& print_current_val(const char* name, const T& value)
    {
        cout << "\t" << demangle(typeid(T).name()) << " " << name << " = " << value <<endl;
        return *this;
    }
    Assert& print_context(const char* file, const int line)
    {
        cout << "Assertion failed in " << "[" << file << " : " << line << "]" << endl;
        cout << "Expr : '" << expr() << "'" << endl;
        cout << "Vals : " << endl;
        return *this;
    }
    const std::string& expr()const
    {
        return m_expr;
    }
};

Assert make_assert(const char* expr)
{
    return Assert(expr);
}

/*
如何理解这个技巧：当预处理器看到SMART_ASSERT_A(或_B)后面跟着一对括号时，它就将
这个当成对宏的调用来对待。如果没有括号，预处理器就简单地将这个符号仍然留在那里。而
在后一种情况下，符号SMART_ASSERT_A(或_B)只是代表成员变量。
*/
#define SMART_ASSERT_OP(x, next)\
    SMART_ASSERT_A.print_current_val(#x, (x)).SMART_ASSERT_##next

#define SMART_ASSERT_A(x) SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x) SMART_ASSERT_OP(x, A)

#define SMART_ASSERT( expr ) \
    if( (expr) ) ;\
    else make_assert(#expr).print_context(__FILE__, __LINE__).SMART_ASSERT_A



class assert_context
{
public:
    //where the assertion failed : file&line
    std::string get_context_file() const;
    int get_context_line() const;

    //get/set expression
    void set_expr(const std::string& str);
    const std::string& get_expr() const;

    typedef std::pair<std::string, std::string> val_and_str;
    typedef std::vector<val_and_str> vals_array;
    //return values array as a vector of pairs: [Value, corresponding string]
    const vals_array& get_vals_array() const;
    //adds one value and its corresponding string
    int add_val(const std::string& val, const std::string& str);

    //get/set level of assertion
    void set_level(int nLevel);
    int get_level() const;

    //get/set user-friendly message
    void set_level_msg(const char* strMsg);
    const std::string& get_level_msg() const;

    const std::string& msg() const;
};

//show a message box with two buttons: ignore and ignore all
void customerfriendly_handler(const assert_context& ctx)
{
    /*
    static bool ignore_all = false;
    if(ignore_all)
        return;
    std::ostringstream out;
    if(ctx.msg().size() > 0)
    {
        out << ctx.msg();
    }
    else
    {
        out << "Expression: '" << ctx.get_expr() << " ' failed!";
    }
    int result =  message_box(out.str());
    if(result == do_ignore_all)
    {
        ignore_all = true;
    }
    */
}

#endif
