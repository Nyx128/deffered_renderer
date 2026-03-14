#include "resource_tracker.hpp"
#include <iostream>
#include <iomanip>

namespace utils {

    size_t resource_tracker::s_ram_usage = 0;
    size_t resource_tracker::s_vram_usage = 0;

    void resource_tracker::allocate_ram(size_t size) {
        s_ram_usage += size;
    }

    void resource_tracker::free_ram(size_t size) {
        if (size > s_ram_usage) {
            // Should not happen if tracking is correct
            s_ram_usage = 0;
        } else {
            s_ram_usage -= size;
        }
    }

    void resource_tracker::allocate_vram(size_t size) {
        s_vram_usage += size;
    }

    void resource_tracker::free_vram(size_t size) {
         if (size > s_vram_usage) {
            s_vram_usage = 0;
        } else {
            s_vram_usage -= size;
        }
    }

    size_t resource_tracker::get_ram_usage() {
        return s_ram_usage;
    }

    size_t resource_tracker::get_vram_usage() {
        return s_vram_usage;
    }

    void resource_tracker::log_usage() {
        double ram_mb = static_cast<double>(s_ram_usage) / (1024.0 * 1024.0);
        double vram_mb = static_cast<double>(s_vram_usage) / (1024.0 * 1024.0);
        
        std::cout << "[Resource Tracker] RAM: " << std::fixed << std::setprecision(2) << ram_mb << " MB | VRAM: " << vram_mb << " MB" << std::endl;
    }

}
