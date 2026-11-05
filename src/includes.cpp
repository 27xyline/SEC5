#include "includes.hpp"

#include <regex>
#include <stdexcept>

namespace {

const std::regex includePrefix(R"(^\s*#\s*include\s*$)");
const std::regex includeDirective(R"re(^\s*#\s*include\s*(?:<([^>]+)>|"([^"]+)")\s*$)re");

std::string withoutComments(const std::string& line, bool& inBlockComment) {
    std::string code;
    std::size_t i = 0;
    while (i < line.size()) {
        if (inBlockComment) {
            const auto end = line.find("*/", i);
            if (end == std::string::npos) {
                break;
            }
            inBlockComment = false;
            i = end + 2;
            continue;
        }

        if (line.compare(i, 2, "//") == 0) {
            break;
        }
        if (line.compare(i, 2, "/*") == 0) {
            inBlockComment = true;
            code += ' ';
            i += 2;
            continue;
        }

        const char start = line[i];
        const bool isHeader = (start == '<' || start == '"') &&
                              std::regex_match(code, includePrefix);
        code += line[i++];
        if (isHeader || start == '"' || start == '\'') {
            const char end = start == '<' ? '>' : start;
            // Inside a header name or literal, comment markers are ordinary text.
            while (i < line.size()) {
                const char current = line[i++];
                code += current;
                if (current == end) {
                    break;
                }
                if (!isHeader && current == '\\' && i < line.size()) {
                    code += line[i++];
                }
            }
        }
    }
    return code;
}

void checkRead(const std::istream& input) {
    if (input.bad() || !input.eof()) {
        throw std::runtime_error("could not read input");
    }
}

}  // namespace

std::vector<Include> readIncludes(std::istream& source) {
    std::vector<Include> includes;
    bool inBlockComment = false;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(source, line)) {
        ++lineNumber;
        const auto code = withoutComments(line, inBlockComment);
        std::smatch match;
        if (std::regex_match(code, match, includeDirective)) {
            includes.push_back({match[1].matched ? match[1].str() : match[2].str(),
                                lineNumber});
        }
    }
    checkRead(source);
    return includes;
}

AllowedHeaders readAllowedHeaders(std::istream& input) {
    AllowedHeaders allowed;
    std::string line;
    const char* whitespace = " \t\r\n\f\v";
    while (std::getline(input, line)) {
        const auto start = line.find_first_not_of(whitespace);
        if (start != std::string::npos) {
            const auto end = line.find_last_not_of(whitespace);
            allowed.insert(line.substr(start, end - start + 1));
        }
    }
    checkRead(input);
    return allowed;
}

std::vector<Include> findForbidden(const std::vector<Include>& includes,
                                   const AllowedHeaders& allowed) {
    std::vector<Include> forbidden;
    for (const auto& include : includes) {
        if (allowed.count(include.name) == 0) {
            forbidden.push_back(include);
        }
    }
    return forbidden;
}
