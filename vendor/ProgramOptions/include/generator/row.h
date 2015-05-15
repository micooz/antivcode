/*
 * row
 * (c) Copyright 2015 Micooz
 *
 * Released under the Apache License.
 * See the LICENSE file or
 * http://www.apache.org/licenses/LICENSE-2.0.txt for more information.
 */
#ifndef GENERATOR_ROW_H_
#define GENERATOR_ROW_H_

#include <string>

namespace program_options {
class Row {
 public:
  Row();

  enum Field { kShort, kLong, kDefault, kDescription };
  
  typedef std::initializer_list<Field> Order;

  // getter

  inline std::string oshort() const { return option_short; }

  inline std::string olong() const { return option_long; }

  inline std::string value() const { return default_value; }

  inline std::string desc() const { return description; }

  inline bool required() const { return require_value; }

  // setter

  inline void oshort(const std::string& oshort) { option_short = oshort; }

  inline void olong(const std::string& olong) { option_long = olong; }

  inline void value(const std::string& value) { default_value = value; }

  inline void desc(const std::string& desc) { description = desc; }

  inline void required(bool required) { require_value = required; }

 private:
  bool require_value;
  std::string option_short;
  std::string option_long;
  std::string default_value;
  std::string description;
};
}

#endif  // GENERATOR_ROW_H_