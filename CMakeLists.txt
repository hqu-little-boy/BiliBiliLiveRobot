cmake_minimum_required(VERSION 3.28)
project(BiliBiliLiveRobot)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -fsanitize=address -fsanitize=undefined -fsanitize=leak") # 指定编译选项

add_executable(BiliBiliLiveRobot
        main.cpp
        Entity/BiliLiveSession.h
        Entity/BiliLiveSession.cpp
        Base/copyable.h
        Base/Logger.cpp
        Base/Logger.h
        Base/noncopyable.h
        Base/TimeStamp.cpp
        Base/TimeStamp.h
        Util/BiliApiUtil.h
        Util/BiliApiUtil.cpp
        Entity/Url.h
        Entity/Url.cpp
        Entity/Config.h
        Entity/Config.cpp
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
#find_package(gperftools REQUIRED)
target_link_libraries(BiliBiliLiveRobot PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::context
        Boost::filesystem
        Boost::system
        Boost::thread
        Boost::iostreams
        nlohmann_json::nlohmann_json
        brotlidec
#        tcmalloc
)
