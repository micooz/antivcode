#ifndef _SLICE_H_
#define _SLICE_H_
#include <vector>
#include <iostream>
#include "typedef.h"

class IImage;

typedef std::vector<bool> AttributeCode;

class Slice {
public:
    Slice();
    Slice(IImage* base, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);//, bool copied = false);
    Slice(const Slice& slice);
    ~Slice();
    //getter
    uint32_t getX1()const;
    uint32_t getX2()const;
    uint32_t getY1()const;
    uint32_t getY2()const;
    uint32_t getWidth()const;
    uint32_t getHeight()const;
    char getSymbol()const;
    //setter
    void setCode(const AttributeCode &code);
    void setSymbol(char symbol);

    //[Bug]save the slice as bmp file
    void save(const std::string& file);

    //return an attribute code
    AttributeCode getCode()const;

    //compare similar rate with another slice, relative to parameter [slice]
    float similarTo(Slice &slice);

    //output AttributeCode as a string
    friend std::ostream& operator<<(std::ostream &out, Slice &slice);
private:
    AttributeCode _code;
    uint32_t _x1, _x2;
    uint32_t _y1, _y2;
    byte* _data;
    char _symbol;
};

#endif //_SLICE_H_
