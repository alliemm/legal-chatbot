cmake_minimum_required(VERSION 3.20)
project(legal_chatbot)

set(CMAKE_CXX_STANDARD 17)

find_package(asio CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(libenvpp CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)

add_executable(legal_chatbot backend.cpp)

target_link_libraries(legal_chatbot PRIVATE
        asio::asio
        Crow::Crow
        libenvpp::libenvpp
        libpqxx::pqxx
        unofficial-sodium::sodium
        shlwapi
        ws2_32
        secur32
)

