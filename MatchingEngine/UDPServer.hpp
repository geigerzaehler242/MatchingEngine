//
//  UDPServer.hpp
//  MatchingEngine
//
//
//

#ifndef UDPServer_hpp
#define UDPServer_hpp

//#include <stdio.h>

#include "boost/asio.hpp"

static const int UDBBufferSize = 4096;

class UDPServer
{
public:
    
    UDPServer(boost::asio::io_context &io_context, short port);

    ~UDPServer();
    
    void parseInputBuffer();
    std::vector<std::string> getOrderVector();
    std::vector<std::string> split(const std::string &s, char delim);
    //void server_run(boost::asio::io_context io_context);
    //void clearOrderVector();
    //void ioServiceRun();
    
private:
    
    void start_receive();
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
    
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint remote_endpoint;
    std::array<char, UDBBufferSize> receiveBuffer;
    std::string receivedDataString;
    std::vector<std::string> orderVector;
    std::mutex mutexServer;
};


#endif /* UDPServer_hpp */
