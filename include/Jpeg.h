#ifndef _JPEG_H_
#define _JPEG_H_

#include <iostream>
#include <string>
#include <jpeglib.h>
#include "IImage.h"

class Jpeg :
    public IImage {
public:
    Jpeg(const std::string &file);
    ~Jpeg();
    Color getPixel(const uint32_t x, const uint32_t y);
    void setPixel(const uint32_t x, const uint32_t y, const Color &color);
    void binaryZate();
    void makeGray();
    std::shared_ptr<SliceCollection> cut();
    AttributeCode getCode();
    void saveTo(const std::string& file);
    uint32_t getWidth()const;
    uint32_t getHeight()const;
private:
    FILE *_pf;
    jpeg_decompress_struct _info;
    jpeg_error_mgr _jerr;
    byte *_raw;
};

#endif //_JPEG_H_