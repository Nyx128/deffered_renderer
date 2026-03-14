#pragma once

#include <cstddef>
#include <string>

namespace utils {

    class resource_tracker {
    public:
        static void allocate_ram(size_t size);
        static void free_ram(size_t size);
        
        static void allocate_vram(size_t size);
        static void free_vram(size_t size);

        static size_t get_ram_usage();
        static size_t get_vram_usage();

        static void log_usage();

    private:
        static size_t s_ram_usage;
        static size_t s_vram_usage;
    };

}
