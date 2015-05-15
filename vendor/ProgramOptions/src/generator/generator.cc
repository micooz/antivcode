#include "generator/generator.h"
#include "parser/parser.h"

namespace program_options {

Generator::Generator() : parser_(nullptr) {
  current_subroutine_ = Subroutine::get_default_name();
  add_subroutine(current_subroutine_.c_str());
}

Generator::~Generator() {
  if (parser_) {
    delete parser_;
    parser_ = nullptr;
  }
  for (auto it = subroutines_.begin(); it != subroutines_.end(); ++it) {
    if (it->second) {
      delete it->second;
      it->second = nullptr;
    }
  }
}

Generator& Generator::make_usage(const char* first_line) {
  get_subroutine()->set_first_line(first_line);
  return *this;
}

Parser* Generator::make_parser() {
  if (parser_) delete parser_;
  parser_ = new Parser;
  parser_->set_usage_subroutines(&subroutines_);
  return parser_;
}

Generator& Generator::add_subroutine(const char* name) {
  add_subroutine(name, "");
  return *this;
}

Generator& Generator::add_subroutine(const char* name,
                                     const char* description) {
  if (subroutines_.find(name) == subroutines_.end()) {
    // a new subroutine
    current_subroutine_ = name;
    Subroutine* routine = new Subroutine(name, description);
    subroutines_.insert({current_subroutine_, routine});
  }
  return *this;
}

std::map<std::string, std::string> Generator::get_subroutine_list() {
  std::map<std::string, std::string> kv;
  for (auto pr : subroutines_) {
    Subroutine* subroutine = pr.second;
    if (subroutine->get_name() != Subroutine::get_default_name())
      kv[subroutine->get_name()] = subroutine->get_description();
  }
  return std::move(kv);
}

bool Generator::add_usage_line(const char* option, const char* default_value,
                               const char* description) {
  std::string option_str(option);
  auto delimiter_pos = option_str.find(kDelimiter);

  std::string option_short;
  std::string option_long;

  if (delimiter_pos != std::string::npos) {
    option_short.assign(std::move(option_str.substr(0, delimiter_pos)));
    option_long.assign(std::move(option_str.substr(delimiter_pos + 1)));

    Row row;
    row.oshort(option_short);
    row.olong(option_long);
    row.value(default_value);
    row.desc(description);

    get_subroutine()->add_usage_line(row);
    return true;
  }
  return false;
}

std::ostream& operator<<(std::ostream& out, Generator& generator) {
  for (auto pr : generator.subroutines_) {
    Subroutine* subroutine = pr.second;
    if (subroutine->get_name() != Subroutine::get_default_name()) {
      out << subroutine->get_name() << "\t";
    }
    out << subroutine->get_description();
    if (!subroutine->get_usage().empty()) {
      out << std::endl;
    }
    out << *subroutine;
  }
  return out;
}
}
