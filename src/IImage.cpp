#include "IImage.h"


IImage::IImage()
        :
        _data(nullptr), _lattice(nullptr) {
}

bool*
IImage::getLattice() const {
  return _lattice;
}

byte*
IImage::getData() const {
  return _data;
}

IImage::~IImage() {
  if (_data != nullptr) {
    delete[] _data;
    _data = nullptr;
  }
  if (_lattice != nullptr) {
    delete[] _lattice;
    _lattice = nullptr;
  }
}
