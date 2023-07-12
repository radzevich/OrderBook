cmake_minimum_required(VERSION 3.24)
project(BinanceBook)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_subdirectory(src)
    add_executable(BinanceBook main.cpp)
#    target_link_libraries(BinanceBook Boost::container Boost::pool)
endif()