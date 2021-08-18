#pragma once
namespace gl{
    template<typename T>
    struct ArrayView
    {
        using value_type = T;
        const T* d{ nullptr };
        size_t sz{0};
        ArrayView(const T* d, size_t s) :d(d),sz(s) {}
        const T* data()const{return d;}
        size_t size()const {return sz;}
    };

    template<typename Func1,typename Func2>
    struct decorate{
        Func1 f1;
        Func2 f2;
        decorate(Func1 f1, Func2 f2):f1(std::move(f1)),f2(std::move(f2)){}

        template<typename... Args>
        auto operator()(Args&&... args){
           static_assert( std::is_invocable<decltype(f1), Args...>::value );
           static_assert( std::is_invocable<decltype(f2), Args...>::value );
           return f1(std::forward<Args>(args)...),f2(std::forward<Args>(args)...);
        }
        
    };
}
