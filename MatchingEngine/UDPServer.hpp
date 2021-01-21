//
//  UDPServer.hpp
//  MatchingEngine
//
//  Created by fernando marto on 2020-12-06.
//

#ifndef UDPServer_hpp
#define UDPServer_hpp

//#include <stdio.h>

#include "boost/asio.hpp"
#include "boost/bind.hpp"

#include "boost/array.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

static const int UDBBufferSize = 1024;

class UDPServer
{
public:
    
    UDPServer(boost::asio::io_service& io_service, boost::asio::ip::udp::endpoint remote_endpoint);

    ~UDPServer();
    
    void ioServiceRun();
    
    void parseInputBuffer();
    
    std::vector<std::string> getOrderVector();
    void clearOrderVector();
    std::vector<std::string> split(const std::string &s, char delim);
    
private:
    
    void start_receive();

    void handle_receive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/);
    
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint remote_endpoint;
    boost::array<char, UDBBufferSize> receiveBuffer;
    std::string receivedDataString;
    std::vector<std::string> orderVector;
    
};


#endif /* UDPServer_hpp */
