#pragma once

#include <iterator>
#include <vector>
#include <ranges>
#include <sstream>

#include <boost/format.hpp>

#include "utils/generator.h"
#include "order_map.h"
#include "models/book_ticker.h"
#include "models/price_quantity.h"

namespace OrderBook {

    template <typename TPrice = double , typename TQuantity = double, size_t Capacity = 20>
    class BinanceBook {
    private:
        using TPriceType = TPrice;
        using TQuantityType = TQuantity;
        using TAsks = OrderMap<TPrice, TQuantity, std::less<>, Capacity>;
        using TBids = OrderMap<TPrice, TQuantity, std::greater<>, Capacity>;
        using TPriceQuantity = Models::PriceQuantity<TPrice, TQuantity>;
        using TBookTicker = Models::BookTicker<TPrice, TQuantity>;

        TAsks Asks_;
        TBids Bids_;

    public:
        void Clear() noexcept {
            Bids_.Clear();
            Asks_.Clear();
        }

        [[nodiscard]]
        bool IsEmpty() const noexcept {
            return Bids_.IsEmpty() && Asks_.IsEmpty();
        }

        void Replace(InputRange<TPriceQuantity> auto&& bids, InputRange<TPriceQuantity> auto&& asks) noexcept {
            Clear();
            DepthUpdate(bids, asks);
        }

        void DepthUpdate(InputRange<TPriceQuantity> auto&& bids, InputRange<TPriceQuantity> auto&& asks) noexcept {
            Bids_.UpdateOrders(bids);
            Asks_.UpdateOrders(asks);
        }

        void BBOUpdate(TBookTicker ticker) {
            Bids_.UpdateBestOrder({
                .Price = ticker.BestBidPrice,
                .Quantity = ticker.BestBidQty,
            });

            Asks_.UpdateBestOrder({
                .Price = ticker.BestAskPrice,
                .Quantity = ticker.BestAskQty,
            });
        }

        [[nodiscard]]
        auto Extract() const -> std::pair<Utils::Generator<TPriceQuantity>, Utils::Generator<TPriceQuantity>> {
            return std::make_pair(Bids_.Extract(), Asks_.Extract());
        }

        [[nodiscard]]
        std::string ToString() const;
    };

    std::ostream& operator<<(std::ostream& out, const BinanceBook<>& book) {
        if (book.IsEmpty()) {
            out << "[]";
        }

        auto [bids, asks] = book.Extract();

        int index = 1;
        auto bidIt = bids.begin();
        auto askIt = asks.begin();

        {
            boost::format formatter("[%2d] [%9s] %.3f | %.3f [%-9s]\n");

            // std::ranges::zip_view should be here
            for (; bidIt != bids.end() && askIt != asks.end(); ++bidIt, ++askIt, ++index) {
                out << formatter
                       % index
                       % (*bidIt).Quantity
                       % (*bidIt).Price
                       % (*askIt).Price
                       % (*askIt).Quantity;
            }
        }

        // in case, when we have more bids, than asks
        {
            boost::format formatter("[%2d] [%9s] %.3f | %9s [%9s]\n");

            for (; bidIt != bids.end(); ++bidIt, ++index) {
                out << formatter
                       % index
                       % (*bidIt).Quantity
                       % (*bidIt).Price
                       % ""
                       % "";
            }
        }

        // in case, when we have more asks, than bids
        {
            boost::format formatter("[%2d] [%9s] %9s | %.3f [%-9s]\n");

            for (; askIt != asks.end(); ++askIt, ++index) {
                out << formatter
                       % index
                       % ""
                       % ""
                       % (*askIt).Price
                       % (*askIt).Quantity;
            }
        }

        return out;
    }

} // OrderBook
