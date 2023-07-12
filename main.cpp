#include <ostream>

#include "src/order_book.h"
#include "src/models/book_ticker.h"

static constexpr std::string_view Delimiter = "\n**************************************************************\n";

struct StepPrinter {
    std::string_view StepName;
    std::string PreviousTop;
    std::string ExpectedTop;

    StepPrinter(std::string_view stepName, std::string previousTop, std::string expectedTop)
        : StepName(stepName)
        , PreviousTop(std::move(previousTop))
        , ExpectedTop(std::move(expectedTop)) {
    }

};

std::ostream& operator<<(std::ostream& out, StepPrinter step) {
    std::cout << Delimiter;
    std::cout << step.StepName << std::endl;
    std::cout << "Previous top: " << step.PreviousTop << std::endl;
    std::cout << "Expected top: " << step.ExpectedTop << std::endl;
    std::cout << std::endl;

    return out;
}

template <typename ...TArgs>
std::string ToString(TArgs&& ...args) {
    boost::format formatter("[%9s] %.3f | %.3f [%-9s]");
    return (std::stringstream() << ((formatter % args), ...)).str();
}

int main() {
    using TPrice = double;
    using TQuantity = double;
    using TPriceQuantity = OrderBook::Models::PriceQuantity<TPrice, TQuantity>;

    OrderBook::BinanceBook<TPrice, TQuantity, 20> book;

    boost::format formatter("[%9s] %.3f | %.3f [%-9s]\n");

    {
        std::cout << StepPrinter("Depth update empty book",
                                 "N/A",
                                 ToString(0.00431, 20078.540, 20078.91, 0.03437));

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

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    {
        std::cout << StepPrinter("Best Bid/Ask update - New price is better than previous",
                                 ToString(0.00431, 20078.540, 20078.910, 0.03437),
                                 ToString(0.00125, 20079.110, 20077.310, 0.02425));

        book.BBOUpdate({
            .BestBidPrice = 20079.110,
            .BestBidQty = 0.00125,
            .BestAskPrice = 20077.310,
            .BestAskQty = 0.02425
        });

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    {
        std::cout << StepPrinter("Best Bid/Ask update - New price is in the middle of previous top 20 levels",
                                 ToString(0.00125, 20079.110, 20077.310, 0.02425),
                                 ToString(0.00254, 20077.830, 20079.410, 0.05454));

        book.BBOUpdate({
           .BestBidPrice = 20077.830,
           .BestBidQty = 0.00254,
           .BestAskPrice = 20079.410,
           .BestAskQty = 0.05454
       });

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    {
        std::cout << StepPrinter("Best Bid/Ask update - Only quantity changed, but price is same",
                                 ToString(0.00254, 20077.830, 20079.410, 0.05454),
                                 ToString(0.00105, 20077.830, 20079.410, 0.08482));

        book.BBOUpdate({
           .BestBidPrice = 20077.830,
           .BestBidQty = 0.00105,
           .BestAskPrice = 20079.410,
           .BestAskQty = 0.08482
       });

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    {
        std::cout << StepPrinter("Best Bid/Ask update - New price is below previous top 20 levels",
                                 ToString(0.00105, 20077.830, 20079.410, 0.08482),
                                 ToString(0.00254, 20076.140, 20080.230, 0.05454));

        book.BBOUpdate({
           .BestBidPrice = 20076.140,
           .BestBidQty = 0.00254,
           .BestAskPrice = 20080.230,
           .BestAskQty = 0.05454
       });

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    {
        std::cout << Delimiter;
        std::cout << "Is book empty: ";
        std::cout << (book.IsEmpty() ? "True" : "False") << std::endl << std::endl;

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    {
        std::cout << Delimiter;
        std::cout << "Clear book" << std::endl;
        book.Clear();

        std::cout << "Is book empty: ";
        std::cout << (book.IsEmpty() ? "True" : "False") << std::endl << std::endl;

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    {
        std::cout << StepPrinter("Best Bid/Ask update - BBO update in the empty book",
                                 "N/a",
                                 ToString(0.00254, 20077.830, 20079.410, 0.05454));

        book.BBOUpdate({
           .BestBidPrice = 20077.830,
           .BestBidQty = 0.00254,
           .BestAskPrice = 20079.410,
           .BestAskQty = 0.05454
       });

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    {
        std::cout << StepPrinter("Depth update of not empty book",
                                 ToString(0.00254, 20077.830, 20079.410, 0.05454),
                                 ToString(0.00431, 20078.540, 20078.91, 0.03437));


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

        std::cout << "Book: " << std::endl;
        std::cout << book << std::endl;
    }

    return 0;
}
