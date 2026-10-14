//
//  main.cpp
//  MatchingEngineTest
//
//
//

#include <iostream>

#include <thread>
#include <chrono>
#include <tuple>
#include <limits.h>

#include "../MatchingEngine/OrderTracker.hpp" //#include <iostream>
#include "../MatchingEngine/DepthBook.hpp" //#include <map>
#include "../MatchingEngine/Exchange.hpp" //#include <thread>, #include <queue>

#include <gtest/gtest.h>

// function for which we write unit tests
int add(int num1, int num2) {
    return num1 + num2;
}

// to test addition of positive numbers
TEST(SumTest, ForPositiveNumbers) {
    EXPECT_EQ(35, add(23, 12));
}

// to test addition of negative numbers
TEST(SumTest, ForNegativeNumbers) {
    EXPECT_EQ(-1, add(-1, 0));
}

// to test addition of positive and negative numbers
TEST(SumTest, ForMixedNumbers) {
    
    //create exchange, run a Depth Book matching engine on a thread for each trading instrument on the exchange
    std::shared_ptr<Exchange> pExchange = std::make_shared<Exchange>(true); //true allows exchange to run
    std::thread tExchangeMatchingEngine(&Exchange::exchangeMatchingEngine, pExchange); //(pointer-to-member, object (or pointer), argument
    
    std::vector<std::string> orderCommand = {"N", "1", "TSLA", "10", "100", "B", "1"};
    pExchange->enterOrder(orderCommand);
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    pExchange->exchangeEnabled = false;
    
    if (tExchangeMatchingEngine.joinable()) {
        tExchangeMatchingEngine.join(); // Wait for the thread to finish
    }
    
    EXPECT_EQ(0, add(-1, 1));
}


int main(int argc, const char * argv[]) {
    
    testing::InitGoogleTest(&argc, (char**)argv);
    
    std::cout << "matching engine tests:" << std::endl;
    return RUN_ALL_TESTS();
    
}
