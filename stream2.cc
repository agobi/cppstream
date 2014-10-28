#include <utility>
#include <type_traits>
#include <iostream>
#include <functional>
#include <list>

template<typename T> class stream;
template<typename T> class stream_impl;
template<typename T> class stream_iterator_impl;
template<typename T> class iterator_temp;

template <typename T>
using storage_type = typename std::conditional<
        std::is_rvalue_reference<T>::value,
        typename std::remove_reference<T>::type,
        T
    >;

template<typename T>
class is_stream : public std::integral_constant<bool, false>
{};

template<typename T>
class is_stream<stream<T>> : public std::integral_constant<bool, true>
{};

template<typename T>
class stream
{
public:
    typedef T value_type;

    stream(stream &) = delete;
    stream(stream &&s): _impl(0)
    {
        std::swap(_impl, s._impl);
    }

    stream(stream_impl<T> * impl)
        : _impl(impl)
    {}

    ~stream()
    {
        delete _impl;
    }

    struct iterator
    {
        iterator()
            : _tmp(new iterator_temp<T>(*this))
        {}

        iterator(iterator &) = delete;
        iterator(iterator &&rhs)
        {
            std::swap(_its, rhs._its);
            std::swap(_tmp, rhs._tmp);
        }

        iterator &
        operator = (iterator &&rhs)
        {
            std::swap(_its, rhs._its);
            return *this;
        }

        iterator &operator ++ ()
        {
            _tmp->step();
            for(std::unique_ptr<stream_iterator_impl<T>> &p : _its)
                p->step();

            return *this;
        }

        const T& operator *()
        {
            return _its.front()->value();
        }

        std::list<std::unique_ptr<stream_iterator_impl<T>>> _its;
        std::unique_ptr<stream_iterator_impl<T>> _tmp;
    };

    iterator begin()
    {
        iterator it;
        _impl->build(it);
        return it;
    }

private:
    stream_impl<T> * _impl;
};

template <typename T>
class stream_iterator_impl
{
public:
    inline stream_iterator_impl() {}
    inline virtual ~stream_iterator_impl() {}
    stream_iterator_impl(stream_iterator_impl&) = delete;
    stream_iterator_impl(stream_iterator_impl&&) = delete;
    virtual void step() = 0;
    virtual const T & value() = 0;
};

template <typename T>
class stream_impl
{
public:
    inline stream_impl() {}
    stream_impl(stream_impl&) = delete;
    stream_impl(stream_impl&&) = delete;
    inline virtual ~stream_impl() {}
    virtual void build(typename stream<T>::iterator &) = 0;
};

template<typename T>
class iterator_temp: public stream_iterator_impl<T>
{
public:
    iterator_temp(typename stream<T>::iterator& s): _s(s) {}
    virtual void step() { _value = *_s; }
    virtual const T & value() { return _value; };
    typename stream<T>::iterator &_s;
    T _value;
};

template<typename S>
class cons_impl : public stream_iterator_impl<S>
{
public:
    typedef typename std::remove_reference<S>::type::value_type value_type;

    cons_impl(const value_type & i, S && s)
        : _x(i), _xs(std::forward<S>(s))
    {}

private:
    value_type _x;
    typename storage_type<S>::type _xs;

    void build(typename stream<value_type>::iterator &)
    {

    }
};

template<typename S>
class cons : public stream_impl<typename std::remove_reference<S>::type::value_type>
{
public:
    typedef typename std::remove_reference<S>::type::value_type value_type;

    cons(const value_type & i, S && s)
        : _x(i), _xs(std::forward<S>(s))
    {}

private:
    value_type _x;
    typename storage_type<S>::type _xs;

    void build(typename stream<value_type>::iterator &)
    {

    }
};

template<typename T, typename std::enable_if<is_stream<typename std::remove_reference<T>::type>::value>::type* = nullptr>
typename std::remove_reference<T>::type operator <<= (typename std::remove_reference<T>::type::value_type i, T && s)
{
    return new cons<T>(i, std::forward<T>(s));
}

template<typename S, typename T, typename OP>
class binop : public stream_impl<typename std::remove_reference<S>::type::value_type>
{
public:
    typedef typename std::remove_reference<S>::type::value_type value_type;

    binop(S && lhs, T && rhs): _lhs(std::forward<S>(lhs)), _rhs(std::forward<T>(rhs))
    {}

    void build(typename stream<value_type>::iterator &)
    {
    }
private:
    typename storage_type<S>::type _lhs;
    typename storage_type<T>::type _rhs;
};

template<typename S, typename T, typename std::enable_if<is_stream<typename std::remove_reference<T>::type>::value>::type* = nullptr>
typename std::remove_reference<T>::type operator + (T && lhs, S && rhs)
{
    return new binop<S, T, std::plus<typename std::remove_reference<T>::type::value_type>>(std::forward<S>(lhs), std::forward<T>(rhs));
}

int main()
{
    stream<int> zeroes = 0 <<= zeroes;
    stream<int> ones = 1 <<= ones;
    stream<int> zo = 0 <<= 1 <<= zo;
    stream<int> increment = 0 <<= ones + increment;
//    stream<int> sums = 0 <<= sums + std::cin;

    stream<int>::iterator it = zeroes.begin();
    for(int i=0; i<100; ++i, ++it)
        std::cout<<*it<<std::endl;

    it = increment.begin();
    for(int i=0; i<100; ++i, ++it)
        std::cout<<*it<<std::endl;
}
