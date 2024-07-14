# BiliBiliLiveRobot
## 本项目概述
本项目是一个基于C++ boost 异步处理的B站直播弹幕机器人，可以实现自动回复弹幕、自动发送弹幕、自动抽奖等功能。

## 功能列表
* 弹幕欢迎（普通用户，舰长用户）
  *  指定用户欢迎
  *  自定义欢迎词

* 关注答谢
* 分享感谢
* 下播公告
* 礼物答谢
* 自定义礼物答谢延迟
* 盲盒统计
* PK 相关
* PK 输出对手信息
* 对方用户串门识别
## 构建环境
**C++ 23**
**CMake 3.21+**
**Boost 1.83+**
**iconv**
**openssl**
**zlib**

## 构建步骤
```bash
mkdir build
cd build/
cmake  -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/cc -DCMAKE_CXX_COMPILER=/usr/bin/c++ ..
cmake --build . -j$(nproc)
```
## 使用说明
```bash
./BiliBiliLiveRobot
```