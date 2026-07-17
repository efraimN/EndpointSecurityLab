#pragma once

template<class T>
struct remove_ref 
{ 
    typedef T type; 
};

// template<class T>
// struct remove_ref<T&>
// { 
//     typedef T type; 
// };

template<class T>
struct remove_ref<T&&> 
{ 
    typedef T type; 
};

template<class F>
class endfun {
public:
    endfun(F* p) : m_p(p) {}
    ~endfun() { (*m_p)(); }
private:
    F* m_p;
};

#define LEAVE(L) \
    auto&& end = (L); \
    typedef typename remove_ref<decltype(end)>::type xx; \
    endfun<xx> __xx__yy__zz__(&end)

#if 0
// This is an example of use
unsigned testFunction()
{
    unsigned retx = 50;

    LEAVE([&]() {
        // cleanup / actions at end of scope
        // ...
    });

    if (/* fail 1 */ false) return 1;   // early return: will execute 'end'
    if (/* fail 2 */ false) return 2;   // early return: will execute 'end'

    // normal work...
    return retx;        // on exit, will execute 'end'
}
#endif