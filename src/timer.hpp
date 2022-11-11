#pragma once

#include <chrono>
#include <string>

class Timer
{
public:
    Timer(const std::string& name);
    ~Timer();
    
    void reset();
    double time();

private:
    std::string m_name;
    std::chrono::system_clock::time_point m_start;
};
