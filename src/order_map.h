#pragma once

#include <optional>

#include <boost/container/flat_map.hpp>

#include "models/price_quantity.h"
#include "utils/generator.h"
#include "stack_memory_allocator.h"

namespace OrderBook {

    template <typename T, typename TValue>
    concept InputRange = requires(T&& range) {
        std::input_iterator<std::ranges::iterator_t<decltype(range)>>;
        std::is_same_v<typename std::ranges::iterator_t<decltype(range)>::value_type, TValue>;
    };

    template <typename TPrice, typename TQuantity, typename TKeyComparator, size_t PriceLevels>
    class OrderMap {
    private:
        using TBestOrder = Models::PriceQuantity<TPrice, TQuantity>;

        /*
         * We use a flat_map data structure provided by Boost, to store the orders in sorted order.
         * This data structure is chosen because the number of values in the map is relatively small
         * and can benefit from the performance boost of cache locality.
         * The flat_map implementation stores the key-value pairs in a contiguous memory block, resulting
         * in improved cache locality and potentially faster lookup and iteration compared to other map
         * implementations.
         *
         * To optimize memory allocation and improve performance, flat_map utilizes a custom StackMemoryAllocator.
         * This allocator allows preallocating the required memory on the stack based on the maximum number
         * of elements in the map, which is PriceLevels + 1. The additional 1 is reserved for the scenario
         * when the best order is updated and becomes better than all other existing records in the map.
        */
        using TOrdersMap = boost::container::flat_map<TPrice, TQuantity,
                                                      TKeyComparator,
                                                      StackMemoryAllocator<std::pair<TPrice, TQuantity>, PriceLevels + 1>>;

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

            // In case of receiving BBO (Best Bid/Offer) update before Depth Update,
            // add the update as the first entry to ensure the map is not empty.
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

            // Yield the best order first.
            co_yield BestOrder_;

            // Iterate over the orders and yield those that are under the best order.
            for (auto [price, quantity] : Orders_) {
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
            // If the quantity of the update is greater than zero, insert or update the order.
            if (update.Quantity > 0) {
                // Try to insert the update at the hinted position if available,
                // which is the position of the last insertion or removal.
                // This improves performance by avoiding unnecessary lookups from the beginning of the map.
                auto it = hint.has_value()
                          ? Orders_.try_emplace(hint.value(), update.Price, update.Quantity)
                          : Orders_.try_emplace(update.Price, update.Quantity).first;

                // If the inserted order becomes the first order in the map (has best price), update the best order.
                if (it == Orders_.begin()) {
                    BestOrder_ = update;
                }

                // Insertion has an effect only if there is no order with the same price yet.
                // Check if the insertion had an effect by comparing the quantity of the inserted order
                // with the returned one.
                const bool insertionHadEffect = it->second == update.Quantity;

                // It is unlikely that we receive an update with the same quantity and price as we already have,
                // but it is still possible (ABA problem). If such a case occurs, we simply update the quantity
                // with the same value as it has, ensuring that the order remains consistent.
                if (!insertionHadEffect) {
                    it->second = update.Quantity;
                }
                // If the size of the map exceeds the specified number of price levels,
                // remove the last order from the map.
                // This ensures that the map size remains within the defined limit,
                // which helps to keep the `flat_map` small and avoid potential performance slowdowns.
                // After removing the last order, update the iterator to point to the first order in the map.
                else if (Orders_.size() > PriceLevels) {
                    Orders_.erase(std::prev(Orders_.end()));
                    it = Orders_.begin();
                }

                return it;
            }
            // If the quantity of the update is zero or less, remove the order with the given price.
            else {
                auto it = Orders_.lower_bound(update.Price);
                if (it != Orders_.end() && it->first == update.Price) {
                    it = Orders_.erase(it); // get next item after deleted

                    // If the deleted order was the best order and the map is not empty,
                    // update the best order to the new first order.
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