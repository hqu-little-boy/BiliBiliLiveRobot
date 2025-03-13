//
// Created by zpf on 25-3-12.
//

#ifndef ROBOT_H
#define ROBOT_H
#include <boost/asio/io_context.hpp>
#include <thread>


class BiliLiveSession;
/// @brief 机器人类，调用Run方法启动机器人
class Robot
{
public:
    Robot();
    ~Robot() = default;

    /// @brief 启动机器人
    void Run();
    void Stop();
    bool IsRunning() const;

private:
    void RobotThread();

private:
    std::atomic<bool>                        stopFlag;
    std::thread                              robotThread;
    std::unique_ptr<boost::asio::io_context> iocPtr;
    std::shared_ptr<BiliLiveSession>         liveSessionPtr;
};



#endif //ROBOT_H
