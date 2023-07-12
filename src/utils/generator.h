#pragma once

#include <experimental/coroutine>
#include <type_traits>
#include <utility>
#include <exception>
#include <iterator>
#include <functional>

namespace OrderBook::Utils {

    template <typename T>
    class Generator {
        struct Promise {
            T value_;

            auto get_return_object() -> Generator {
                using Handle = std::experimental::coroutine_handle<Promise>;
                return Generator{Handle::from_promise(*this)};
            }

            auto initial_suspend() {
                return std::experimental::suspend_always();
            }

            auto final_suspend() noexcept {
                return std::experimental::suspend_always();
            }

            void return_void() {
            }

            void unhandled_exception() {
                throw;
            }

            auto yield_value(T&& value) {
                value_ = std::move(value);
                return std::experimental::suspend_always();
            }

            auto yield_value(const T& value) {
                value_ = value;
                return std::experimental::suspend_always();
            }
        };

        struct Sentinel {};

        struct Iterator {
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            std::experimental::coroutine_handle<Promise> Handle_;

            Iterator& operator++() {
                Handle_.resume();
                return *this;
            }

            void operator++(int) {
                (void)operator++();
            }

            T operator*() const {
                return Handle_.promise().value_;
            }

            T* operator->() const {
                return std::addressof(operator*());
            }

            bool operator==(Sentinel) const {
                return Handle_.done();
            }
        };

        std::experimental::coroutine_handle<Promise> Handle_;

        explicit Generator(std::experimental::coroutine_handle<Promise> handle) : Handle_(handle) {
        }

    public:
        using promise_type = Promise;

        Generator(Generator&& g) noexcept : Handle_(std::exchange(g.Handle_, {})) {
        }

        ~Generator() {
            if (Handle_) {
                Handle_.destroy();
            }
        }

        auto begin() {
            Handle_.resume();
            return Iterator{Handle_};
        }

        auto end() {
            return Sentinel{};
        }
    };

}
