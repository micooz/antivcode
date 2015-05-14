#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <string>
#include "typedef.h"

#pragma pack(2)
struct bmp_fileheader {
  unsigned short bfType;
  unsigned long bfSize;
  unsigned short bfReverved1;
  unsigned short bfReverved2;
  unsigned long bfOffBits;
};
#pragma pack()

struct bmp_infoheader {
  unsigned long biSize;
  unsigned long biWidth;
  unsigned long biHeight;
  unsigned short biPlanes;
  unsigned short biBitCount;
  unsigned long biCompression;
  unsigned long biSizeImage;
  unsigned long biXPelsPerMeter;
  unsigned long biYpelsPerMeter;
  unsigned long biClrUsed;
  unsigned long biClrImportant;
};

class Bitmap {
 public:
  Bitmap();

  ~Bitmap();

  void save(const std::string& file, byte* data,
            unsigned long width, unsigned long height, uint16_t depth);

 private:
  void write_bmp_header();

  void write_bmp_data();

  FILE* _pFile;
  byte* _data;
  unsigned long _width;
  unsigned long _height;
  uint16_t _depth;
};

#endif // _BITMAP_H_