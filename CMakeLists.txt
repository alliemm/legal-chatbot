cmake_minimum_required(VERSION 3.20)
project(legal_chatbot)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DCROW_USE_BOOST)

if(WIN32)
    set(BOOST_ROOT "C:/local/boost_1_90_0")
    set(Boost_INCLUDE_DIR "C:/local/boost_1_90_0")
    set(Boost_LIBRARY_DIR "C:/local/boost_1_90_0/lib64-msvc-14.3")
endif()

if(APPLE)
    add_compile_options(-Wno-missing-field-initializers)
endif()

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/crow")

add_executable(legal_chatbot backend.cpp)

target_link_libraries(legal_chatbot PRIVATE Boost::boost)

if(WIN32)
    target_link_libraries(legal_chatbot PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(legal_chatbot PRIVATE Threads::Threads)
endif()