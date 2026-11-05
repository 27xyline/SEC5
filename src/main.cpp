#include "includes.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace {

std::ifstream openFile(const char* path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error(std::string("could not open ") + path);
    }
    return file;
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string_view command = argc > 1 ? argv[1] : "";
    const bool list = command == "list" && argc == 3;
    const bool check = command == "check" && argc == 4;
    if (!list && !check) {
        std::cerr << "Usage:\n"
                  << "  sec5 list <source.cpp>\n"
                  << "  sec5 check <source.cpp> <allowed.txt>\n";
        return 2;
    }

    try {
        auto source = openFile(argv[2]);
        const auto includes = readIncludes(source);
        if (list) {
            AllowedHeaders printed;
            for (const auto& include : includes) {
                if (printed.insert(include.name).second) {
                    std::cout << include.name << '\n';
                }
            }
            return 0;
        }

        auto allowlistFile = openFile(argv[3]);
        const auto allowed = readAllowedHeaders(allowlistFile);
        const auto forbidden = findForbidden(includes, allowed);
        for (const auto& include : forbidden) {
            std::cout << argv[2] << ':' << include.line
                      << ": header not allowed: " << include.name << '\n';
        }
        return forbidden.empty() ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << "sec5: " << error.what() << '\n';
        return 2;
    }
}
