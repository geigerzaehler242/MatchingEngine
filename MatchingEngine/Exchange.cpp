//
//  Exchange.cpp
//  MatchingEngine
//
//  Created by fernando marto on 2021-01-21.
//

#include "Exchange.hpp"



Exchange::Exchange(bool enableExchange) : exchangeEnabled(enableExchange) {};

    void Exchange::addDepthBook(OrderSymbol orderSymbol) {

        std::shared_ptr<DepthBook> pDepthBook = std::make_shared<DepthBook>(orderSymbol);
        pDepthBook->enableDepthBook(orderSymbol);
        
        depthBookMap[orderSymbol] = pDepthBook;
        
    }

    void Exchange::deleteDepthBook(OrderSymbol orderSymbol) {

        depthBookMap[orderSymbol]->closeDepthBook(orderSymbol);
        depthBookMap[orderSymbol].reset();

    }

    void Exchange::exchangeMatchingEngine() {

        while(exchangeEnabled) {
            
            while(orderQueue.size() > 0) {
                
                std::shared_ptr<OrderTracker> pNewOrder = orderQueue.front();
                orderQueue.pop();
                
                OrderSymbol symbol = pNewOrder->getSymbol();
                OrderId orderId = pNewOrder->getOrderId();
                
                depthBookMap[symbol]->placeOrder(pNewOrder); //insert order onto symbols depthbook
                activeOrdersMap[orderId] = pNewOrder;
                
                depthBookMap[symbol]->matchingEngine(symbol); //check for order match on depth book
            }
        }
    }

    void Exchange::enterOrder(std::vector<std::string> orderCommand) {

        std::unique_lock<std::mutex> threadLock(mutexExchange);
        
        OrderId userOrderId = stoi( orderCommand[PlaceOrderKey::userOrderId] );
        Price price = stoi( orderCommand[PlaceOrderKey::price] );
        Quantity quantity = stoi( orderCommand[PlaceOrderKey::qty] );
        OrderType side = orderCommand[PlaceOrderKey::side] == "B" ? buy : sell;
        OrderSymbol symbol = orderCommand[PlaceOrderKey::symbol];
        UserId userId = stoi( orderCommand[PlaceOrderKey::user] );

        std::shared_ptr<OrderTracker> pNewOrder = std::make_shared<OrderTracker>(userOrderId, price, quantity, side, symbol, userId);
        
        if(depthBookMap.find(orderCommand[PlaceOrderKey::symbol]) == depthBookMap.end()) {
            addDepthBook(orderCommand[PlaceOrderKey::symbol]);
        }

        this->orderQueue.push(pNewOrder);

//        threadLock.unlock();
    }

    void Exchange::cancelOrder(std::vector<std::string> orderCommand) {

        //C, 2, 101

        std::unique_lock<std::mutex> threadLock(mutexExchange);
        
        std::shared_ptr<OrderTracker> pActiveOrder = activeOrdersMap[stoi( orderCommand[CancelOrderKey::userOrderIdCancel] )];

        this->depthBookMap[pActiveOrder->getSymbol()]->cancelOrder(pActiveOrder);
        this->activeOrdersMap.erase(pActiveOrder->getOrderId());
        
//        threadLock.unlock();
    }

    void Exchange::flushOrderBook() {

        std::unique_lock<std::mutex> threadLock(mutexExchange);
        
        for(auto & depthBook : depthBookMap) {

            deleteDepthBook(depthBook.first);
        }

        depthBookMap.clear();   //clear all depth books. F command should also include the symbol!!
        
        this->activeOrdersMap.clear();
        
        while (!this->orderQueue.empty())
        {
            this->orderQueue.pop();
        }
        
//        threadLock.unlock();
    }

