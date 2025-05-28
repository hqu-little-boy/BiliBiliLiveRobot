//
// Created by zeng on 24-6-26.
//

#include "Url.h"

#include "../Global/Config.h"

#include <algorithm>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/md5.h>
#include <ctime>
#include <fmt/format.h>
#include <vector>
Url::Url(const std::string_view& host, unsigned port, const std::string_view& target,
         const std::initializer_list<std::pair<std::string, std::string>>& query)
    : host(host)
    , port(port)
    , target(target)
// , query(query)
{
    for (const auto& [key, value] : query)
    {
        this->query.emplace(key, value);
    }
}

const std::string& Url::GetHost() const
{
    return this->host;
}

unsigned Url::GetPort() const
{
    return this->port;
}

const std::string& Url::GetTarget() const
{
    return this->target;
}

const std::string Url::GetTargetWithQuery() const
{
    if (this->query.empty())
    {
        return this->target;   // 如果没有查询参数，直接返回目标路径
    }
    return fmt::format("{}?{}", this->target, this->GetQueryString());
}
const std::string Url::GetTargetWithWbiParamSafeQuery() const
{
    if (this->query.empty())
    {
        return this->target;   // 如果没有查询参数，直接返回目标路径
    }
    return fmt::format("{}?{}", this->target, this->GetWbiParamSafeQueryString());
}

const std::map<std::string, std::string>& Url::GetQuery() const
{
    return this->query;
}
const std::string Url::GetQueryString() const
{
    // 构建查询字符串
    std::string strQuery;
    for (const auto& [key, value] : query)
    {
        if (strQuery.empty())
        {
            strQuery = fmt::format("{}={}", key, value);
        }
        else
        {
            strQuery = fmt::format("{}&{}={}", strQuery, key, value);
        }
    }
    return strQuery;
}
const std::string Url::GetWbiParamSafeQueryString() const
{
    // Step 2: Add "wts" if it doesn't exist
    // this->query["wts"] = std::to_string(now);

    // Step 3: Sort the keys lexicographically
    std::vector<std::string> sortedKeys;
    for (const auto& [key, value] : this->query)
    {
        sortedKeys.push_back(key);
    }
    std::sort(sortedKeys.begin(), sortedKeys.end());
    std::string strQuery;
    for (const auto& key : sortedKeys)
    {
        if (strQuery.empty())
        {
            strQuery = fmt::format("{}={}", key, this->query.find(key)->second);
        }
        else
        {
            strQuery = fmt::format("{}&{}={}", strQuery, key, this->query.find(key)->second);
        }
    }
    time_t now = std::time(nullptr);
    strQuery   = fmt::format("{}&{}={}", strQuery, "wts", std::to_string(now));

    std::string dataToHash = strQuery + Config::GetInstance()->GetWbiMixKey();

    // 计算 MD5 哈希
    CryptoPP::MD5  hash;
    CryptoPP::byte digest[CryptoPP::MD5::DIGESTSIZE];
    hash.Update(reinterpret_cast<const CryptoPP::byte*>(dataToHash.data()), dataToHash.size());
    hash.Final(digest);

    // 将哈希转换为小写十六进制字符串
    std::string md5Hex;
    {
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(md5Hex), false);   // false 表示小写
        encoder.Put(digest, sizeof(digest));
        encoder.MessageEnd();
    }

    return fmt::format("{}&{}={}", strQuery, "w_rid", md5Hex);
}

void Url::SetQuery(const std::initializer_list<std::pair<std::string, std::string>>& query)
{
    this->query.clear();
    for (const auto& [key, value] : query)
    {
        this->query.emplace(key, value);
    }
    // this->query = query;
}

std::string Url::ToString() const
{
    return fmt::format("{}:{}{}", host, port, this->GetTargetWithQuery());
}