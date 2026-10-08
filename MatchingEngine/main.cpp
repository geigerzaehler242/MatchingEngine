//
//  main.cpp
//  MatchingEngine
//
//
//

//#include <iostream>
//#include <map>
//#include <queue>
#include <thread>
#include <chrono>
#include <tuple>
#include <limits.h>

//BUILD SETTINGS
// xcode targets -> Build Settings -> search paths -> /opt/homebrew/Cellar/boost/1.92.0/include/
// preprocessor macro -> USE_XCODE=1

#ifdef USE_XCODE
#include "/opt/homebrew/Cellar/boost/1.92.0/include/boost/asio.hpp"
#endif

#include "boost/asio.hpp"
#include "UDPServer.hpp"
#include "OrderTracker.hpp" //#include <iostream>
#include "DepthBook.hpp" //#include <map>
#include "Exchange.hpp" //#include <thread>, #include <queue>

//Format new order:
// N     user(int)    symbol(string)    price(int)    qty(int)    side(char B or S)    userOrderId(int)

// Format cancel order:
// C     user(int)    userOrderId(int)

//Format flush order book:
// F

// Notes:
// * Price is 0 for market order    <>0 for limit order
// * TOB = Top Of Book     highest bid     lowest offer
// * Between scenarios flush order books

// N, 1, IBM, 10, 100, B, 1,
// N, 1, IBM, 12, 100, S, 2,
// N, 1, IBM, 12, 100, S, 3,
// N, 2, IBM, 9, 100, B, 101,
// N, 2, IBM, 11, 100, S, 102,
// N, 1, IBM, 11, 100, B, 4,
// N, 2, IBM, 10, 100, S, 103,
// N, 1, IBM, 10, 100, B, 5,
// N, 2, IBM, 11, 100, S, 104,
// F


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
        else if(command == "Q") {
            //Format exit:
            // Q
            std::cout << "shutdown exchange command received" << std::endl;
            pExchange->exchangeEnabled = false;
        }
        else {
            std::cout << "invalid order command" << std::endl;
        }
}

void runExchange(UDPServer &udpReceiver) {
    
    //    //create depth book, run Depth Book matching engine on a thread
    //    std::shared_ptr<DepthBook> pDepthBook = std::make_shared<DepthBook>("AAPL");
    //    pDepthBook->enableDepthBook("AAPL");
    //    std::thread tMatchingEngine(&DepthBook::matchingEngine, pDepthBook, "AAPL"); //(pointer-to-member, object (or pointer), argument

    //create exchange, run a Depth Book matching engine on a thread for each trading instrument on the exchange
    std::shared_ptr<Exchange> pExchange = std::make_shared<Exchange>(true); //true allows exchange to run
    std::thread tExchangeMatchingEngine(&Exchange::exchangeMatchingEngine, pExchange); //(pointer-to-member, object (or pointer), argument
    
    std::cout << "Running Equity Exchange" << std::endl;
    std::cout << "Send 'Q' command to exit: " << std::endl;
    
    while(pExchange->exchangeEnabled) {
        
//std::this_thread::sleep_for(std::chrono::seconds(1)); //temporary time limiter for debugging!!
        
        auto orderVector = udpReceiver.getOrderVector(); //get any new received orders, protected by mutex
        
        if(!orderVector.empty()) {
            
            std::cout << "received commands/orders" << std::endl;
            
            for(auto & order : orderVector) {
                
                std::vector<std::string> orderCommand = udpReceiver.split(order, ',');
                processOrderCommand(orderCommand, pExchange);
                
//                std::this_thread::sleep_for(std::chrono::nanoseconds(1000000)); //temporary time limiter for debugging!!
            }
            
            orderVector.clear();
            
            std::cout << "Orders/Commands processed" << std::endl;
//            std::cout << "Send 'Q' command to exit: " << std::endl;
        }
    };
    
    pExchange->exchangeEnabled = false;
    
    if (tExchangeMatchingEngine.joinable()) {
        tExchangeMatchingEngine.join(); // Wait for the thread to finish
    }
}

int main(int argc, const char * argv[]) {

//cmake -B build
//cmake --build build
//cd build
//./equityExchangeTest
    
//docker build -t equityexchangetest .
    
//to send the orders data to the matching engine
//cat inputfile.csv | nc -u -v -w 1 127.0.0.1 1234
    
    try {
        
        short port = 1234;
        boost::asio::io_context io_context;
    
        auto work_guard = boost::asio::make_work_guard(io_context); //prevent io_context::run() from returning immediately if there is no work
    
        UDPServer udpReceiver(io_context, port); //create and start UDP receiver for receiving order data
        
        std::thread tUDPReceiver( [&io_context]() {
            io_context.run(); //run io_context in a separate thread. main thread remains unblocked
        }); //thread to receive orders via UDP

        runExchange(udpReceiver); //run exchange until prompted to stop
        
        io_context.stop();
        if (tUDPReceiver.joinable()) {
            tUDPReceiver.join(); // Wait for the thread to finish
        }
        
    } catch (std::exception(& e)) {
        std::cerr << e.what() << std::endl;
    }
    
    std::cout << "Shut down exchange and exit " << std::endl;
    
    return 0;
}
