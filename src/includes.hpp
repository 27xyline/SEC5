#pragma once

#include <cstddef>
#include <istream>
#include <string>
#include <unordered_set>
#include <vector>

struct Include {
    std::string name;
    std::size_t line;
};

using AllowedHeaders = std::unordered_set<std::string>;

std::vector<Include> readIncludes(std::istream& source);
AllowedHeaders readAllowedHeaders(std::istream& input);
std::vector<Include> findForbidden(const std::vector<Include>& includes,
                                   const AllowedHeaders& allowed);
