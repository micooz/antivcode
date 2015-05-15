/*
* parser error
* (c) Copyright 2015 Micooz
*
* Released under the Apache License.
* See the LICENSE file or
* http://www.apache.org/licenses/LICENSE-2.0.txt for more information.
*/
#ifndef PARSER_ERROR_H_
#define PARSER_ERROR_H_

#include <string>

namespace program_options {

class ParseError : public std::exception {
 public:
  explicit ParseError(const std::string& msg);

  const char* what() const throw();

  ~ParseError() throw();

 private:
  std::string _msg;
};

}

#endif  // PARSER_ERROR_H_