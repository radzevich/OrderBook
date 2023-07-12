# OrderBook

## Requirements
### Functional requirements 
1. Update BBO
2. Replace bids/asks (generic container as a source)
3. Clear the book
4. Check if book is empty
5. Get the book state
6. Make string representation of the book state

### NF Requirements
#### Partial update:
- Update period - 100 ms
- Each update gives **up to top 20 levels** in canonical order (bids - desc, asks - asc)
  https://github.com/binance/binance-spot-api-docs/blob/master/web-socket-streams.md#partial-book-depth-streams

#### Best bid/offer (BBO) update
- Update period - 100 ms
- Each update gives only best bid and best offer
  https://github.com/binance/binance-spot-api-docs/blob/master/web-socket-streams.md#individual-symbol-book-ticker-streams

#### Valid book state invariants:
- Price levels are unique
- Price levels are in correct order in any moment of time

### Input data guarantees and tips
- Prices are unique
- Prices are absolute
- Prices with zero values for levels which are not in book can appear and should be ignored
- Prices with zero values for levels which are in book should be considered as removing of price level
- If new best bid is less, than current - the current bid level should be removed, otherwise - moved down in the book
- If new best ask is greater, than current - the current ask level should be removed, otherwise - moved down in the book
- Thread safety is not required

