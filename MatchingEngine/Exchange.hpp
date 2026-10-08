//
//  Exchange.hpp
//  MatchingEngine
//
//  
//

#ifndef Exchange_hpp
#define Exchange_hpp

#include <stdio.h>
#include <thread>
#include <queue>
#include "OrderTracker.hpp"
#include "DepthBook.hpp"

class Exchange {

public:

    std::mutex mutexExchange;

    bool exchangeEnabled = false;

    Exchange(bool enableExchange);

    void addDepthBook(OrderSymbol orderSymbol);

    void deleteDepthBook(OrderSymbol orderSymbol);

    void exchangeMatchingEngine();

    void enterOrder(std::vector<std::string> orderCommand);

    void cancelOrder(std::vector<std::string> orderCommand);

    void flushOrderBook();

private:

    std::queue<std::shared_ptr<OrderTracker>> orderQueue;
    
    std::map<OrderSymbol, std::shared_ptr<DepthBook>> depthBookMap;
    
    std::map<OrderId, std::shared_ptr<OrderTracker>> activeOrdersMap;

    
}; //Exchange



#endif /* Exchange_hpp */
