#pragma once

#include <chrono>
#include <string>

namespace utils {
    class scope_timer {
    public:
        // If name is provided, destructor prints time.
        scope_timer(const char* name = nullptr);
        ~scope_timer();

        // Get elapsed time in specific units
        double get_seconds() const;
        double get_milliseconds() const;
        double get_microseconds() const;

    private:
        const char* m_name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };

}
