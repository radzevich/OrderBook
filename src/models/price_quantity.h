#pragma once

namespace OrderBook::Models {

    template <typename TPrice, typename TQuantity>
    struct PriceQuantity
    {
        TPrice Price{};
        TQuantity Quantity{};
    };

}
