//
//  main.cpp
//  MatchingEngine
//
//  Created by fernando marto on 2020-12-06.
//

//#include <iostream>
//#include <map>
//#include <queue>
#include <thread>
#include <chrono>
#include <tuple>
#include <limits.h>

//ADD YOUR BOOST PATH IN BUILD SETTINGS!!

// xcode targets -> Build Settings -> search paths -> /usr/local/Cellar/boost/1.74.0/include/
//using namespace boost::asio;
//#include "/usr/local/Cellar/boost/1.74.0/include/boost/asio.hpp"
//#include "boost/asio.hpp"
//#include "boost/bind.hpp"
#include "UDPServer.hpp"
#include "OrderTracker.hpp" //#include <iostream>
#include "DepthBook.hpp" //#include <map>
#include "Exchange.hpp" //#include <thread>, #include <queue>

//#Format new order:
//# N     user(int)    symbol(string)    price(int)    qty(int)    side(char B or S)    userOrderId(int)
//#
//#Format cancel order:
//# C     user(int)    userOrderId(int)
//#
//#Format flush order book:
//# F
//
//# Notes:
//# * Price is 0 for market order    <>0 for limit order
//# * TOB = Top Of Book     highest bid     lowest offer
//# * Between scenarios flush order books


//    std::array<char, 9> b = {
//                              N, 1, IBM, 10, 100, B, 1,
//                              N, 1, IBM, 12, 100, S, 2,
//                              N, 1, IBM, 12, 100, S, 2,
//                              N, 2, IBM, 9, 100, B, 101,
//                              N, 2, IBM, 11, 100, S, 102,
//                              N, 1, IBM, 11, 100, B, 3,
//                              N, 2, IBM, 10, 100, S, 103,
//                              N, 1, IBM, 10, 100, B, 4,
//                              N, 2, IBM, 11, 100, S, 104,
//                              F
//                             };

void processOrderCommand(std::vector<std::string> orderCommand, std::shared_ptr<Exchange> pExchange) {

    if(orderCommand.size() == 0) return;

    auto command = orderCommand[PlaceOrderKey::commandType];

        if(command == "N") {
            //Format new order:
            // N     user(int)    symbol(string)    price(int)    qty(int)    side(char B or S)    userOrderId(int)
            // Notes: Price is 0 for market order    <>0 for limit order

            pExchange->enterOrder(orderCommand);
        }
        else if(command == "C") {
            //Format cancel order:
            // C     user(int)    userOrderId(int)

            pExchange->cancelOrder(orderCommand);
        }
        else if(command == "F") {
            //Format flush order book:
            // F

            pExchange->flushOrderBook();
        }
        else {
            std::cout << "invalid order command" << std::endl;
        }

}

int main(int argc, const char * argv[]) {

//"cat inputfile.csv | nc -u -w 60 127.0.0.1 1234” to send the input data
    
    //create UDP receiver for order data
    boost::asio::io_service io_service;
    boost::asio::ip::udp::endpoint remote_endpoint =
    boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234);

    UDPServer server(io_service, remote_endpoint);
    std::thread tBoostASIO{[&io_service](){ io_service.run(); } }; //UDP thread to receive orders
    
//    //create depth book, run Depth Book matching engine on a thread
//    std::shared_ptr<DepthBook> pDepthBook = std::make_shared<DepthBook>("AAPL");
//    pDepthBook->enableDepthBook("AAPL");
//    std::thread tMatchingEngine(&DepthBook::matchingEngine, pDepthBook, "AAPL"); //(pointer-to-member, object (or pointer), argument

    
    
    //create exchange, run each Depth Book matching engine on a thread
    std::shared_ptr<Exchange> pExchange = std::make_shared<Exchange>(true); //true allows exchange to run

    std::thread tExchangeMatchingEngine(&Exchange::exchangeMatchingEngine, pExchange); //(pointer-to-member, object (or pointer), argument

    char status = ' ';
    
    std::cout << "Running Exchange" << std::endl;
    std::cout << "Enter 'q' to exit: " << std::endl;
    
    while(  status != 'q' ) {
        
        std::cin >> status;
        
        auto orderVector = server.getOrderVector();
        
        if(orderVector.size() > 0) {
            std::cout << "received orders" << std::endl;
            
            for(auto & order : orderVector) {
                
                std::vector<std::string> orderCommand = server.split(order, ',');
                processOrderCommand(orderCommand, pExchange);
                
                std::this_thread::sleep_for(std::chrono::nanoseconds(1000000)); //temporary time limiter for debugging!!
            }
            
            server.clearOrderVector();
        }
        
    };
    
    
    return 0;
}
