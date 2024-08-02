//
// Created by zeng on 24-6-10.
//

#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class noncopyable
{
public:
    // 禁止拷贝构造函数
    noncopyable(const noncopyable&) = delete;
    // 禁止赋值操作符
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable()  = default;
    ~noncopyable() = default;
};
#endif   // NONCOPYABLE_H
