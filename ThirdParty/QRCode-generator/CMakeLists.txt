cmake_minimum_required(VERSION 3.28)
project(qrcodegen)
# 添加库
add_library(qrcodegen STATIC include/QrCode.h src/QrCode.cpp)