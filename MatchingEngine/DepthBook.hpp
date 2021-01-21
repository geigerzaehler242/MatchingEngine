//
//  DepthBook.hpp
//  MatchingEngine
//
//  Created by fernando marto on 2021-01-20.
//

#ifndef DepthBook_hpp
#define DepthBook_hpp

//#include <stdio.h>
#include <thread>
#include <map>
#include "OrderTracker.hpp"

class DepthBook {
    
public:
    
    std::mutex mutexDepthBook;
    
    DepthBook(OrderSymbol orderSymbol);
    
    void placeOrder(std::shared_ptr<OrderTracker> pTradeOrder);
    
    std::tuple<Price,Quantity,Price,Quantity> getTOB();
    
    void flushOrderBook();
    
    void cancelOrder(std::shared_ptr<OrderTracker> pTradeOrder);
    
    void changeOrder(std::shared_ptr<OrderTracker> pTradeOrder, Price newPrice, Quantity newQuantity);

    void matchingEngine(OrderSymbol const& targetTicker);
    
    void printOrderCancel(UserId userId, OrderId orderId);
    
    void printOrderChange(UserId userId, OrderId orderId);
    
    void printOrderFill(UserId userIdBuy, OrderId userOrderIdBuy, UserId userIdSell, OrderId userOrderIdSell, Quantity quantity, Price price);
    
    void printTOBChange(Quantity quantity, Price price, OrderType orderType);
    
    void enableDepthBook(OrderSymbol tickerSymbol);

    void closeDepthBook(OrderSymbol tickerSymbol);
    
private:
    OrderSymbol tickerSymbol;
    std::map<OrderSymbol, bool> depthBookControlMap;
    std::multimap<Price, std::shared_ptr<OrderTracker>, std::greater<Price> >  Bids; //price and time priority maintained
    std::multimap<Price, std::shared_ptr<OrderTracker>, std::less<Price> >     Asks; //price and time priority maintained
}; //DepthBook




#endif /* DepthBook_hpp */
