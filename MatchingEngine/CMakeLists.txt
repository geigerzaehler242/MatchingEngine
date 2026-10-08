cmake_minimum_required(VERSION 3.25)
project(equityExchange)

# greater than 20 has build errors
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-pthread")

add_executable(equityexchangetest)

#set(Boost_DIR "/opt/homebrew/Cellar/boost/1.92.0/include/boost/stage/lib/cmake/Boost-*VERSION*")
# set(Boost_USE_STATIC_LIBS ON)
find_package(Boost CONFIG COMPONENTS program_options)
target_link_libraries(equityexchangetest PUBLIC ${Boost_LIBRARIES})

target_sources(equityexchangetest
  PRIVATE
    main.cpp DepthBook.cpp Exchange.cpp OrderTracker.cpp UDPServer.cpp
)
