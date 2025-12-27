#include "ArenaAllocator.h"

#include <memory>

namespace Radium
{
    ArenaAllocator::ArenaAllocator(const size_t size) : m_size(size) {
        m_base = malloc(size);
        m_pos = m_base;
    }

    ArenaAllocator::~ArenaAllocator()
    {
        free(m_base);
    }
}
