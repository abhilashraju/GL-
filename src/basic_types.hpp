#pragma once
/*
MIT License

Copyright (c) 2021 abhilashraju

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
namespace gl{
    template<typename T>
    struct ArrayView
    {
        using value_type = T;
        const T* d{ nullptr };
        size_t sz{0};
        ArrayView(const T* d, size_t s) :d(d),sz(s) {}
        const T* data()const{return d;}
        const T& at(size_t index)const { assert(index < size());  return *(data() + index); }
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
