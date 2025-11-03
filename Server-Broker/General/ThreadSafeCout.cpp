#include "../General/ThreadSafeCout.hpp"

std::mutex General::ThreadSafeCout::m_mutex;

General::ThreadSafeCout& General::ThreadSafeCout::operator << (const endl_statement& value)
{
    std::lock_guard<std::mutex> lock(General::ThreadSafeCout::m_mutex);
    std::cout << this->m_buffer.str() << std::endl;
    this->m_buffer.str("");
    this->m_buffer.clear();
    return *this;
}
