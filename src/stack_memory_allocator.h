#pragma once

#include <cstddef>
#include <array>
#include <boost/pool/simple_segregated_storage.hpp>
#include <iostream>

namespace {

    constexpr std::size_t GetNextPowerOf2(std::size_t value) {
        // Decrement in case the value is a power of 2 already.
        --value;

        // Set all bits after the last set bit.
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;

        return ++value; // Give 1 back.
    }

}

namespace OrderBook {

    /**
     * This allocator allows preallocating the required memory on the stack based on the maximum number
     * of elements in the flat_map.
     *
     * By preallocating memory on the stack, heap allocations are avoided, resulting in improved performance.
     * Additionally, the use of StackMemoryAllocator ensures that the container can hold all the elements
     * without requiring dynamic reallocation when it reaches its maximum capacity.
     * This eliminates the potential performance overhead associated with reallocation.
     *
     * Furthermore, using StackMemoryAllocator also eliminates object construction and destruction during reallocation.
     * Since the required memory is preallocated on the stack, the container does not need to reallocate its elements.
     * As a result, unnecessary object creation and destruction overhead is reduced, improving performance and efficiency.
    */
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
        // The size of memory allocated for flat_map is the next power of 2
        static constexpr std::size_t Capacity = GetNextPowerOf2(MaxNumberOfElements);
        alignas(T)  std::array<char, Capacity * sizeof(T)> Arena_;
        char* NextConstructed_ = Arena_.data();

    public:
        StackMemoryAllocator() {
        }

        // Copying or moving instances of StackMemoryAllocator is disabled,
        // ensuring that memory management remains consistent and localized.
        StackMemoryAllocator(const StackMemoryAllocator& rhs) = delete;
        StackMemoryAllocator& operator=(const StackMemoryAllocator& rhs) = delete;
        StackMemoryAllocator(StackMemoryAllocator&& rhs) = delete;
        StackMemoryAllocator& operator=(StackMemoryAllocator&& rhs) = delete;

        // All objects allocated by the allocator should be destructed at the end of it's lifetime.
        ~StackMemoryAllocator() {
            for (char* address = Arena_.data(); address != NextConstructed_; address += sizeof(T)) {
                reinterpret_cast<T*>(address)->~T();
            }
        }

        value_type* allocate(std::size_t n) {
            // The flat_map, which is aligned to the map managed by Arena_,
            // allocates a sequential block to store all items.
            // Instead of reallocation of a new block of bigger size, we reuse the same block all the time.
            return reinterpret_cast<value_type*>(&Arena_);
        }

        void deallocate(value_type*, std::size_t) {
            // Since no memory is actually allocated, there is nothing to deallocate.
            // pass
        }

        template <typename U, typename... Args>
        void construct(U* p, Args&&... args) {
            assert(reinterpret_cast<char*>(p) <= NextConstructed_);

            // Since we don't actually allocate a new block of memory but use an existing one,
            // we should not call the constructor twice for objects that have already been created.
            // NextConstructed_ keeps track of the next address where the constructor is not called yet.
            if (reinterpret_cast<char*>(p) == NextConstructed_) {
                ::new ((void*)p) U(std::forward<Args>(args)...);
                NextConstructed_ += sizeof(T);
            }
        }

        template<class U>
        void destroy(U*) {
            // Since we reuse memory, there is nothing to destroy.
            // All created objects will be destroyed in the destructor of StackMemoryAllocator.
            // pass
        }
    };


}
