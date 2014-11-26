#ifndef _COLOR_H_
#define _COLOR_H_

#include "typedef.h"

class Color {
public:
    Color(byte r, byte g, byte b);

    //getter
    byte r()const;
    byte g()const;
    byte b()const;

    //setter
    void setRgb(byte r, byte g, byte b);

    bool operator > (const int &hex);

private:
    byte _r, _g, _b;
};

#endif //_COLOR_H_