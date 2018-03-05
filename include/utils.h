/*!
    \file utils.h
    \brief Contains the definition of various utility functions.
    \author Wojciech Jarosz
    
    CS 89/189 Computational Aspects of Digital Photography C++ basecode.
*/
#pragma once

#include <chrono>


//! Clamps a value between two bounds.
/*!
    \param a The value to test.
    \param l The lower bound.
    \param h The upper bound.
    \return The value \a a clamped to the lower and upper bounds.
    
    This function has been specially crafted to prevent NaNs from propagating.
*/
template <typename T>
inline T clamp(T a, T l, T h)
{
    return (a >= l) ? ((a <= h) ? a : h) : l;
}


//! Returns a modulus b.
template <typename T>
inline T mod(T a, T b)
{
    int n = (int)(a/b);
    a -= n*b;
    if (a < 0)
        a += b;
    return a;
}


/*!
 * @brief Python-style range: iterates from min to max in range-based for loops
 *
 * To use:
 *  @code
 *     for(int i = 0; i < 100; i++) { ... }             // old way
 *     for(auto i : range(100))     { ... }             // new way
 *
 *     for(int i = 10; i < 100; i+=2)  { ... }          // old way
 *     for(auto i : range(10, 100, 2)) { ... }          // new way
 *
 *     for(float i = 3.5f; i > 1.5f; i-=0.01f) { ... }  // old way
 *     for(auto i : range(3.5f, 1.5f, -0.01f)) { ... }  // new way
 * @endcode
*/
template<typename T>
class Range
{
public:
    class Iterator
    {
    public:
        Iterator(T pos, T step) : m_pos(pos), m_step(step) {}

        bool operator!=(const Iterator &o) const { return (o.m_pos - m_pos) * m_step > T(0);}
        Iterator &operator++() {m_pos += m_step; return *this;}
        Iterator operator++(int) {Iterator copy(*this); operator++(); return copy;}
        T operator*() const {return m_pos;}
    private:
        T m_pos, m_step;
    };

    Range(T start, T end, T step = T(1))
        : m_start(start), m_end(end), m_step(step) {}

    Iterator begin() const {return Iterator(m_start, m_step);}
    Iterator end() const {return Iterator(m_end, m_step);}

private:
    T m_start, m_end, m_step;
};

template<typename T>
Range<T> range(T end) {return Range<T>(T(0), end, T(1));}

template<typename T>
Range<T> range(T start, T end, T step = T(1)) {return Range<T>(start, end, step);}


/*!
    @brief Simple timer with millisecond precision.

    This class is convenient for collecting performance data
*/
class Timer
{
public:
    //! Create a new timer and reset it
    Timer()
    {
        reset();
    }

    //! Reset the timer to the current time
    void reset()
    {
        start = std::chrono::system_clock::now();
    }

    //! Return the number of milliseconds elapsed since the timer was last reset
    double elapsed() const
    {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        return (double) duration.count();
    }

    //! Return the number of milliseconds elapsed since the timer was last reset and then reset it
    double lap()
    {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        start = now;
        return (double) duration.count();
    }

private:
    std::chrono::system_clock::time_point start;
};