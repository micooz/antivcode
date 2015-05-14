#include "Bitmap.h"

Bitmap::Bitmap()
        : _pFile(nullptr), _data(nullptr), _width(0), _height(0), _depth(0) {

}

void
Bitmap::save(const std::string& file, byte* data,
             unsigned long width, unsigned long height,
             uint16_t depth) {
  if (_pFile) {
    fclose(_pFile);
  }
  _data = data;
  _height = height;
  _width = width;
  _depth = depth;

  _pFile = fopen(file.c_str(), "wb");
  write_bmp_header();
  write_bmp_data();
  fclose(_pFile);
}

void
Bitmap::write_bmp_header() {
  bmp_fileheader bfh;
  bmp_infoheader bih;

  unsigned long width;
  unsigned long height;
  unsigned short depth;
  unsigned long headersize;
  unsigned long filesize;

  width = _width;
  height = _height;
  depth = _depth;

  if (depth == 1) {
    headersize = 14 + 40 + 256 * 4;
    filesize = headersize + width * height;
  }

  if (depth == 3) {
    headersize = 14 + 40;
    filesize = headersize + width * height * depth;
  }

  memset(&bfh, 0, sizeof(bmp_fileheader));
  memset(&bih, 0, sizeof(bmp_infoheader));

  bfh.bfType = 0x4D42;
  bfh.bfSize = filesize;
  bfh.bfOffBits = headersize;

  bih.biSize = 40;
  bih.biWidth = width;
  bih.biHeight = height;
  bih.biPlanes = 1;
  bih.biBitCount = (unsigned short) depth * 8;
  bih.biSizeImage = width * height * depth;

  fwrite(&bfh, sizeof(struct bmp_fileheader), 1, _pFile);
  fwrite(&bih, sizeof(struct bmp_infoheader), 1, _pFile);

  if (depth == 1) {
    byte* platte;
    platte = new byte[256 * 4];
    byte j = 0;
    for (int i = 0; i < 1024; i += 4) {
      platte[i] = j;
      platte[i + 1] = j;
      platte[i + 2] = j;
      platte[i + 3] = 0;
      j++;
    }
    fwrite(platte, sizeof(byte) * 1024, 1, _pFile);
    delete[] platte;
  }
}

void
Bitmap::write_bmp_data() {
  byte* dst_width_buff;
  byte* point;

  unsigned long width;
  unsigned long height;
  unsigned short depth;

  width = _width;
  height = _height;
  depth = _depth;

  dst_width_buff = new byte[width * depth];
  memset(dst_width_buff, 0, sizeof(byte) * width * depth);

  point = _data + width * depth * (height - 1);
  for (unsigned long i = 0; i < height; i++) {
    for (unsigned long j = 0; j < width * depth; j += depth) {
      if (depth == 1) {
        dst_width_buff[j] = point[j];
      }
      if (depth == 3) {
        dst_width_buff[j + 2] = point[j + 0];
        dst_width_buff[j + 1] = point[j + 1];
        dst_width_buff[j + 0] = point[j + 2];
      }
    }
    point -= width * depth;
    fwrite(dst_width_buff, sizeof(byte) * width * depth, 1, _pFile);
  }
}

Bitmap::~Bitmap() {
  if (_pFile) {
    fclose(_pFile);
    _pFile = nullptr;
  }
}
