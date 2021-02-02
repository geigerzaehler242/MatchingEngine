//
//  DepthBook.cpp
//  MatchingEngine
//
//  Created by fernando marto on 2021-01-20.
//

#include "DepthBook.hpp"

    
    DepthBook::DepthBook(OrderSymbol orderSymbol) : tickerSymbol(orderSymbol) {};
    
    
    void DepthBook::placeOrder(std::shared_ptr<OrderTracker> pTradeOrder) {
        
//        std::unique_lock<std::mutex> threadLock(mutexDepthBook);
        
        Price price = pTradeOrder->getOrderPrice();
        
        auto[buyPriceTOB,buyQuantityTOB,sellPriceTOB,sellQuantityTOB] = getTOB();
        
        switch(pTradeOrder->getOrderType()) {
                
            case buy:
                Bids.insert( std::pair<Price, std::shared_ptr<OrderTracker>>(pTradeOrder->getOrderPrice(), pTradeOrder) );
                
                if(price > buyPriceTOB) {
                    printTOBChange(pTradeOrder->getOrderQuantity(), price, buy);
                } else if(price == buyPriceTOB) {
                    printTOBChange(pTradeOrder->getOrderQuantity() + buyQuantityTOB, buyPriceTOB, buy);
                }
                break;
            case sell:
                Asks.insert( std::pair<Price, std::shared_ptr<OrderTracker>>(pTradeOrder->getOrderPrice(), pTradeOrder) );
                if(price < sellPriceTOB) {
                    printTOBChange(pTradeOrder->getOrderQuantity(), price, sell);
                } else if(price == sellPriceTOB) {
                    printTOBChange(pTradeOrder->getOrderQuantity() + sellQuantityTOB, sellPriceTOB, sell);
                }
                break;
            case shortSell:
                Asks.insert( std::pair<Price, std::shared_ptr<OrderTracker>>(pTradeOrder->getOrderPrice(), pTradeOrder) );
                if(price < sellPriceTOB) {
                    printTOBChange(pTradeOrder->getOrderQuantity(), price, shortSell);
                } else if(price == sellPriceTOB) {
                    printTOBChange(pTradeOrder->getOrderQuantity() + sellQuantityTOB, sellPriceTOB, shortSell);
                }
                break;
            default:
                break;
        }
        
//        threadLock.unlock();
    }
    
    std::tuple<Price, Quantity, Price, Quantity> DepthBook::getTOB() {
        
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::greater<Price> >::iterator pBids = Bids.begin();
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::less<Price> >::iterator pAsks = Asks.begin();
        
        Price bestBuyPrice = 0;
        Quantity totalBuyQuantity = 0;
        Price bestSellPrice = INT_MAX;
        Quantity totalSellQuantity = 0;
        
        if(pBids != Bids.end()) {
            bestBuyPrice = pBids->first;
            totalBuyQuantity = pBids->second->getOrderQuantity();
            pBids++;
            for( ; pBids != Bids.end() && pBids->first == bestBuyPrice; pBids++) {
                
                totalBuyQuantity += pBids->second->getOrderQuantity();
            }
        }
        
        if(pAsks != Asks.end()) {
            bestSellPrice = pAsks->first;
            totalSellQuantity = pAsks->second->getOrderQuantity();
            pAsks++;
            for( ; pAsks != Asks.end() && pAsks->first == bestSellPrice; pAsks++) {
                
                totalSellQuantity += pAsks->second->getOrderQuantity();
            }
        }
        
        auto result = std::make_tuple (bestBuyPrice, totalBuyQuantity, bestSellPrice, totalSellQuantity);
        return result;
    }
    
    
    void DepthBook::flushOrderBook() {
        
//        std::unique_lock<std::mutex> threadLock(mutexDepthBook);
        
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::greater<Price> >::iterator pBids = Bids.begin();
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::less<Price> >::iterator pAsks = Asks.begin();
        
        for( ; pBids != Bids.end() ; pBids++) {
            Bids.erase(pBids);
        }
        
        for( ; pAsks != Asks.end() ; pAsks++) {
            Asks.erase(pAsks);
        }
        
//        threadLock.unlock();
    }
    
    
    void DepthBook::cancelOrder(std::shared_ptr<OrderTracker> pTradeOrder) {
        
//        std::unique_lock<std::mutex> threadLock(mutexDepthBook);
        
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::greater<Price> >::iterator pBids = Bids.begin();
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::less<Price> >::iterator pAsks = Asks.begin();
        
        Price price = pTradeOrder->getOrderPrice();
        
        auto[buyPriceTOB,buyQuantityTOB,sellPriceTOB,sellQuantityTOB] = getTOB();
        
        switch(pTradeOrder->getOrderType()) {
                
            case buy:
                
                for( ; pBids != Bids.end() ; pBids++) {
                    
                    if(pBids->second->getOrderId() == pTradeOrder->getOrderId() &&
                       pBids->second->getUserId() == pTradeOrder->getUserId() ) {
                        Bids.erase(pBids);
                        printOrderCancel(pBids->second->getUserId(), pBids->second->getOrderId());
                        break;
                    }
                }
                
                if(price == buyPriceTOB) {
                    printTOBChange(buyQuantityTOB - pTradeOrder->getOrderQuantity(), buyPriceTOB, buy);
                }
                
                break;
                
            case sell:
                
                for( ; pAsks != Asks.end() ; pAsks++) {
                    
                    if(pAsks->second->getOrderId() == pTradeOrder->getOrderId() &&
                       pAsks->second->getUserId() == pTradeOrder->getUserId() ) {
                        Asks.erase(pAsks);
                        printOrderCancel(pAsks->second->getUserId(), pAsks->second->getOrderId());
                        break;
                    }
                }
                
                if(price == sellPriceTOB) {
                    printTOBChange(sellQuantityTOB - pTradeOrder->getOrderQuantity(), sellPriceTOB, sell);
                }
                break;
                
            case shortSell:
                
                for( ; pAsks != Asks.end() ; pAsks++) {
                    
                    if(pAsks->second->getOrderId() == pTradeOrder->getOrderId() &&
                       pAsks->second->getUserId() == pTradeOrder->getUserId() ) {
                        Asks.erase(pAsks);
                        printOrderCancel(pAsks->second->getUserId(), pAsks->second->getOrderId());
                        break;
                    }
                }
                
                if(price == sellPriceTOB) {
                    printTOBChange(sellQuantityTOB - pTradeOrder->getOrderQuantity(), sellPriceTOB, shortSell);
                }
                break;
                
            default:
                break;
        } //switch
        
//        threadLock.unlock();
    }
    
    void DepthBook::changeOrder(std::shared_ptr<OrderTracker> pTradeOrder, Price newPrice, Quantity newQuantity) {
        
//        std::unique_lock<std::mutex> threadLock(mutexDepthBook);
        
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::greater<Price> >::iterator pBids = Bids.begin();
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::less<Price> >::iterator pAsks = Asks.begin();
        
        Price price = pTradeOrder->getOrderPrice();
        
        auto[buyPriceTOB,buyQuantityTOB,sellPriceTOB,sellQuantityTOB] = getTOB();
        
        switch(pTradeOrder->getOrderType()) {
                
            case buy:
                
                for( ; pBids != Bids.end() ; pBids++) {
                    
                    if(pBids->second->getOrderId() == pTradeOrder->getOrderId()) {
                        
                        if(price > buyPriceTOB) {
                            printTOBChange(pTradeOrder->getOrderQuantity(), newPrice, buy);
                        } else if(price == buyPriceTOB) {
                            if(newQuantity < pBids->second->getOrderQuantity()) {
                                printTOBChange(buyQuantityTOB - (pBids->second->getOrderQuantity() - newQuantity), buyPriceTOB, buy);
                            }
                            else {
                                printTOBChange(buyQuantityTOB + (pBids->second->getOrderQuantity() - newQuantity), buyPriceTOB, buy);
                            }
                        }
                        
                        pBids->second->setOrderPrice(newPrice);
                        pBids->second->setOrderQuantity(newQuantity);
                        printOrderChange(pBids->second->getUserId(), pBids->second->getOrderId());
                        break;
                    }
                }
                break;
                
            case sell:
                
                for( ; pAsks != Asks.end() ; pAsks++) {
                    
                    if(pAsks->second->getOrderId() == pTradeOrder->getOrderId()) {
                        
                        if(price < sellPriceTOB) {
                            printTOBChange(pTradeOrder->getOrderQuantity(), newPrice, sell);
                        } else if(price == sellPriceTOB) {
                            if(newQuantity < pBids->second->getOrderQuantity()) {
                                printTOBChange(sellQuantityTOB - (pAsks->second->getOrderQuantity() - newQuantity), sellPriceTOB, sell);
                            }
                            else {
                                printTOBChange(sellQuantityTOB + (pAsks->second->getOrderQuantity() - newQuantity), sellPriceTOB, sell);
                            }
                        }
                        
                        pAsks->second->setOrderPrice(newPrice);
                        pAsks->second->setOrderQuantity(newQuantity);
                        printOrderChange(pAsks->second->getUserId(), pAsks->second->getOrderId());
                        break;
                    }
                }
                break;
                
            case shortSell:
                
                for( ; pAsks != Asks.end() ; pAsks++) {
                    
                    if(pAsks->second->getOrderId() == pTradeOrder->getOrderId()) {
                        
                        if(price < sellPriceTOB) {
                            printTOBChange(pTradeOrder->getOrderQuantity(), newPrice, shortSell);
                        } else if(price == sellPriceTOB) {
                            if(newQuantity < pBids->second->getOrderQuantity()) {
                                printTOBChange(sellQuantityTOB - (pAsks->second->getOrderQuantity() - newQuantity), sellPriceTOB, shortSell);
                            }
                            else {
                                printTOBChange(sellQuantityTOB + (pAsks->second->getOrderQuantity() - newQuantity), sellPriceTOB, shortSell);
                            }
                        }
                        
                        pAsks->second->setOrderPrice(newPrice);
                        pAsks->second->setOrderQuantity(newQuantity);
                        printOrderChange(pAsks->second->getUserId(), pAsks->second->getOrderId());
                        break;
                    }
                }
                break;
                
            default:
                break;
        } //switch
        
        
//        threadLock.unlock();
    }


    
    void DepthBook::matchingEngine(OrderSymbol const& targetTicker) {
        
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::greater<Price> >::iterator pBids;
        std::multimap<Price, std::shared_ptr<OrderTracker>, std::less<Price> >::iterator pAsks;
       
 //       std::cout << "starting depth book: " << targetTicker << std::endl;
        
//        while( depthBookControlMap[targetTicker] && (Bids.size() > 0 || Asks.size() > 0) ) {

//////////////////////////
//std::this_thread::sleep_for(std::chrono::nanoseconds(1000000)); //temporary time limiter for debugging!!
//////////////////////////
//            std::unique_lock<std::mutex> threadLock(mutexDepthBook);
            
//            std::cout << "looking for match..." << std::endl;
            pBids = Bids.begin();
            pAsks = Asks.begin();

            if(pBids->first == pAsks->first && Bids.size() != 0 && Asks.size() != 0) {
                
//                std::cout << "found match" << std::endl;
                
                if(pBids->second->getOrderQuantity() < pAsks->second->getOrderQuantity()) { //BID size < ASK size
                    Quantity currentBidQuantity = pBids->second->getOrderQuantity();
                    pBids->second->setOrderQuantity(0);
                    pAsks->second->setOrderQuantity(pAsks->second->getOrderQuantity() - currentBidQuantity);
                    
                    pAsks->second->setOrderFills(currentBidQuantity);
                    pBids->second->setOrderFills(currentBidQuantity);
                    
                    Price fillPrice = pBids->second->getOrderPrice();
                    pBids->second->setOrderFilled(true);
                    Bids.erase(pBids);
                    printOrderFill(pBids->second->getUserId(), pBids->second->getOrderId(), pAsks->second->getUserId(), pAsks->second->getOrderId(), currentBidQuantity, fillPrice);
                }
                else if(pBids->second->getOrderQuantity() > pAsks->second->getOrderQuantity() ) { //BID size > ASK size
                    Quantity currentAskQuantity = pAsks->second->getOrderQuantity();
                    pAsks->second->setOrderQuantity(0.0);
                    pBids->second->setOrderQuantity(pBids->second->getOrderQuantity() - currentAskQuantity);
                    
                    pAsks->second->setOrderFills(currentAskQuantity);
                    pBids->second->setOrderFills(currentAskQuantity);
                    
                    Price fillPrice = pBids->second->getOrderPrice();
                    pAsks->second->setOrderFilled(true);
                    Asks.erase(pAsks);
                    printOrderFill(pBids->second->getUserId(), pBids->second->getOrderId(), pAsks->second->getUserId(), pAsks->second->getOrderId(), currentAskQuantity, fillPrice);
                }
                else { //BID size = ASK size
                    Quantity currentBidAskQuantity = pAsks->second->getOrderQuantity();
                    pBids->second->setOrderFills(currentBidAskQuantity);
                    pAsks->second->setOrderFills(currentBidAskQuantity);
                    
                    Price fillPrice = pBids->second->getOrderPrice();
                    pBids->second->setOrderFilled(true);
                    pAsks->second->setOrderFilled(true);
                    Bids.erase(pBids);
                    Asks.erase(pAsks);
                    printOrderFill(pBids->second->getUserId(), pBids->second->getOrderId(), pAsks->second->getUserId(), pAsks->second->getOrderId(), currentBidAskQuantity, fillPrice);
                }
            
            } //if
//            threadLock.unlock();
//        } //while
    }
    
    void DepthBook::printOrderCancel(UserId userId, OrderId orderId) {
        
        std::thread tPrintService([&] {
            std::cout << "A, " << userId << ", " << orderId << std::endl;
        });
        
        tPrintService.join();
    }
    
    void DepthBook::printOrderChange(UserId userId, OrderId orderId) {
    
        std::thread tPrintService([&] {
            std::cout << "A, " << userId << ", " << orderId << std::endl;
        });
        
        tPrintService.join();
    }
    
    void DepthBook::printOrderFill(UserId userIdBuy, OrderId userOrderIdBuy, UserId userIdSell, OrderId userOrderIdSell, Quantity quantity, Price price) {
        
        
        std::thread tPrintService([&] {
            std::cout << "T, " << userIdBuy << ", " << userOrderIdBuy << ", " << userIdSell << ", " << userOrderIdSell << ", " << price << ", " << quantity << std::endl;
        });
        
        tPrintService.join();
        
    }
    
    void DepthBook::printTOBChange(Quantity quantity, Price price, OrderType orderType) {
        
        std::thread tPrintService([&] {
            std::string side = (orderType == buy) ? "B" : "S";
            std::cout << "B, " << side << ", " << price << ", " << quantity << std::endl;
        });
        
        tPrintService.join();
    }
    
//    void DepthBook::enableDepthBook(OrderSymbol tickerSymbol) {
//        depthBookControlMap[tickerSymbol] = true;
//    }
//
//    void DepthBook::closeDepthBook(OrderSymbol tickerSymbol) {
//        depthBookControlMap[tickerSymbol] = false;
//    }
    
