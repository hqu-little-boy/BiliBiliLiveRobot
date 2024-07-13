//
// Created by zeng on 24-7-13.
//

#ifndef MULTIPART_H
#define MULTIPART_H
#include <cstdint>
#include <list>
#include <string>
#include <vector>


// class Part
// {
// public:
//     Part(const std::string& name, const std::string& value, const std::string& contentType = {});
//     Part(const std::string& name, const int32_t& value, const std::string& contentType = {});
//
// private:
//     std::string name;
//     std::string value;
//     std::string contentType;
//     size_t      datalen{0};
// };

class Multipart
{
public:
    Multipart(const std::initializer_list<std::pair<std::string, std::string>>& parts);
    Multipart(const std::list<std::pair<std::string, std::string>>& parts);
    Multipart(const std::list<std::pair<std::string, std::string>>&& parts);

    std::string GetSerializeMultipartFormdataGetContentType();
    std::string GetSerializeMultipartFormdata();

private:
    std::string MakeMultipartDataBoundary();
    std::string RandomString(uint16_t length);

    std::string                                    boundary;
    std::list<std::pair<std::string, std::string>> parts;
};



#endif   // MULTIPART_H
