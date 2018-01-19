#ifndef SCOPE_GUARD_H
#define SCOPE_GUARD_H

#include <algorithm>

//C++中设计ScopeGuard的关键技术：通过局部变量析构函数来管理资源，根据是否是正常退出来确定是佛需要清理资源。

template <typename F>
class ScopeGuard 
{
private:
    F m_func;
    bool m_dismiss;
    ScopeGuard();  //默认构造函数
    ScopeGuard(const ScopeGuard&);  //拷贝构造函数
    ScopeGuard& operator=(const ScopeGuard&);  //拷贝赋值运算符
public:
    explicit ScopeGuard(F && f) : m_func(std::move(f)), m_dismiss(false)
    { }
    explicit ScopeGuard(const F& f) : m_func(f), m_dismiss(false)
    { }
    ~ScopeGuard()
    {
        if(!m_dismiss)
        {
            m_func();
        }
    }
    ScopeGuard(ScopeGuard && rhs) : m_func(std::move(rhs.m_func)), m_dismiss(rhs.m_dismiss)
    {
        rhs.Dismiss();
    }
    //为了支持rollback模式
    //example:
    /*
    ScopeGuard onFailureRollback([&] { // roolback code; });
    ...//do something that could fail
    onFailureRollback.Dismiss();

    说明："do something"的过程中只要任何地方抛出异常，rollback逻辑都会被执行。如果do something成功了，onFailureRollback.Dismiss()
    会被调用，设置m_dismiss为true，阻止rollback逻辑的执行。

    */
    void Dismiss()   
    {
        m_dismiss = true;
    }

    /*
    template<class Args>
    auto Run(Args&&... args)->typename std::result_of<F(Args...)>::type
    {
        return m_func(std::forward<Args>(args)...);
    }
    */
};

template <typename F>
ScopeGuard<typename std::decay<F>::type> MakeGuard(F && f)
{
    return ScopeGuard<typename std::decay<F>::type>(std::forward<F>(f));
}

 //宏定义
 #define SCOPEGUARD_LINENAME_CAT(name, line) name##line
 #define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line) 
 #define ON_SCOPE_EXIT(callback) ScopeGuard<std::function<void()>> SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

/*
C++的RAII范式被认为是资源确定性释放的最佳范式，有了ON_SCOPE_EXIT之后，在C++里面申请释放资源就变得非常方便了
Acquire Resource1
ON_SCOPE_EXIT([&] { // Release Resource1});
Acquire Resource2
ON_SCOPE_EXIT([&] { // Release Resource2 });
*/

#endif