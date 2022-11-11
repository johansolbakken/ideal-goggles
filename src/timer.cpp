
#include "timer.hpp"

#include <iostream>

Timer::Timer(const std::string& name)
{
    m_name = name;
    m_start = std::chrono::system_clock::now();
}

Timer::~Timer()
{
    std::cout << "Timer '" + m_name << "': " << time() << "ms." << std::endl;
}

void Timer::reset()
{
    m_start = std::chrono::system_clock::now();
}

double Timer::time()
{
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start);
    return duration.count();
}
