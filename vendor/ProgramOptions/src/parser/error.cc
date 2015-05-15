#include "parser/error.h"

namespace program_options {

ParseError::ParseError(const std::string& msg) : _msg(msg) {}

const char* ParseError::what() const throw() {
  std::string msg;
  msg.append("Command line parse error: ").append(_msg).push_back('.');
  return msg.c_str();
}

ParseError::~ParseError() throw() {}

}
