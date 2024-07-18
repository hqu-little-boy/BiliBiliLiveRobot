//
// Created by zeng on 24-7-13.
//

#include "Multipart.h"

#include <format>
#include <random>
#include <array>

// Part::Part(const std::string& name, const std::string& value, const std::string& contentType)
//     : name{name}
//     , value{value}
//     , contentType{contentType}
// {
// }
//
// Part::Part(const std::string& name, const int32_t& value, const std::string& contentType)
//     : name{name}
//     , value{std::to_string(value)}
//     , contentType{contentType}
// {
// }
//
Multipart::Multipart(const std::initializer_list<std::pair<std::string, std::string>>& parts)
    : parts{parts}
    , boundary{MakeMultipartDataBoundary()}
{
}

Multipart::Multipart(const std::list<std::pair<std::string, std::string>>& parts)
    : parts{parts}
    , boundary{MakeMultipartDataBoundary()}
{
}

Multipart::Multipart(const std::list<std::pair<std::string, std::string>>&& parts)
    : parts{parts}
    , boundary{MakeMultipartDataBoundary()}
{
}

std::string Multipart::MakeMultipartDataBoundary()
{
    return std::format("----WebKitFormBoundary{}", this->RandomString(16));
}

std::string Multipart::RandomString(uint16_t length)
{
    constexpr std::array<char, 62> data{
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

    // std::random_device might actually be deterministic on some
    // platforms, but due to lack of support in the c++ standard library,
    // doing better requires either some ugly hacks or breaking portability.
    static std::random_device seed_gen;

    // Request 128 bits of entropy for initialization
    static std::seed_seq seed_sequence{seed_gen(), seed_gen(), seed_gen(), seed_gen()};

    static std::mt19937 engine(seed_sequence);

    std::string result;
    for (size_t i = 0; i < length; i++)
    {
        result += data[engine() % data.size()];
    }
    return result;
}

std::string Multipart::GetSerializeMultipartFormdataGetContentType()
{
    return std::format("multipart/form-data; boundary={}", this->boundary);
}

std::string Multipart::GetSerializeMultipartFormdata()
{
    std::string body;

    for (const auto& part : this->parts)
    {
        body += std::format("--{}\r\n", this->boundary);
        body += std::format("Content-Disposition: form-data; name=\"{}\"\r\n\r\n", part.first);
        body += part.second;
        body += "\r\n";
    }
    body += std::format("--{}--\r\n", this->boundary);
    return body;
}