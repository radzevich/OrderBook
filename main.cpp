#include <iostream>

#include "src/order_book.h"

int main() {
    using TPrice = double;
    using TQuantity = double;
    using TPriceQuantity = OrderBook::Models::PriceQuantity<TPrice, TQuantity>;

    OrderBook::BinanceBook<TPrice, TQuantity, 20> book;

    book.DepthUpdate([]() -> OrderBook::Utils::Generator<TPriceQuantity> {
        co_yield { .Price = 20078.54000000, .Quantity = 0.00431000 };
        co_yield { .Price = 20078.39000000, .Quantity = 0.00100000 };
        co_yield { .Price = 20078.27000000, .Quantity = 0.00070000 };
        co_yield { .Price = 20078.21000000, .Quantity = 0.00066000 };
        co_yield { .Price = 20077.91000000, .Quantity = 0.03781000 };
        co_yield { .Price = 20077.90000000, .Quantity = 0.00110000 };
        co_yield { .Price = 20077.86000000, .Quantity = 0.00070000 };
        co_yield { .Price = 20077.80000000, .Quantity = 0.00650000 };
        co_yield { .Price = 20077.73000000, .Quantity = 0.00055000 };
        co_yield { .Price = 20077.71000000, .Quantity = 0.00100000 };
        co_yield { .Price = 20077.69000000, .Quantity = 0.00984000 };
        co_yield { .Price = 20077.66000000, .Quantity = 0.00066000 };
        co_yield { .Price = 20077.61000000, .Quantity = 0.04000000 };
        co_yield { .Price = 20077.60000000, .Quantity = 0.02484000 };
        co_yield { .Price = 20077.56000000, .Quantity = 0.05481000 };
        co_yield { .Price = 20077.52000000, .Quantity = 0.28882000 };
        co_yield { .Price = 20077.51000000, .Quantity = 0.00064000 };
        co_yield { .Price = 20077.45000000, .Quantity = 0.00070000 };
        co_yield { .Price = 20077.43000000, .Quantity = 0.05181000 };
        co_yield { .Price = 20077.39000000, .Quantity = 0.00689000 };
    }(), []() -> OrderBook::Utils::Generator<TPriceQuantity> {
        co_yield { .Price = 20078.91000000, .Quantity = 0.03437000 };
        co_yield { .Price = 20078.95000000, .Quantity = 0.00100000 };
        co_yield { .Price = 20078.99000000, .Quantity = 0.00498000 };
        co_yield { .Price = 20079.01000000, .Quantity = 0.04981000 };
        co_yield { .Price = 20079.09000000, .Quantity = 0.00070000 };
        co_yield { .Price = 20079.15000000, .Quantity = 0.24902000 };
        co_yield { .Price = 20079.30000000, .Quantity = 0.04110000 };
        co_yield { .Price = 20079.31000000, .Quantity = 0.00066000 };
        co_yield { .Price = 20079.35000000, .Quantity = 0.00864000 };
        co_yield { .Price = 20079.42000000, .Quantity = 0.00100000 };
        co_yield { .Price = 20079.44000000, .Quantity = 0.09402000 };
        co_yield { .Price = 20079.46000000, .Quantity = 0.09402000 };
        co_yield { .Price = 20079.49000000, .Quantity = 0.00100000 };
        co_yield { .Price = 20079.50000000, .Quantity = 0.00070000 };
        co_yield { .Price = 20079.51000000, .Quantity = 0.17430000 };
        co_yield { .Price = 20079.53000000, .Quantity = 0.09602000 };
        co_yield { .Price = 20079.60000000, .Quantity = 0.00100000 };
        co_yield { .Price = 20079.61000000, .Quantity = 0.22853000 };
        co_yield { .Price = 20079.62000000, .Quantity = 0.08741000 };
        co_yield { .Price = 20079.66000000, .Quantity = 0.00400000 };
    }());

    std::cout << book << std::endl;

    return 0;
}
