cmake_minimum_required(VERSION 3.21)
project(BiliBiliLiveRobot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_${CMAKE_BUILD_TYPE}Bin)
if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -fsanitize=address -fsanitize=undefined -fsanitize=leak") # 指定编译选项
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fsanitize=undefined") # 指定编译选项

# libqrencode使用，不需要PNG，不需要工具，不需要测试
set(WITHOUT_PNG ON)
set(WITH_TOOLS OFF)
set(WITH_TESTS OFF)

set(STATIC_BUILD OFF)
if (STATIC_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(BUILD_SHARED_LIBS OFF)
    set(Boost_USE_STATIC_LIBS ON)
    set(OPENSSL_USE_STATIC_LIBS ON)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif ()

set(Test ON)
set(CRYPTOPP_BUILD_TESTING OFF)
if (Test)
    add_definitions(-DTEST)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
    #    if (NOT STATIC_BUILD)
    #        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    #        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-var-tracking")
    #        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-stack-protector")
    #        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize-address-use-after-scope")
    #        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=signed-integer-overflow")
    #    endif ()
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=native")
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-rtti")
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-stack-protector")
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer")
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-var-tracking")
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-strict-aliasing")
endif ()


if (APPLE)
    add_definitions(-DMACOS)
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
    add_definitions(-D_WIN32_WINNT=0x0601)
    include_directories(
            ${VCPKG_INCLUDE_DIRS}
    )
    target_link_libraries(${PROJECT_NAME}
            ${VCPKG_LIBRARIES}
    )
endif ()

# 如果编译器是Clang，添加编译选项
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # 链接libc++，而不是libstdc++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif ()

# 添加源文件
file(GLOB_RECURSE BASE_SOURCES "Base/*.h" "Base/*.cpp")
file(GLOB_RECURSE ENTITY_SOURCES "Entity/*.h" "Entity/*.cpp")
file(GLOB_RECURSE UTIL_SOURCES "Util/*.h" "Util/*.cpp")
file(GLOB_RECURSE GUI_SOURCES "GUI/*.h" "GUI/*.cpp")
file(GLOB_RECURSE RELATIVE GUI_QML_SOURCES "GUI/*.qml")

add_executable(${PROJECT_NAME}-cli
        cli.cpp
        ${BASE_SOURCES}
        ${ENTITY_SOURCES}
        ${UTIL_SOURCES}
        #        qml/qml.qrc
)

add_custom_command(TARGET ${PROJECT_NAME}-cli PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX}/Config"
)

add_custom_command(TARGET ${PROJECT_NAME}-cli PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/Config/configure.json"
        "${CMAKE_INSTALL_PREFIX}/Config/configure.json"
)

# 加入Boost库
find_package(Boost COMPONENTS context iostreams regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
find_package(zstd CONFIG REQUIRED)
add_subdirectory(ThirdParty/json)
include_directories(ThirdParty/json/include)
add_subdirectory(ThirdParty/brotli)
include_directories(ThirdParty/brotli/c/include)
add_subdirectory(ThirdParty/libqrencode)
include_directories(ThirdParty/libqrencode)
#add_subdirectory(ThirdParty/concurrencpp)
#include_directories(ThirdParty/concurrencpp/include)
add_subdirectory(ThirdParty/utfcpp)
include_directories(ThirdParty/utfcpp/source)
add_subdirectory(ThirdParty/fmt)
include_directories(ThirdParty/fmt/include)
# 查找 Crypto++ 库
add_subdirectory(ThirdParty/cryptopp-cmake)
# 添加SQLiteCpp
add_subdirectory(ThirdParty/SQLiteCpp)
include_directories(ThirdParty/SQLiteCpp/include)
target_link_libraries(${PROJECT_NAME}-cli PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::context
        Boost::iostreams
        Boost::regex
        nlohmann_json::nlohmann_json
        qrencode
        brotlidec
        utf8cpp
        cryptopp::cryptopp
        fmt::fmt-header-only
        zstd::libzstd
        SQLiteCpp
        sqlite3
)
