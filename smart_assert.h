#ifndef SMART_ASSERT_H
#define SMART_ASSERT_H

#include <string>
#include <vector>
#include <utility>
#include <sstream>

class Assert
{
public:
    explicit Assert(): SMART_ASSERT_A(*this), SMART_ASSERT_B(*this) 
    {
    }
    Assert& SMART_ASSERT_A;
    Assert& SMART_ASSERT_B;
    //whatever member functions
    Assert& print_current_val(bool, const char*);
};

Assert& Assert::print_current_val(bool, const char*)
{
    return *this;
}

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
}

#define SMART_ASSERT_OP(x, next)\
    SMART_ASSERT_A.print_current_val((x), #x).SMART_ASSERT_ ## next

#define SMART_ASSERT_A(x) SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x) SMART_ASSERT_OP(x, A)

#define SMART_ASSERT( expr ) \
    if( (expr) ) ;\
    else make_assert(#expr).print_context(__FILE__, __LINE__).SMART_ASSERT_A;

#endif
