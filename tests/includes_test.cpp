#include "includes.hpp"

#include <gtest/gtest.h>
#include <sstream>

TEST(ReadIncludes, CollectsBothFormsAndPreservesLineNumbers) {
    std::istringstream source(
        "#include <vector>\n"
        "\n"
        "  # include \"lib/config.hpp\" // local header\n"
        "#include <vector>\n");
    const auto includes = readIncludes(source);

    ASSERT_EQ(includes.size(), 3u);
    EXPECT_EQ(includes[0].name, "vector");
    EXPECT_EQ(includes[0].line, 1u);
    EXPECT_EQ(includes[1].name, "lib/config.hpp");
    EXPECT_EQ(includes[1].line, 3u);
    EXPECT_EQ(includes[2].line, 4u);
}

TEST(ReadIncludes, IgnoresCommentsAndAcceptsCommentsBetweenTokens) {
    std::istringstream source(
        "// #include <ignored1>\n"
        "/* start\n"
        "#include \"ignored2\"\n"
        "*/ # /* gap */ include <string>\n");
    const auto includes = readIncludes(source);

    ASSERT_EQ(includes.size(), 1u);
    EXPECT_EQ(includes[0].name, "string");
    EXPECT_EQ(includes[0].line, 4u);
}

TEST(ReadIncludes, HandlesCommentMarkersInsideLiterals) {
    std::istringstream source(
        "const char* text = \"/*\";\n"
        "#include \"lib//config.hpp\"\n"
        "#include <vector>\n");
    const auto includes = readIncludes(source);

    ASSERT_EQ(includes.size(), 2u);
    EXPECT_EQ(includes[0].name, "lib//config.hpp");
    EXPECT_EQ(includes[1].name, "vector");
}

TEST(ReadIncludes, SkipsMacroIncludesAndIncompleteDirectives) {
    std::istringstream source(
        "#include HEADER\n"
        "#include <unfinished\n"
        "#include_next <string>\n");
    EXPECT_TRUE(readIncludes(source).empty());
}

TEST(ReadAllowedHeaders, TrimsWhitespaceAndDeduplicates) {
    std::istringstream input(" vector  \r\n\n\tlib/config.hpp\r\nvector\n");
    const AllowedHeaders expected{"vector", "lib/config.hpp"};
    EXPECT_EQ(readAllowedHeaders(input), expected);
}

TEST(FindForbidden, ReportsEveryOccurrenceOutsideAllowedSet) {
    const std::vector<Include> includes{{"vector", 1}, {"debug.hpp", 2}, {"debug.hpp", 4}};
    const auto forbidden = findForbidden(includes, {"vector"});
    ASSERT_EQ(forbidden.size(), 2u);
    EXPECT_EQ(forbidden[0].name, "debug.hpp");
    EXPECT_EQ(forbidden[0].line, 2u);
    EXPECT_EQ(forbidden[1].line, 4u);
}

TEST(FindForbidden, AcceptsAllowedHeaders) {
    EXPECT_TRUE(findForbidden({{"vector", 1}}, {"vector"}).empty());
}
