#include "Slice.h"
#include "IImage.h"
#include "Bitmap.h"

Slice::Slice()
        : _x1(0), _x2(0), _y1(0), _y2(0), _data(nullptr), _symbol(0) {
}

Slice::Slice(IImage* base, uint32_t x1, uint32_t y1, uint32_t x2,
             uint32_t y2)//,bool copied)
        : _x1(x1), _x2(x2), _y1(y1), _y2(y2), _data(nullptr), _symbol(0) {

  uint32_t width = base->getWidth();
  bool* pLattice = base->getLattice();
  byte* pData = base->getData();
  size_t pixels = (_x2 - _x1) * (_y2 - _y1);

  _code.reserve(pixels);

  //if (copied) {
  _data = new byte[pixels * 3];
  //}
  byte* pDst = _data;
  for (size_t y = _y1; y < _y2; ++y) {
    for (size_t x = _x1; x < _x2; ++x) {
      _code.push_back(pLattice[y * width + x]);
      //copied data
      //if (copied) {
      byte* pSrc = pData + (y * width + x) * 3;

      pDst[0] = pSrc[0];
      pDst[1] = pSrc[1];
      pDst[2] = pSrc[2];

      pDst += 3;
      //}
    }
  }
}

Slice::Slice(const Slice& slice) {
  _code = slice._code;
  _symbol = slice._symbol;
  _x1 = slice._x1;
  _x2 = slice._x2;
  _y1 = slice._y1;
  _y2 = slice._y2;

  size_t pixels = (_x2 - _x1) * (_y2 - _y1);

  _data = new byte[pixels * 3];
  //make deep copy
  memcpy(_data, slice._data, pixels * 3);
}

uint32_t Slice::getX1() const {
  return _x1;
}

uint32_t Slice::getX2() const {
  return _x2;
}

uint32_t Slice::getY1() const {
  return _y1;
}

uint32_t Slice::getY2() const {
  return _y2;
}

char Slice::getSymbol() const {
  return _symbol;
}

uint32_t Slice::getWidth() const {
  return _x2 - _x1;
}

uint32_t Slice::getHeight() const {
  return _y2 - _y1;
}

AttributeCode Slice::getCode() const {
  return _code;
}

void Slice::setCode(const AttributeCode& code) {
  _code = code;
}

void Slice::setSymbol(char symbol) {
  _symbol = symbol;
}

float Slice::similarTo(Slice& slice) {
  AttributeCode sc = slice.getCode();
  AttributeCode tc = this->getCode();
  size_t slen = sc.size();
  size_t tlen = tc.size();
  int eq_true = 0;
  size_t total = slen;

  for (size_t i = 0; i < total; ++i) {
    //to prevent cross-border
    if (i >= tlen)
      break;
    if (sc.at(i)) {
      if (sc.at(i) == tc.at(i))
        ++eq_true;
    }
  }
  return (float) eq_true /
         ((slen < tlen) ? tlen : slen);// std::fmin(slen, tlen);
}

void Slice::save(const std::string& file) {
  if (_data) {
    Bitmap bmp;
    bmp.save(file, _data, getWidth(), getHeight(), 3);
  }
}

Slice::~Slice() {
  if (_data) {
    delete[] _data;
    _data = nullptr;
  }
  //std::cout << "Slice Destroy." << std::endl;
}

std::ostream& operator<<(std::ostream& out, const AttributeCode& code) {
  for (auto c : code)
    out << c;
  out << std::endl;
  return out;
}
