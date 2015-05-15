#include <algorithm>
#include "parser/parser.h"
#include "parser/error.h"

using namespace std;

namespace program_options {

ParseItem* Parser::get(const string& key) {
  if (pr_->find(key) != pr_->end()) {
    return (*pr_)[key];
  }
  return nullptr;
}

Parser::Parser()
        : subroutines_(nullptr), pr_(nullptr) { }

Parser::~Parser() { this->cleanup(); }

Parser::ParseResult* Parser::parse(const int argc, const char** argv) {
  if (!this->init(argc, argv)) {
    return nullptr;
  }
  auto ibegin = args_.begin() + 1;  // ignore the first cmd name
  auto iend = args_.end();
  auto it = ibegin;

  if (argc >= 2 && args_[1][0] != '-') {
    // the second block may be a subroutine name
    // e.g., ./exec pull --option
    if (subroutines_ && (subroutines_->find(args_[1]) != subroutines_->end())) {
      subroutine_name_ = args_[1];
      it++;  // ignore the subroutine name
    } else {
      subroutine_name_ = args_[1];
    }
  } else {
    // there is no options as well as subroutine name
    // e.g., ./exec
    subroutine_name_ = Subroutine::get_default_name();
  }

  string block;
  string previous(*ibegin);

  for (; it != iend; ++it) {
    block.assign(*it);

    switch (block.size()) {
      case 1:
        if (block == "-") {
          throw ParseError("single '-' is not allowed");
        }
        break;
      case 2:
        if (block[0] == '-') {
          if (block[1] == '-') {
            throw ParseError("option '--' is incomplete");
          } else if (block[1] == '=') {
            throw ParseError("option '-=' is invalid");
          } else {
            // single option
            // e.g., ./exec -s
            (*pr_)[block.substr(1)] = nullptr;
          }
        }
        break;
      default:  // >=3
        if (block[0] == '-') {
          if (block[1] == '-') {
            size_t pos_equal = block.find('=');
            if (pos_equal == string::npos) {
              // a long format option
              // e.g., ./exec --option
              (*pr_)[block.substr(2)] = nullptr;
            } else {
              if (pos_equal > 3) {
                // e.g, ./exec --op[..=]value
                string key(block.substr(2, pos_equal - 2));
                if (block.size() > 5)
                  // e.g, ./exec --op=v
                  (*pr_)[key] = new ParseItem(block.substr(pos_equal + 1));
                else
                  (*pr_)[key] = nullptr;
              } else {
                // a long format option but = is illegal
                // e.g., ./exec --o=[...]
                (*pr_)[block.substr(2)] = nullptr;
              }
            }
          } else if (block[2] == '=') {
            // a single option with =
            // e.g., ./exec -o=[...]
            string key;
            key.push_back(block[1]);
            if (block.size() > 3)
              (*pr_)[key] = new ParseItem(block.substr(3));
            else
              (*pr_)[key] = nullptr;
          } else {
            // a combination options
            // e.g., ./exec -ab[...]
            auto tbegin = block.begin() + 1;  // ignore the first '-'
            auto tend = block.end();
            auto t = tbegin;

            for (; t != tend; ++t) {
              string key;
              key.push_back(*t);
              (*pr_)[key] = nullptr;
            }
          }
        }
        break;
    }  // switch

    if (block[0] != '-' && previous != block  // not the first option
            ) {
      if (previous[0] != '-') {
        // previous is not an option, error occur
        // e.g., ./exec abc def
        throw ParseError("'" + block + "' is not allowed here");
      }

      string key;

      if (previous[0] == '-' && previous[1] == '-') {
        // previous is a long format option.
        // e.g., ./exec --option value
        key = previous.substr(2);
      } else {
        // it's the value of previous option.
        // e.g., ./exec -o   [...]
        // e.g., ./exec -opq [...]
        key.push_back(*(previous.end() - 1));
      }

      if (pr_->find(key) != pr_->end()) {
        (*pr_)[key] = new ParseItem(block);
      }
    }

    previous = block;
  }  // for

  if (subroutines_) {
    this->set_addition();
  }

  return pr_;
}

Parser::ParseResult* Parser::parse(const char* command_line) {
  int i = 0;
  string block;
  vector<string> blocks;
  char c;
  while ((c = command_line[i++]) != '\0') {
    if (c != ' ') {
      block.push_back(c);
    } else {
      if (!block.empty()) {
        blocks.push_back(block);
      }
      block.clear();
    }
  }
  if (!block.empty()) {
    blocks.push_back(block);
  }
  size_t size = blocks.size();  // argc
  char** argv = new char* [size];
  i = 0;
  std::for_each(blocks.begin(), blocks.end(), [argv, &i](const string& b) {
    argv[i++] = const_cast<char*>(b.c_str());
  });
  auto pr =
          this->parse(static_cast<const int>(size),
                      const_cast<const char**>(argv));

  delete[] argv;
  argv = nullptr;

  return pr;
}

bool Parser::has(const char* key) {
  string skey(key);

  if (pr_ && !pr_->empty() && !skey.empty()) {
    if (skey[0] == '-') {
      // check combination options, e.g., Parser::has("-xyz")
      for (size_t i = 1; i < skey.size(); ++i) {
        string tkey;
        tkey.push_back(skey[i]);
        if (pr_->find(tkey) == pr_->end()) {
          return false;
        }
      }
      return true;
    } else {
      // check single option, e.g., Parser::has("x")
      return pr_->find(skey) != pr_->end();
    }
  }
  return false;
}

bool Parser::has_or(std::initializer_list<const char*> options) {
  if (options.size() == 0) {
    return false;
  }
  for (auto key : options) {
    if (this->has(key)) return true;
  }
  return false;
}

bool Parser::has_and(std::initializer_list<const char*> options) {
  if (options.size() == 0) {
    return false;
  }
  for (auto key : options) {
    if (!this->has(key)) return false;
  }
  return true;
}

bool Parser::init(const int argc, const char** argv) {
  argc_ = argc;
  // argv_ = argv;
  // don't save it, point to a local var in parse(const char* command_line).
  // use member var args_ instead.
  if (argc > 0) {
    this->cleanup();

    args_.reserve(static_cast<size_t>(argc_));

    for (int i = 0; i < argc_; ++i) {
      args_.push_back(argv[i]);
    }

    pr_ = new Parser::ParseResult;
    return true;
  }
  return false;
}

void Parser::cleanup() {
  args_.clear();
  if (pr_) {
    auto ibegin = pr_->begin();
    auto iend = pr_->end();
    auto it = ibegin;
    for (; it != iend; ++it) {
      ParseItem* item = it->second;
      if (item) delete item;
    }
    delete pr_;
    pr_ = nullptr;
  }
}

void Parser::set_addition() {
  for (const Row& row : *(subroutines_->at(subroutine_name_))) {
    // assume both -o and --option are allowed,
    // but only provide -o,
    // then set the another --option.
    // vice versa.
    const string& def = row.value();
    const string& ops = row.oshort();
    const string& opl = row.olong();
    ParseResult& pr = *pr_;

    bool has_short = this->has(ops.c_str());
    bool has_long = this->has(opl.c_str());

    // assume -o [ --option ] arg = 1
    // but not provide option value,
    // then set to default 1.
    // otherwise, both set to user defined value

    if (!ops.empty()) {
      if (has_short) {
        if (pr[ops] != nullptr && !opl.empty()) {
          pr[opl] = new ParseItem(std::move(pr[ops]->val()));
        } else if (pr[ops] == nullptr && !def.empty()) {
          pr[ops] = new ParseItem(std::move(def));
          if (!opl.empty()) pr[opl] = new ParseItem(std::move(def));
        } else {
          pr[opl] = nullptr;
        }
      }
    }

    if (!opl.empty()) {
      if (has_long) {
        if (pr[opl] != nullptr && !ops.empty()) {
          pr[ops] = new ParseItem(std::move(pr[opl]->val()));
        } else if (pr[opl] == nullptr && !def.empty()) {
          if (!ops.empty()) pr[ops] = new ParseItem(std::move(def));
          pr[opl] = new ParseItem(std::move(def));
        } else {
          pr[ops] = nullptr;
        }
      }
    }

    if (!has_long && !has_short && !def.empty()) {
      if (!opl.empty()) pr[opl] = new ParseItem(std::move(def));
      if (!ops.empty()) pr[ops] = new ParseItem(std::move(def));
    }
  }  // for
}
}
