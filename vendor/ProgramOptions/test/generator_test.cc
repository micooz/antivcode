#include "gtest/gtest.h"
#include "generator/generator.h"

using namespace program_options;

TEST(TestGenerator, generating) {
  Generator gen;
  gen.make_usage("");
  EXPECT_STREQ(gen.to_string().c_str(), "");
  
  gen.make_usage("this is an usage information");
  EXPECT_STREQ(gen.to_string().c_str(), "this is an usage information");

  gen.make_usage("usage of subroutine_1:")
     .add_subroutine
    ("subroutine_1", "description of subroutine_1")
    ("a,", "option with a short-form")
    (",long", "option with a long-form")
    ("option", "this option will be ignored")
    ("o,option", "option with both short and long form");
  EXPECT_STREQ(gen("subroutine_1").to_string().c_str(), "  -a              option with a short-form\n  --long          option with a long-form\n  -o [ --option ] option with both short and long form\n");

  gen.add_subroutine
    ("subroutine_2", "description of subroutine_2")
    ("o,option", "", "option require a value")
    ("p,person", "micooz", "option with default value");
  EXPECT_STREQ(gen("subroutine_2").to_string().c_str(), "  -o [ --option ] arg          option require a value\n  -p [ --person ] arg = micooz option with default value\n");

  auto kv = gen.get_subroutine_list();
  std::stringstream ss;
  for(auto sub : kv) {
    ss << "\t" << sub.first << "\t" << sub.second << std::endl;
  }
  EXPECT_STREQ(ss.str().c_str(), "\tsubroutine_1\tdescription of subroutine_1\n\tsubroutine_2\tdescription of subroutine_2\n");
}

TEST(TestGenerator, templatizing){
  Generator gen;
  gen.make_template("% - % = % [%]",
                    {Row::kShort, Row::kLong, Row::kDefault, Row::kDescription});
  gen.make_usage("the program usage:")
    ("o,option", "value", "description");
  EXPECT_STREQ(gen(Subroutine::get_default_name()).to_string().c_str(),
               "o - option = value [description]\n");
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
