#include "../Global/ThreadSafeCout.hpp"

std::mutex Global::ThreadSafeCout::m_mutex;

Global::ThreadSafeCout& Global::ThreadSafeCout::operator << (const endl_statement& value)
{
    std::lock_guard<std::mutex> lock(this->m_mutex);
    std::cout << this->m_buffer.str() << std::endl;
    this->m_buffer.str("");
    this->m_buffer.clear();
    return *this;
}
