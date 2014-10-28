/*
 * Copyright (c) 2011-2012, Attila Gobi and Zalan Szugyi
 * All rights reserved.
 *
 * This software was developed by Attila Gobi and Zalan Szugyi.
 * The project was supported by the European Union and co-financed by the
 * European Social Fund (grant agreement no. TAMOP 4.2.1./B-09/1/KMR-2010-0003)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __stream_h__
#define __stream_h__

#include <iostream>
#include <utility>
#include <ctime>
#include <cassert>

#if __GNUC__ > 4 || \
          (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
#define HAVE_LITERALS
#endif

template<typename T>
struct storage_type
{
    typedef typename std::conditional<
        std::is_lvalue_reference<typename std::remove_const<T>::type>::value,
        typename std::remove_const<T>::type,
        typename std::remove_reference<typename std::remove_const<T>::type>::type
    >::type type;
};

template<typename T>
struct stream
{
protected:
    struct impl;

public:
    typedef T value_type;

    stream(const stream<T> &o)
        : impl_(o.impl_->clone())
    {
    }

    stream(stream<T> &&o)
        : impl_(0)
    {
        std::swap(impl_, o.impl_);
    }

    stream<T> & operator = (stream<T> &&o)
    {
        if(this != &o) {
            std::swap(impl_, o.impl_);
            return *this;
        }
    }

    ~stream()
    {
        delete impl_;
    }

    struct iterator {
        iterator& operator ++()
        {
            (*impl_)->next(*this);
            return *this;
        }

        const T& operator *() const
        {
            return (*impl_)->get(*this);
        }

        friend struct stream<T>;

    protected:
        iterator(impl ** impl)
            :impl_(impl)
        { }

        impl **impl_;
    };

    iterator begin() const
    {
        return iterator(&impl_);
    }

protected:

    struct impl {
        virtual ~impl() {}
        virtual const T &get(const iterator &) = 0;
        virtual void next(iterator &) = 0;
        virtual impl* clone() = 0;
    };

    mutable impl * impl_;

    stream(impl *i)
        :impl_(i)
    { }

    template<typename ST>
    struct addimpl: public impl {
        addimpl(const T &a, ST &&s)
            : a_(a), s_(std::forward<ST>(s))
        { }

        const T &get(const iterator &)
        {
            return a_;
        }

        void next(iterator &it)
        {
            it.impl_ = const_cast<impl**>(&s_.impl_);
        }

        impl *clone()
        {
            return new addimpl<ST>(a_, std::forward<ST>(s_));
        }

    private:
        const T a_;
        typename storage_type<ST>::type s_;
    };

    template<typename Op, typename ST>
    struct mapimpl2: public impl {
        mapimpl2(Op op, ST &&s)
            :s_(std::forward<ST>(s)),
            it1(s_.begin()), op_(op)
        {
        }

        const T &get(const iterator &it)
        {
            *(it.impl_)=new addimpl<stream<T>&&>(op_(*it1), stream<T>(this));
            ++it1;
            return *it;
        }

        void next(iterator &it)
        {
            *(it.impl_)=new addimpl<stream<T>&&>(op_(*it1), stream<T>(this));
            ++it1;
            ++it;
        }

        impl *clone()
        {
            return new mapimpl2<Op, ST>(op_, std::forward<ST>(s_));
        }
    private:
        typename storage_type<ST>::type s_;
        iterator it1;
        Op op_;
    };

    template<typename Op, typename ST>
    struct mapimpl: public impl {
        mapimpl(Op op, ST &&s)
            : s_(std::forward<ST>(s)), op_(op)
        { }

        const T &get(const iterator &it)
        {
            impl *x = new mapimpl2<Op, ST>(op_, std::forward<ST>(s_));
            *(it.impl_) = x;

            delete this;
            return *it;
        }

        void next(iterator &it)
        {
            impl *x = new mapimpl2<Op, ST>(op_, std::forward<ST>(s_));
            *(it.impl_) = x;
            delete this;
            ++it;
        }

        impl *clone()
        {
            return new mapimpl<Op, ST>(op_, std::forward<ST>(s_));
        }

    private:
        typename storage_type<ST>::type s_;
        Op op_;
    };

    template<typename Op, typename ST1, typename ST2>
    struct zipimpl: public impl
    {
        zipimpl(Op op, ST1 &&s1, ST2 &&s2)
            :s1_(std::forward<ST1>(s1)), s2_(std::forward<ST2>(s2)), op_(op)
        { }

        const T &get(const iterator &it)
        {
            impl *x = new zipimpl2<Op, ST1, ST2>(op_, std::forward<ST1>(s1_), std::forward<ST2>(s2_));
            *(it.impl_) = x;
            delete this;
            return *it;
        }

        void next(iterator &it)
        {
            impl *x = new zipimpl2<Op, ST1, ST2>(op_, std::forward<ST1>(s1_), std::forward<ST2>(s2_));
            *(it.impl_) = x;
            delete this;
            ++it;
        }

        impl *clone()
        {
            return new zipimpl<Op, ST1, ST2>(op_, std::forward<ST1>(s1_), std::forward<ST2>(s2_));
        }
    private:
        typename storage_type<ST1>::type s1_;
        typename storage_type<ST2>::type s2_;
        Op op_;
    };

    template<typename Op, typename ST1, typename ST2>
    struct zipimpl2: public impl
    {
        zipimpl2(Op op, ST1 &&s1, ST2 &&s2)
            :s1_(std::forward<ST1>(s1)),s2_(std::forward<ST2>(s2)),
            it1(s1_.begin()), it2(s2_.begin()), op_(op)
        {
        }

        const T &get(const iterator &it)
        {
            *(it.impl_)=new addimpl<stream<T>&&>(op_(*it1, *it2), stream<T>(this));
            ++it1;
            ++it2;
            return *it;
        }

        void next(iterator &it)
        {
            *(it.impl_)=new addimpl<stream<T>&&>(op_(*it1, *it2), stream<T>(this));
            ++it1;
            ++it2;
            ++it;
        }

        impl *clone()
        {
            return new zipimpl2<Op, ST1, ST2>(op_, std::forward<ST1>(s1_), std::forward<ST2>(s2_));
        }
    private:
        typename storage_type<ST1>::type s1_;
        typename storage_type<ST2>::type s2_;
        iterator it1, it2;
        Op op_;
    };

public:
    template <typename S, typename U>
    friend stream<U> operator <<= (const U& a, S && s);

    template <typename Op, typename ST1, typename ST2>
    static stream<T> zipwith(Op op, ST1 &&s1, ST2 &&s2)
    {
        return stream(new zipimpl<Op, decltype(s1), decltype(s2)>(op, std::forward<ST1>(s1), std::forward<ST2>(s2)));
    }

    template <typename ST1, typename Op>
    static stream<T> map(Op op, ST1 &&s1)
    {
        return stream(new mapimpl<Op, decltype(s1)>(op, std::forward<ST1>(s1)));
    }

    static stream<T> pure(const T& v)
    {
        stream<T> s = v<<=s;
        return s;
    }
};

template<typename ST>
struct stream_value_type
{
    typedef typename std::remove_reference<ST>::type::value_type type;
};

template <typename S, typename U=typename stream_value_type<S>::type>
stream<U> operator <<= (const U& a, S && s)
{
    return stream<U>(new typename stream<U>::template addimpl<decltype(s)>(a, std::forward<S>(s)));
}

template<typename ST1, typename ST2, typename T=typename stream_value_type<ST1>::type>
stream<T> operator +(ST1 &&s1, ST2 &&s2)
{
    return stream<T>::zipwith(std::plus<T>(), std::forward<ST1>(s1), std::forward<ST2>(s2));
}

template<typename ST1, typename ST2, typename T=typename stream_value_type<ST1>::type>
stream<T> operator -(ST1 &&s1, ST2 &&s2)
{
    return stream<T>::zipwith(std::minus<T>(), std::forward<ST1>(s1), std::forward<ST2>(s2));
}

template<typename ST1, typename ST2, typename T=typename stream_value_type<ST1>::type>
stream<T> operator *(ST1 &&s1, ST2 &&s2)
{
    return stream<T>::zipwith(std::multiplies<T>(), std::forward<ST1>(s1), std::forward<ST2>(s2));
}

template<typename ST1, typename ST2, typename T=typename stream_value_type<ST1>::type>
stream<T> operator /(ST1 &&s1, ST2 &&s2)
{
    return stream<T>::zipwith(std::divides<T>(), std::forward<ST1>(s1), std::forward<ST2>(s2));
}

template<typename ST1, typename ST2, typename T=typename stream_value_type<ST1>::type>
stream<T> operator %(ST1 &&s1, ST2 &&s2)
{
    return stream<T>::zipwith(std::modulus<T>(), std::forward<ST1>(s1), std::forward<ST2>(s2));
}

template<typename ST, typename T=typename stream_value_type<ST>::type>
stream<T> operator -(ST &&s)
{
    return stream<T>::map(std::negate<T>(), std::forward<ST>(s));
}

#ifdef HAVE_LITERALS

struct stream_proxy
{
    stream_proxy(unsigned long long i): x(i) {}
    template <typename T>
    operator stream<T> () {
        stream<T> a = static_cast<T>(x)<<=a;
        return a;
    }
    unsigned long long x;
};

stream_proxy operator "" _s (unsigned long long i)
{
    return stream_proxy(i);
}

#endif//HAVE_LITERALS

#endif//__stream_h__
