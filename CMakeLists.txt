cmake_minimum_required(VERSION 3.28)
project(BiliBiliLiveRobot)

if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif ()

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -fsanitize=address -fsanitize=undefined -fsanitize=leak") # 指定编译选项
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fsanitize=undefined") # 指定编译选项

# 添加源文件
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

add_custom_command(TARGET BiliBiliLiveRobot PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Config"
)

add_custom_command(TARGET BiliBiliLiveRobot PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/Config/configure.json"
        "${CMAKE_BINARY_DIR}/Config/configure.json"
)
# 加入Boost库
find_package(Boost COMPONENTS context iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
add_subdirectory(ThirdParty/json)
include_directories(ThirdParty/json/include)
add_subdirectory(ThirdParty/brotli)
include_directories(ThirdParty/brotli/c/include)
add_subdirectory(ThirdParty/libqrencode)
include_directories(ThirdParty/libqrencode)
add_subdirectory(ThirdParty/concurrencpp)
include_directories(ThirdParty/concurrencpp/include)
add_subdirectory(ThirdParty/utfcpp)
include_directories(ThirdParty/utfcpp/source)
target_link_libraries(BiliBiliLiveRobot PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::context
        Boost::iostreams
        nlohmann_json::nlohmann_json
        qrencode
        brotlidec
        concurrencpp
        utf8cpp
)
