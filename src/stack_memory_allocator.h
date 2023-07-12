#pragma once

#include <cstddef>
#include <array>
#include <boost/pool/simple_segregated_storage.hpp>
#include <iostream>

namespace {

    constexpr std::size_t GetNextPowerOf2(std::size_t value) {
        // decrement in case value is a power of 2 already
        --value;

        // set all bits after the last set bit
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;

        return ++value; // give 1 back
    }

}

namespace OrderBook {

    template <typename T, std::size_t MaxNumberOfElements>
    class StackMemoryAllocator {
    public:
        using mapped_type = T;
        using value_type = T;

        template <typename U>
        struct rebind {
            typedef StackMemoryAllocator<U, MaxNumberOfElements> other;
        };

    private:
        // The size of memory allocated for binary map is the next power of 2
        static constexpr std::size_t Capacity = GetNextPowerOf2(MaxNumberOfElements);
        alignas(T)  std::array<char, Capacity * sizeof(T)> Arena_;
        boost::simple_segregated_storage<std::size_t> Pool_;
        char* LastConstructed_ = Arena_.data();

    public:
        StackMemoryAllocator() {
            Pool_.add_block(&Arena_, Arena_.size(), sizeof(T));
        }

        StackMemoryAllocator(const StackMemoryAllocator& rhs) = delete;
        StackMemoryAllocator& operator=(const StackMemoryAllocator& rhs) = delete;

        StackMemoryAllocator(StackMemoryAllocator&& rhs) = delete;
        StackMemoryAllocator& operator=(StackMemoryAllocator&& rhs) = delete;

        value_type* allocate(std::size_t n) {
            return reinterpret_cast<value_type*>(&Arena_);
        }

        void deallocate(value_type*, std::size_t) {
            // pass
        }

        template <typename U, typename... Args>
        void construct(U* p, Args&&... args) {
            assert(reinterpret_cast<char*>(p) <= LastConstructed_);

            if (reinterpret_cast<char*>(p) == LastConstructed_) {
                ::new ((void*)p) U(std::forward<Args>(args)...);
                LastConstructed_ += sizeof(T);
            }
        }

        template<class U>
        void destroy(U*) {
            // pass
        }
    };


}
