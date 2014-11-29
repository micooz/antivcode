#include <iostream>
#include <memory>
#include "Jpeg.h"
#include "Bitmap.h"

Jpeg::Jpeg(const std::string &file)
    :_pf(nullptr) {

    _info.err = jpeg_std_error(&_jerr);
    jpeg_create_decompress(&_info);

    _pf = fopen(file.c_str(), "rb");
    if (_pf == nullptr) {
        throw std::logic_error("file not found.");
    }
    jpeg_stdio_src(&_info, _pf);
    jpeg_read_header(&_info, true);

    _data = new byte[_info.image_width * _info.image_height * _info.num_components];

    jpeg_start_decompress(&_info);
    byte *pLine;
    int i = 0;
    while (_info.output_scanline < _info.image_height) {
        pLine = _data + i++ * _info.image_width * _info.num_components;
        jpeg_read_scanlines(&_info, &pLine, 1);
    }
    jpeg_finish_decompress(&_info);
}

inline Color
Jpeg::getPixel(const uint32_t x, const uint32_t y) {
    byte *pos;
    pos = _data + (y * _info.image_width + x) * _info.num_components;
    Color color(pos[0], pos[1], pos[2]);
    return color;
}

inline void
Jpeg::setPixel(const uint32_t x, const uint32_t y, const Color &color) {
    byte *pos;
    pos = _data + (y * _info.image_width + x) * _info.num_components;

    pos[0] = color.r();
    pos[1] = color.g();
    pos[2] = color.b();
}

void
Jpeg::binaryZate() {
    if (!_lattice) {
        _lattice = new bool[_info.image_width * _info.image_height];
    }

    for (size_t x = 0; x < _info.image_width; ++x) {
        for (size_t y = 0; y < _info.image_height; ++y) {
            Color color(getPixel(x, y));
            if (color > 0xb5b5b5) {
                color.setRgb(0xff, 0xff, 0xff);
                setPixel(x, y, color);
                _lattice[y * _info.image_width + x] = false;
            } else {
                color.setRgb(0x0, 0x0, 0x0);
                setPixel(x, y, color);
                _lattice[y * _info.image_width + x] = true;
            }
        }
    }
}

void
Jpeg::makeGray() {
    const float wr = 0.3f, wg = 0.59f, wb = 0.11f;

    for (size_t x = 0; x < _info.image_width; ++x) {
        for (size_t y = 0; y < _info.image_height; ++y) {
            Color color(getPixel(x, y));
            byte rgb = color.r() *wr + color.g() * wg + color.b() *wb;
            color.setRgb(rgb, rgb, rgb);
            setPixel(x, y, color);
        }
    }
}

std::shared_ptr<SliceCollection>
Jpeg::cut() {
    if (!_lattice) {
        binaryZate();
    }
    std::vector<bool> xline;//integral line
    size_t width = _info.image_width;
    size_t height = _info.image_height;

    xline.reserve(width);
    xline.assign(width, false);
    //projection
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            if (_lattice[y*width + x]) {
                xline[x] = true;
            }
        }
    }
    //get x1 x2
    std::shared_ptr<SliceCollection> coll(new SliceCollection);
    coll->reserve(5);//necessary

    uint32_t x1, y1, x2, y2;
    bool _insec = false;
    auto it = xline.begin();

    for (int x = 0; it != xline.end(); ++it, ++x) {
        if (*it && _insec == false) {
            _insec = true;
            x1 = x;
            continue;
        }
        if (*it == false && _insec) {
            _insec = false;
            x2 = x - 1;
            //get y1 y2
            bool first = true;
            for (size_t y = 0; y < height; ++y) {
                for (size_t xx = x1; xx < x2; ++xx) {
                    bool v = _lattice[y*width + xx];
                    if (first && v) {
                        y1 = y;
                        first = false;
                        ++y;
                        continue;
                    }
                    if (v) {
                        y2 = y;
                        ++y;
                    }
                }
            }
            std::shared_ptr<Slice> pSlice(new Slice(this, x1, y1, x2, y2));
            coll->push_back(pSlice);
        }
    }

    return coll;
}

void
Jpeg::saveTo(const std::string& file) {
    Bitmap *bmp = new Bitmap();
    bmp->save(file, _data, _info.output_width, _info.output_height, _info.output_components);
    delete bmp;
}

uint32_t
Jpeg::getWidth()const {
    return _info.image_width;
}

uint32_t
Jpeg::getHeight()const {
    return _info.image_height;
}

Jpeg::~Jpeg() {
    jpeg_destroy_decompress(&_info);
    fclose(_pf);
}
