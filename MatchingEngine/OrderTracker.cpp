//
//  OrderTracker.cpp
//  MatchingEngine
//
//  Created by fernando marto on 2021-01-20.
//

#include "OrderTracker.hpp"

    OrderTracker::OrderTracker(OrderId orderId, Price orderPrice, Quantity orderQuantity, OrderType orderType, OrderSymbol orderSymbol, UserId userId) :
    orderId(orderId),
    orderPrice(orderPrice * DecimalFactor),
    orderQuantity(orderQuantity),
    orderType(orderType),
    orderSymbol(orderSymbol),
    userId(userId) {};
    
//OrderTracker::OrderTracker(const OrderTracker &copyConstructor) {
//        orderId = copyConstructor.orderId;
//        orderPrice = copyConstructor.orderPrice;
//        orderQuantity = copyConstructor.orderQuantity;
//        orderType = copyConstructor.orderType;
//        orderSymbol = copyConstructor.orderSymbol;
//        userId = copyConstructor.userId;
//        
//    }
    
    void OrderTracker::setOrderFilled(bool newState) {
        orderFilled = newState;
    }
    
    void OrderTracker::setOrderFills(Quantity newQuantity) {
        orderFills = newQuantity;
    }
    
    void OrderTracker::setOrderQuantity(Quantity newQuantity) {
        orderQuantity = newQuantity;
    }
    
    Quantity OrderTracker::getOrderQuantity() {
        return orderQuantity;
    }
    
    Price OrderTracker::getOrderPrice() {
        return orderPrice;
    }
    
    void OrderTracker::setOrderPrice(Price newPrice) {
        orderPrice = newPrice;
    }
    
    OrderType OrderTracker::getOrderType() {
        return orderType;
    }
    
    OrderId OrderTracker::getOrderId() {
        return  orderId;
    }
    
    UserId OrderTracker::getUserId() {
        return  userId;
    }
    
    OrderSymbol OrderTracker::getSymbol() {
        return  orderSymbol;
    }

