#include "gtest/gtest.h"
#include "parser/parser.h"
#include "parser/error.h"

using namespace program_options;

TEST(TestParser, Parsing) {
  Parser parser;
  EXPECT_THROW(parser.parse("path/to/program -"), ParseError);
  EXPECT_THROW(parser.parse("path/to/program --"), ParseError);
  EXPECT_THROW(parser.parse("path/to/program -="), ParseError);
  EXPECT_THROW(parser.parse("path/to/program -o -"), ParseError);
  EXPECT_THROW(parser.parse("path/to/program -o --"), ParseError);
  EXPECT_THROW(parser.parse("path/to/program abc def"), ParseError);

  EXPECT_NO_THROW(parser.parse("path/to/program -g"));
  EXPECT_NO_THROW(parser.parse("path/to/program --g"));
  EXPECT_NO_THROW(parser.parse("path/to/program -a -b -c"));
  EXPECT_NO_THROW(parser.parse("path/to/program -abc"));
  EXPECT_NO_THROW(parser.parse("path/to/program -g-"));
  EXPECT_NO_THROW(parser.parse("path/to/program --g-"));
  EXPECT_NO_THROW(parser.parse("path/to/program -g,.%!"));
  EXPECT_NO_THROW(parser.parse("path/to/program -o="));
}

TEST(TestParser, Checking) {
  Parser parser;
  parser.parse("path/to/program -o --option -a -xyz");
  EXPECT_TRUE(parser.has("o"));
  EXPECT_TRUE(parser.has("option"));
  EXPECT_TRUE(parser.has("-oa"));
  EXPECT_TRUE(parser.has("y"));
  EXPECT_TRUE(parser.has("z"));
  EXPECT_TRUE(parser.has("-zy"));
  EXPECT_TRUE(parser.has_and({"o", "a"}));
  EXPECT_TRUE(parser.has_and({"o", "option"}));
  EXPECT_TRUE(parser.has_or({"o", "k"}));

  EXPECT_FALSE(parser.has(""));
  EXPECT_FALSE(parser.has("k"));
  EXPECT_FALSE(parser.has("ok"));
  EXPECT_FALSE(parser.has_and({"-ao", "k"}));
  EXPECT_FALSE(parser.has_and({}));
  EXPECT_FALSE(parser.has_or({}));
}

TEST(TestParser, Accessing) {
  Parser parser;
  parser.parse(
      "path/to/program -o 1 --option 2.3 -p=micooz --person=micooz -xyz 30");
  EXPECT_NE(parser.get("o"), nullptr);
  EXPECT_NE(parser.get("option"), nullptr);
  EXPECT_NE(parser.get("p"), nullptr);
  EXPECT_NE(parser.get("person"), nullptr);

  EXPECT_EQ(parser.get("k"), nullptr);
  EXPECT_EQ(parser.get("o")->as<int>(), 1);
  EXPECT_EQ(parser.get("z")->as<int>(), 30);
  EXPECT_EQ(parser.get("p")->val(), parser.get("person")->as<std::string>());

  EXPECT_DOUBLE_EQ(parser.get("option")->as<double>(), 2.3);
  EXPECT_STREQ(parser.get("p")->val().c_str(), "micooz");
}

TEST(TestParser, Overwriting) {
  Parser parser;
  parser.parse("path/to/program -o hello -o world");
  EXPECT_STREQ(parser.get("o")->val().c_str(), "world");
}

TEST(TestParser, Subroutine) {
  Parser parser;
  parser.parse("path/to/program subroutine -o 1 -p=q");
  EXPECT_EQ(parser.get("o")->as<int>(), 1);
  EXPECT_STREQ(parser.get_subroutine_name().c_str(), "subroutine");
  EXPECT_STREQ(parser.get("p")->val().c_str(), "q");
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
