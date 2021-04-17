cmake_minimum_required(VERSION 3.17)
project(xml_cpp)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost REQUIRED context)
find_package(EXPAT REQUIRED)

add_executable(xml_cpp main.cpp xml-cpp/parser.cpp xml-cpp/parser.hpp)
target_link_libraries(xml_cpp PUBLIC Boost::context EXPAT::EXPAT)