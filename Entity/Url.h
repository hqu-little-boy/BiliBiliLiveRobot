//
// Created by zeng on 24-6-26.
//

#ifndef URL_H
#define URL_H
#include "../Base/copyable.h"

#include <list>
#include <string>
#include <unordered_map>
class Url : public copyable
{
public:
    Url() = default;
    Url(const std::string_view& host, unsigned port, const std::string_view& target,
        const std::list<std::pair<std::string, std::string>>& query = {});

    [[nodiscard]] const std::string& GetHost() const;
    [[nodiscard]] unsigned           GetPort() const;
    [[nodiscard]] const std::string& GetTarget() const;
    [[nodiscard]] const std::string  GetTargetWithQuery() const;

    [[nodiscard]] const std::list<std::pair<std::string, std::string>>& GetQuery() const;
    [[nodiscard]] std::string                                           ToString() const;

private:
    std::string                                    host;
    unsigned                                       port{};
    std::string                                    target;
    std::list<std::pair<std::string, std::string>> query;
};



#endif   // URL_H
