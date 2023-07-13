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

    /*
     * An order book for Binance or a similar protocol.
     * Receives updates in the form of bids and asks for top PriceLevels as well as best pure best bid/ask updates.
    */
    template <typename TPrice = double , typename TQuantity = double, size_t PriceLevels = 20>
    class BinanceBook {
    private:
        using TAsks = OrderMap<TPrice, TQuantity, std::less<>, PriceLevels>;
        using TBids = OrderMap<TPrice, TQuantity, std::greater<>, PriceLevels>;
        using TPriceQuantity = Models::PriceQuantity<TPrice, TQuantity>;
        using TBookTicker = Models::BookTicker<TPrice, TQuantity>;

        TAsks Asks_; // Asks container
        TBids Bids_; // Bids container

    public:
        // Clear the order book by removing all bids and asks.
        void Clear() noexcept {
            Bids_.Clear();
            Asks_.Clear();
        }

        // Check if the order book is empty.
        [[nodiscard]]
        bool IsEmpty() const noexcept {
            return Bids_.IsEmpty() && Asks_.IsEmpty();
        }

        // Replace the entire contents of the order book with new bids and asks.
        void Replace(InputRange<TPriceQuantity> auto&& bids, InputRange<TPriceQuantity> auto&& asks) noexcept {
            Clear();
            DepthUpdate(bids, asks);
        }

        // Update the order book with new bids and asks.
        void DepthUpdate(InputRange<TPriceQuantity> auto&& bids, InputRange<TPriceQuantity> auto&& asks) noexcept {
            Bids_.UpdateOrders(bids);
            Asks_.UpdateOrders(asks);
        }

        // Update the best bid and best ask in the order book based on the book ticker data.
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

        // Retrieve the bids and asks from the order book as generators,
        // enabling lazy evaluation and avoiding unnecessary memory copies.
        [[nodiscard]]
        auto Extract() const -> std::pair<Utils::Generator<TPriceQuantity>, Utils::Generator<TPriceQuantity>> {
            return std::make_pair(Bids_.Extract(), Asks_.Extract());
        }

        // Convert the order book to a string representation.
        [[nodiscard]]
        std::string ToString() const {
            std::stringstream ss;
            ss << *this;

            return ss.str();
        }
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
