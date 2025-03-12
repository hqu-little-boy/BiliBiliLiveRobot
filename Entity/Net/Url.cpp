//
// Created by zeng on 24-6-26.
//

#include "Url.h"

#include <fmt/format.h>

Url::Url(const std::string_view& host, unsigned port, const std::string_view& target,
         const std::list<std::pair<std::string, std::string>>& query)
    : host(host)
    , port(port)
    , target(target)
    , query(query)
{
}

const std::string& Url::GetHost() const
{
    return host;
}

unsigned Url::GetPort() const
{
    return port;
}

const std::string& Url::GetTarget() const
{
    return target;
}

const std::string Url::GetTargetWithQuery() const
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
    return fmt::format("{}?{}", target, strQuery);
}

const std::list<std::pair<std::string, std::string>>& Url::GetQuery() const
{
    return query;
}

void Url::SetQuery(const std::list<std::pair<std::string, std::string>>& query)
{
    this->query = query;
}

std::string Url::ToString() const
{
    return fmt::format("{}:{}{}", host, port, this->GetTargetWithQuery());
}