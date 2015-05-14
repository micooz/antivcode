#include "Color.h"

Color::Color(byte r, byte g, byte b)
        :
        _r(r), _g(g), _b(b) {
}

byte Color::r() const {
  return _r;
}

byte Color::g() const {
  return _g;
}

byte Color::b() const {
  return _b;
}

void Color::setRgb(byte r, byte g, byte b) {
  _r = r;
  _g = g;
  _b = b;
}

bool Color::operator>(const int& hex) {
  return (_r << 16 | _g << 8 | _b) > hex;
}
