#pragma once

#include <boost/container/flat_map.hpp>
#include <optional>
#include "models/price_quantity.h"
#include "utils/generator.h"
#include "stack_memory_allocator.h"

namespace OrderBook {

    template <typename T, typename TValue>
    concept InputRange = requires(T&& range) {
        std::input_iterator<std::ranges::iterator_t<decltype(range)>>;
        std::is_same_v<typename std::ranges::iterator_t<decltype(range)>::value_type, TValue>;
    };

    template <typename TPrice, typename TQuantity, typename TKeyComparator, size_t Capacity>
    class OrderMap {
    private:
        using TBestOrder = Models::PriceQuantity<TPrice, TQuantity>;
        using TOrdersMap = boost::container::flat_map<TPrice, TQuantity,
                                                      TKeyComparator,
                                                      StackMemoryAllocator<std::pair<TPrice, TQuantity>, Capacity + 1>>;

        TBestOrder BestOrder_;
        TOrdersMap Orders_;
        TKeyComparator Comparator;

    public:
        [[nodiscard]]
        bool IsEmpty() const {
            return Orders_.empty();
        }

        void Clear() {
            Orders_.clear();
        }

        void UpdateBestOrder(Models::PriceQuantity<TPrice, TQuantity> update) {
            BestOrder_ = update;

            // in case of receiving BBO update before Depth Update
            if (IsEmpty()) [[unlikely]] {
                Orders_.emplace(update.Price, update.Quantity);
            }
        }

        auto UpdateOrders(InputRange<Models::PriceQuantity<TPrice, TQuantity>> auto&& updates) {
            std::optional<typename TOrdersMap::const_iterator> hint;

            for (auto&& update : updates) {
                hint = UpdateOrder(update, hint);
            }
        }

        [[nodiscard]]
        auto Extract() const -> Utils::Generator<Models::PriceQuantity<TPrice, TQuantity>> {
            if (IsEmpty()) {
                co_return;
            }

            co_yield BestOrder_;

            for (auto [price, quantity] : Orders_) {
                // return all orders under the best order
                // all orders with better price are stale
                if (Comparator(BestOrder_.Price, price)) {
                    co_yield Models::PriceQuantity<TPrice, TQuantity> {
                        .Price = price,
                        .Quantity = quantity,
                    };
                }
            }
        }

    private:
        auto UpdateOrder(Models::PriceQuantity<TPrice, TQuantity> update,
                         std::optional<typename TOrdersMap::const_iterator> hint = std::nullopt) {
            if (update.Quantity > 0) {
                auto it = hint.has_value()
                        ? Orders_.try_emplace(hint.value(), update.Price, update.Quantity)
                        : Orders_.try_emplace(update.Price, update.Quantity).first;

                if (it == Orders_.begin()) {
                    BestOrder_ = update;
                }

                const bool insertionHadEffect = it->second == update.Quantity;

                if (!insertionHadEffect) {
                    it->second = update.Quantity;
                } else if (Orders_.size() > Capacity) {
                    Orders_.erase(std::prev(Orders_.end()));
                    it = Orders_.begin();
                }

                return it;
            } else {
                auto it = Orders_.lower_bound(update.Price);
                if (it != Orders_.end() && it->first == update.Price) {
                    it = Orders_.erase(it); // get next item after deleted

                    if (update.Price == BestOrder_.Price && !IsEmpty()) {
                        UpdateBestOrder({
                            .Price = Orders_.begin()->first,
                            .Quantity = Orders_.begin()->second,
                        });
                    }
                }

                return it;
            }
        }
    };

}