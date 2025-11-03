#pragma once
#include <iostream>
#include <mutex>
#include <sstream>

namespace General
{
    class ThreadSafeCout
    {
    public:
        struct endl_statement {};

        template <typename Type>
        ThreadSafeCout& operator << (const Type& value);
        ThreadSafeCout& operator << (const endl_statement& value);

    private:
        static std::mutex m_mutex;
        std::ostringstream m_buffer;
    };

    inline ThreadSafeCout cout;
    inline ThreadSafeCout::endl_statement endl;
}

template <typename Type>
General::ThreadSafeCout& General::ThreadSafeCout::operator << (const Type& value)
{
    std::lock_guard<std::mutex> lock(General::ThreadSafeCout::m_mutex);
    this->m_buffer << value;
    return *this;
}
