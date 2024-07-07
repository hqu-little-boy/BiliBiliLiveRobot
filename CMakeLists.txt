cmake_minimum_required(VERSION 3.28)
project(BiliBiliLiveRobot)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -fsanitize=address -fsanitize=undefined -fsanitize=leak") # 指定编译选项
file(GLOB_RECURSE BASE_SOURCES "Base/*.h" "Base/*.cpp")
file(GLOB_RECURSE ENTITY_SOURCES "Entity/*.h" "Entity/*.cpp")
file(GLOB_RECURSE UTIL_SOURCES "Util/*.h" "Util/*.cpp")
file(GLOB_RECURSE THREADPOOL_SOURCES "ThreadPool/*.h" "ThreadPool/*.cpp")
add_executable(BiliBiliLiveRobot
        main.cpp
        ${BASE_SOURCES}
        ${ENTITY_SOURCES}
        ${UTIL_SOURCES}
        ${THREADPOOL_SOURCES}
)

# 加入Boost库
find_package(Boost COMPONENTS context filesystem system thread iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
add_subdirectory(ThirdParty/json)
include_directories(ThirdParty/json/include)
add_subdirectory(ThirdParty/brotli)
include_directories(ThirdParty/brotli/c/include)
#add_subdirectory(ThirdParty/QRCode-generator)
#include_directories(ThirdParty/QRCode-generator/include)
add_subdirectory(ThirdParty/libqrencode)
include_directories(ThirdParty/libqrencode)
target_link_libraries(BiliBiliLiveRobot PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::context
        Boost::filesystem
        Boost::system
        Boost::thread
        Boost::iostreams
        nlohmann_json::nlohmann_json
        #        qrcodegen
        #        qrenc
        qrencode
        brotlienc
        brotlidec
        brotlicommon
)
