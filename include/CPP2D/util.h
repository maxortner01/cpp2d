#pragma once

#include <cstddef>
#include <stdint.h>

#define FREE_CHECK(ptr) \
    if (ptr) { std::free(ptr); ptr = nullptr; }