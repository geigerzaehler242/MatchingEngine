//
//  OrderTracker.hpp
//  MatchingEngine
//
//  
//

#ifndef OrderTracker_hpp
#define OrderTracker_hpp

#include <iostream>
#include <cstdint>
//#include <stdio.h>

typedef std::string OrderSymbol;
typedef std::uint32_t UserId;
typedef std::uint32_t OrderId;
typedef std::uint32_t Price;
typedef std::uint32_t Quantity;
typedef std::uint32_t Cost;
typedef std::uint32_t FillId;
typedef std::uint32_t ChangeId;
typedef std::uint32_t TransId;
typedef std::uint32_t OrderConditions;

static const int DecimalFactor = 1; //use 100 when having to account for decimal prices!!

enum OrderType {
    buy,
    sell,
    shortSell
};

enum PlaceOrderKey {
    
    commandType = 0,
    user,
    symbol,
    price,
    qty,
    side,
    userOrderId
    
};

enum CancelOrderKey {
    
    commandTypeCancel = 0,
    userCancel,
    userOrderIdCancel
    
};


class OrderTracker {
    
public:
    
    OrderTracker(OrderId orderId, Price orderPrice, Quantity orderQuantity, OrderType orderType, OrderSymbol orderSymbol, UserId userId);
    
    OrderTracker(const OrderTracker &copyConstructor) {
        orderId = copyConstructor.orderId;
        orderPrice = copyConstructor.orderPrice;
        orderQuantity = copyConstructor.orderQuantity;
        orderType = copyConstructor.orderType;
        orderSymbol = copyConstructor.orderSymbol;
        userId = copyConstructor.userId;
    }
    
    void setOrderFilled(bool newState);
    
    void setOrderFills(Quantity newQuantity);
    
    void setOrderQuantity(Quantity newQuantity);
   
    Quantity getOrderQuantity();
        
    Price getOrderPrice();
        
    void setOrderPrice(Price newPrice);
       
    OrderType getOrderType();
        
    OrderId getOrderId();
        
    UserId getUserId();
        
    OrderSymbol getSymbol();
        
    private:
        OrderSymbol orderSymbol = "";
        UserId userId = 0;
        OrderId orderId = 0;
        Price orderPrice = 0;
        Quantity orderQuantity = 0;
        Quantity orderFills = 0;
        Cost orderCost = 0.0;
        OrderType orderType = buy;
        bool orderFilled = false;
    }; //OrderTracker



#endif /* OrderTracker_hpp */
