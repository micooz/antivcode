/*
* Parser
* (c) Copyright 2015 Micooz
*
* Released under the Apache License.
* See the LICENSE file or
* http://www.apache.org/licenses/LICENSE-2.0.txt for more information.
*/
#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include <initializer_list>
#include <map>
#include <stdarg.h>
#include "generator/generator.h"
#include "parser/item.h"

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#endif

namespace program_options {

class Parser {
 public:
  typedef std::vector<std::string> ParameterList;
  typedef std::map<std::string, ParseItem*> ParseResult;

  Parser();

  /*
  * release memory allocated in parse()
  */
  ~Parser();

  /*
   * parse the command line by given argc and argv
   */
  ParseResult* parse(const int argc, const char** argv);

  /*
   * parse the command line by given command string
   */
  ParseResult* parse(const char* command_line);

  /*
   * check whether a certain option exist
   */
  bool has(const char* key);

  /*
   * check whether a sequence of options exist
   * example: has_or(3, "he", "or", "she");
   */
  DEPRECATED(bool has_or(int n, ...)) {
    va_list keys;
    va_start(keys, n);
    while (n--) {
      const char* key = va_arg(keys, const char*);
      if (this->has(key)) {
        return true;
      }
    }
    va_end(keys);
    return false;
  }

  /*
   * check whether a sequence of options exist
   * example: has_and(3, "he", "and", "she");
   */
  DEPRECATED(bool has_and(int n, ...)) {
    va_list keys;
    va_start(keys, n);
    while (n--) {
      const char* key = va_arg(keys, const char*);
      if (!this->has(key)) {
        return false;
      }
    }
    va_end(keys);
    return true;
  }

  /*
   * check whether a sequence of options exist using std::initializer_list
   * example: has_or({"he", "or", "she"});
   */
  bool has_or(std::initializer_list<const char*> options);

  /*
   * check whether a sequence of options exist using std::initializer_list
   * example: has_and({"he", "and", "she"});
   */
  bool has_and(std::initializer_list<const char*> options);

  /*
   * get the specified option value
   */
  ParseItem* get(const std::string& key);

  inline void set_usage_subroutines(
      const Generator::SubroutineCollection* subroutines) {
    subroutines_ = subroutines;
  }

  inline std::string get_subroutine_name() const { return subroutine_name_; }

 private:
  bool init(const int argc, const char** argv);

  void cleanup();

  void set_addition();

  const Generator::SubroutineCollection* subroutines_;
  std::string subroutine_name_;
  int argc_;
  ParameterList args_;
  ParseResult* pr_;
};
}

#endif  // PARSER_PARSER_H_
