//
//  UDPServer.cpp
//  MatchingEngine
//
//  Created by fernando marto on 2020-12-06.
//

#include "UDPServer.hpp"

UDPServer::UDPServer(boost::asio::io_service& io_service, boost::asio::ip::udp::endpoint remote_endpoint) :
remote_endpoint(remote_endpoint), socket(io_service, remote_endpoint) {
    
        start_receive();
    }
    
UDPServer::~UDPServer() {
        socket.close();
}

void UDPServer::ioServiceRun() {
    io_service.run();
}

void UDPServer::start_receive() {
    
    socket.async_receive_from(
                              boost::asio::buffer(receiveBuffer), remote_endpoint,
                              boost::bind(&UDPServer::handle_receive,
                                          this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred)
                              );
}

void UDPServer::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
        
    if (!error || error == boost::asio::error::message_size) {
    
        //
        // HERE IS WHERE I NEED TO DO A PRODUCER/CONSUMER EXCHANGE TO THE DEPTH BOOK WITH TRADE ORDERS
        //
        std::string newReceivedDataString(this->receiveBuffer.begin(), this->receiveBuffer.end());
        this->receivedDataString += newReceivedDataString;
        
        if(bytes_transferred < UDBBufferSize) { //when < buff size then all data has been received
        
            UDPServer::parseInputBuffer();
        }
        
        this->receiveBuffer.fill(' ');
        start_receive(); //start listening for the next request
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
    
    return this->orderVector;
}

void UDPServer::clearOrderVector() {
    
    std::mutex mutexOrderVector;
    std::unique_lock<std::mutex> threadLock(mutexOrderVector);
    
    this->orderVector.clear();
    
    threadLock.unlock();
}

void UDPServer::parseInputBuffer() {
    
    std::mutex mutexOrderVector;
    std::unique_lock<std::mutex> threadLock(mutexOrderVector);
  
    remove(this->receivedDataString.begin(), this->receivedDataString.end(), ' '); //remove spaces
    
    std::vector<std::string> parsedVectorOrderString = split(this->receivedDataString, '\n');
  
    this->orderVector = parsedVectorOrderString;
    
    threadLock.unlock();
        
}
    

//void udp_session::handle_request(const boost::system::error_code& error)
//{
//    if (!error || error == boost::asio::error::message_size)
//    {
//     //   message = make_daytime_string(); // let's assume this might be slow
//
//        // let the server coordinate actual IO
//     //   server_->enqueue_response(shared_from_this());
//    }
//}

