#ifndef _IIMAGE_H_
#define _IIMAGE_H_

#include <memory>
#include <string>
#include <vector>
#include "Color.h"
#include "Slice.h"

typedef std::vector<std::shared_ptr<Slice> > SliceCollection;

class IImage {
public:
    IImage();
    virtual ~IImage();

    virtual Color getPixel(const uint32_t x, const uint32_t y) = 0;
    virtual void setPixel(const uint32_t x, const uint32_t y, const Color &color) = 0;
    virtual void binaryZate() = 0;
    virtual void makeGray() = 0;
    virtual std::shared_ptr<SliceCollection> cut() = 0;
    virtual void saveTo(const std::string& file) = 0;
    virtual uint32_t getWidth()const = 0;
    virtual uint32_t getHeight()const = 0;

    //getter
    bool* getLattice()const;
    byte* getData()const;

protected:
    byte *_data;
    bool *_lattice;
};

#endif //_IIMAGE_H_