#pragma once

#include "price_quantity.h"

namespace OrderBook::Models {

    template <typename TPrice, typename TQuantity>
    struct BookTicker {
        TPrice BestBidPrice{};
        TQuantity BestBidQty{};
        TPrice BestAskPrice{};
        TQuantity BestAskQty{};
    };

}
