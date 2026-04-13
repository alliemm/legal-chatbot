cmake_minimum_required(VERSION 3.20)
project(legal_chatbot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(asio CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(libenvpp CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

add_executable(legal_chatbot backend.cpp)

target_include_directories(legal_chatbot PRIVATE ${SODIUM_INCLUDE_DIRS})

target_link_libraries(legal_chatbot PRIVATE
        asio::asio
        Crow::Crow
        CURL::libcurl
        libenvpp::libenvpp
        libpqxx::pqxx
        ${SODIUM_LIBRARIES}
)

target_link_directories(legal_chatbot PRIVATE ${SODIUM_LIBRARY_DIRS})
