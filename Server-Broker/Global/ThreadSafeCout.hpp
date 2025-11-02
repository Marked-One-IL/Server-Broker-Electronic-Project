#pragma once
#include <iostream>
#include <mutex>
#include <sstream>

namespace Global
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
Global::ThreadSafeCout& Global::ThreadSafeCout::operator << (const Type& value)
{
    this->m_buffer << value;
    return *this;
}
