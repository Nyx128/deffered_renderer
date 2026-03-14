#include "scope_timer.hpp"
#include <iostream>

namespace utils {

    scope_timer::scope_timer(const char* name) 
        : m_name(name), m_start(std::chrono::high_resolution_clock::now()) 
    {
    }

    scope_timer::~scope_timer() {
        if (m_name) {
            std::cout << "[TIMER] " << m_name << " took " << get_milliseconds() << " ms" << std::endl;
        }
    }

    double scope_timer::get_seconds() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - m_start).count();
    }

    double scope_timer::get_milliseconds() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - m_start).count();
    }

    double scope_timer::get_microseconds() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::micro>(end - m_start).count();
    }

}
