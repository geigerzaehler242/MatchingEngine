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
//#include <string>

#include <boost/asio.hpp>
#include <librdkafka/rdkafkacpp.h>
#include <kafka/KafkaProducer.h>
#include <kafka/KafkaConsumer.h>

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

std::string getBrokers() {
    const char* env = std::getenv("KAFKA_BROKERS"); // set to "kafka:9092" when run in docker container "docker compose up --build"
    return env ? env : "localhost:9092";   // fallback to localhost if run with IDE or cmake
}

int main(int argc, const char * argv[]) {

//cmake -B build
//cmake --build build
//cd build
//./equityExchangeTest

    
//docker build -t equityexchangetest .

// run from command line in docker container which launches kafka
// docker compose up --build
    
    
//to send the orders data to the matching engine
//cat inputfile.csv | nc -u -v -w 1 127.0.0.1 1234

//  brew services start kafka, brew services list
//  brew services stop kafka
    
    //using namespace kafka;
    //using namespace kafka::clients::producer;
    //using namespace kafka::clients::consumer;
    
    
    std::cout << "brokers ENV: " << getBrokers() << std::endl;
    
    const kafka::Properties propsConsumer({
        {"bootstrap.servers",           {getBrokers()}},
        {"group.id", {"my-group"}},
        {"auto.offset.reset",   {"earliest"}},               // ← the important one
        {"enable.auto.commit",  {"true"}}
    });

    kafka::clients::consumer::KafkaConsumer consumer(propsConsumer);
    consumer.subscribe({"my-topic"});
    
    
    
    const kafka::Properties propsProducer({
        {"bootstrap.servers",           {getBrokers()}},
        {"enable.idempotence",          {"true"}},

        // Fail after this many ms if the message cannot be delivered
        {"message.timeout.ms",         {"5000"}},   // 5 seconds (default is 300000)

        // Optional but useful extras
        {"socket.timeout.ms",          {"3000"}},   // network request timeout
//        {"request.timeout.ms",         {"3000"}},
        {"socket.connection.setup.timeout.ms", {"3000"}}
//        {"metadata.request.timeout.ms",{"3000"}},
    });
    
    kafka::clients::producer::KafkaProducer producer(propsProducer);
    std::string line = "Test kafka message to my-topic";
    
    kafka::clients::producer::ProducerRecord record("my-topic", kafka::NullKey, kafka::Value(line.data(), line.size()));

    auto deliveryCallback = [](const kafka::clients::producer::RecordMetadata& metadata, const kafka::Error& error) {
           if (!error) {
               std::cout << "Delivered: " << metadata.toString() << std::endl;
           } else {
               std::cerr << "Failed: " << error.message() << std::endl;
           }
       };
    
    producer.send(record, deliveryCallback);
    
    
    
    
    
//    while (true) {
//        auto records = consumer.poll(std::chrono::milliseconds(1000));
//        for (const auto& record : records) {
//            if (!record.error()) {
//                std::cout << "Got: " << record.value().toString() << std::endl;
//                break;
//            }
//        }
//    }
    
    
    
    
    
    producer.close();  // optional
    
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
