#include "order_book.h"

namespace OrderBook {

    template <typename TPrice , typename TQuantity, size_t Capacity>
    std::string BinanceBook<TPrice, TQuantity, Capacity>::ToString() const {
        std::stringstream ss;
        ss << *this;

        return ss.str();
    }

}