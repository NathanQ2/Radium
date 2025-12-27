#pragma once

#include <cstddef>
#include <__algorithm/ranges_for_each_n.h>

namespace Radium
{
    class ArenaAllocator
    {
    public:
        explicit ArenaAllocator(size_t size);
        
        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;
        
        ~ArenaAllocator();
        
        template<typename T>
        [[nodiscard]] T* alloc();
    private:
        // TODO: Change to std::byte* so we can easily do pointer arithmetic in 'alloc' function
        void* m_base;
        const size_t m_size;
        
        void* m_pos;
    };
    
    template <typename T>
    T* ArenaAllocator::alloc()
    {
        void* ptr = m_pos;
        m_pos = static_cast<void*>(static_cast<char*>(m_pos) + sizeof(T));
        
        return static_cast<T*>(ptr);
    }
}
