//
//  UDPServer.cpp
//  MatchingEngine
//
//
//

#include <string>
#include <iostream>
#include "UDPServer.hpp"
    
UDPServer::UDPServer(boost::asio::io_context& io_context, short port) :
     socket(io_context, boost::asio::ip::udp::udp::endpoint(boost::asio::ip::udp::udp::v4(), port)) {
    
        start_receive();
    }

UDPServer::~UDPServer() {
        socket.close();
}

//void UDPServer::ioServiceRun() {
//    io_service.run();
//}

void UDPServer::start_receive() {
    
    socket.async_receive_from(
        boost::asio::buffer(receiveBuffer),
        remote_endpoint,
        [this](boost::system::error_code ec, std::size_t bytes_transferred) {
            handle_receive(ec, bytes_transferred);
        }
    );
}

void UDPServer::handle_receive(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        
    if (!ec && bytes_transferred > 0) { 
    
        std::string newReceivedDataString(receiveBuffer.data(), bytes_transferred);
        
//        receivedDataString += newReceivedDataString;
        receivedDataString = newReceivedDataString;
        
//        if(bytes_transferred < UDBBufferSize) { //when < buff size then all data has been received
        
            UDPServer::parseInputBuffer();
//        }
        
//        receiveBuffer.fill(' ');
        start_receive(); //start listening for the next packet
    }
    else if (ec) {
        std::cerr << "Receive failed: " << ec.message() << std::endl;
    }
}

void UDPServer::parseInputBuffer() {
    
    // XX\U00000001X\U00000001X\U00000001
    // \U00000001
    
    receivedDataString.erase(
        remove(receivedDataString.begin(), receivedDataString.end(), ' '), receivedDataString.end()
    ); //remove spaces
    
    if(receivedDataString == "X") {
        std::string substring = "X"; //remove header data, seems to send 4 single X chars first
        size_t pos = receivedDataString.find(substring);
        if (pos != std::string::npos) {
             receivedDataString.erase(pos, substring.length());
        }
    }
    
    std::string substring = "\U00000001"; //remove header data
    size_t pos = receivedDataString.find(substring);
    if (pos != std::string::npos) {
         receivedDataString.erase(pos, substring.length());
    }
    
    std::vector<std::string> parsedOrderStringVector = split(receivedDataString, '\n'); //convert to a vector by parsing newline char

//TODO handle when UDP sends partial strings!!
    //check if last vector item has proper elements!!
//    std::string lastElement = parsedOrderStringVector.back();
//    std::vector<std::string> testStringVector = split(lastElement, ',');
//    if(testStringVector[0] == "N" && testStringVector.size() != 7) {
//        
//    }
//    else if(testStringVector[0] == "C" && testStringVector.size() != 3) {
//        
//    }
    
    // lock the mutex before modifying the shared vector
    {
        std::lock_guard<std::mutex> lock(mutexServer);
//        orderVector = parsedVectorOrderStringVector;
        orderVector.insert(orderVector.end(), parsedOrderStringVector.begin(), parsedOrderStringVector.end());
    }
    
}
    
std::vector<std::string> UDPServer::split(const std::string &s, char delim) {
    
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    
    return elems;
}

std::vector<std::string> UDPServer::getOrderVector() {
    
    std::lock_guard<std::mutex> lock(mutexServer);
    std::vector<std::string> currentOrderVector = orderVector;
    orderVector.clear();
    return currentOrderVector;
}
