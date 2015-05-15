#include "generator/subroutine.h"
#include <algorithm>
#include <iostream>

namespace program_options {

Subroutine::Subroutine() : first_line_("") {}

Subroutine::Subroutine(const char* name, const char* description)
    : first_line_(""), description_(description), name_(name) {
  usages_.reserve(5);
}

void Subroutine::print_with_row(std::ostream& out) {
  // print the subroutine name and its description
  if (strcmp(get_first_line(), "") != 0) {
    // print the first line
    out << get_first_line();
    if (!usages_.empty()) {
      out << std::endl;
    }
  }
  auto begin = usages_.begin();
  auto end = usages_.end();

  std::vector<std::stringstream> row_list;
  row_list.reserve(usages_.size());

  // build usage rows without description field,
  // find the max-len row at the same time.
  size_t max_len = 0;
  std::for_each(begin, end, [&max_len, &row_list](const Row& row) {
    std::stringstream ss;
    ss << "  ";
    if (!row.oshort().empty()) {
      ss << "-" << row.oshort() << " ";
    }
    if (!row.olong().empty()) {
      if (!row.oshort().empty())
        ss << "[ --" << row.olong() << " ] ";
      else
        ss << "--" << row.olong() << " ";
    }

    if (row.required()) {
      ss << "arg ";
      if (!row.value().empty()) {
        ss << "= " << row.value() << " ";
      }
    }

    max_len = std::max(max_len, ss.str().size());
    row_list.push_back(std::move(ss));
  });

  // show all rows and align description field
  size_t row_count = usages_.size();
  for (size_t i = 0; i < row_count; ++i) {
    std::string str_row(std::move(row_list[i].str()));
    // print row without description
    out << str_row;
    // print spaces
    size_t spaces = 0;
    size_t len = str_row.size();
    if (max_len > len) spaces = max_len - len;

    while (spaces--) {
      out << " ";
    }
    // print description
    out << usages_.at(i).desc() << std::endl;
  }
}

void Subroutine::print_with_template(std::ostream& out) {
  for (auto usage : usages_) {
    size_t i = 0;
    for (auto t = template_str_.begin(); t != template_str_.end(); ++t) {
      if (*t == '%') {
        switch (*(order_.begin() + i)) {
          case Row::kShort:
            out << usage.oshort();
            break;
          case Row::kLong:
            out << usage.olong();
            break;
          case Row::kDefault:
            out << usage.value();
            break;
          case Row::kDescription:
            out << usage.desc();
            break;
          default:
            break;
        }
        ++i;
      } else {
        out << *t;
      }  // if %
    }    // for template_str_
    out << std::endl;
  }  // for usages_
}

std::ostream& operator<<(std::ostream& out, Subroutine& subroutine) {
  if (subroutine.template_str_.empty()) {
    subroutine.print_with_row(out);
  } else {
    subroutine.print_with_template(out);
  }
  return out;
}
}
