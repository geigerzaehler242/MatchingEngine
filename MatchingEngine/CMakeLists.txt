#set(CMAKE_CXX_FLAGS "-pthread")


# Source library (change to add_executable if you prefer a single binary)
add_library(myMatchingEngineLib
    main.cpp DepthBook.cpp Exchange.cpp OrderTracker.cpp UDPServer.cpp
)

# Public include directory so consumers can #include headers from MatchingEngine/
target_include_directories(myMatchingEngineLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Boost + modern-cpp-kafka (which already pulls in librdkafka)
target_link_libraries(myMatchingEngineLib
    PUBLIC
        Boost::system
        Boost::filesystem
        modern-cpp-kafka
)

#create a binary to run
add_executable(myMatchingEngine
    main.cpp DepthBook.cpp Exchange.cpp OrderTracker.cpp UDPServer.cpp
)

target_include_directories(myMatchingEngine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Boost (and any other libraries your sources need)
target_link_libraries(myMatchingEngine
    PRIVATE
        myMatchingEngineLib
)

# Optional: install rules
# install(TARGETS mylib
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
#     RUNTIME DESTINATION bin
# )

install(TARGETS myMatchingEngine #needed by docker
     RUNTIME DESTINATION bin
)
