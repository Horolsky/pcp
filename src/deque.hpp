#pragma once
#include <iostream>

typedef unsigned long int size_t;

template <int N>
inline int mod(int x)
{
    return (N & (N - 1)) ? (x % N) : (x & (N-1));
}

//fixed size deque
template <class T, size_t N>
class deque
{    
public:
    typedef T value_type;

    size_t capacity() {return N; }
    bool empty() const { return m_size == 0; }
    size_t size() const { return m_size; }

    void push_front(const T &value)
    {
        if (m_size == N) throw std::overflow_error("deque overflow");
        m_data[m_head] = value;
        m_head = mod<N>(m_head + 1);
        m_size += 1;
    }
    void push_back(const T &value)
    {
        if (m_size == N) throw std::overflow_error("deque overflow");
        m_data[mod<N>(N+m_head-(m_size++)-1)] = value;
    }
    T pop_front()
    {
        if (m_size == 0) throw std::underflow_error("deque underflow");
        m_size -= 1;
        return m_data[m_head = mod<N>(m_head+N-1)];
    }
    T pop_back()
    {
        if (m_size == 0) throw std::underflow_error("deque underflow");
        return m_data[mod<N>(m_head+N-(m_size--))];
    }
    T operator[](size_t i) const
    {
        if (i >= m_size) throw std::overflow_error("deque index overflow");
        return m_data[mod<N>(m_head+N-i-1)];
    }
    void print() const
    {
        for (int i = 0 ; i < m_size; ++i)
        {
            std::cout << m_data[mod<N>(m_head+N-i-1)] << ", ";
        }
        std::cout << "\n";
    }
    void clear()
    {
        m_head = 0;
        m_size = 0;
    }
    // debug func: internal buffer
    void _bf() const
    {
        for (int i = 0 ; i < N; ++i)
        {
            std::cout << m_data[i] << ", ";
        }
        std::cout << "\n";
    }
    // debug func: head
    int _hd() const { return m_head; }
private:
    
    T m_data[N];
    int m_head{0};
    int m_size{0};
};
