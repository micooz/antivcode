#include <vector>
#include "Decoder.h"
#include "IImage.h"
#include "Jpeg.h"
#include "Database.h"

std::ostream&
operator<<(std::ostream &out, CharSet &charset) {
    out << std::string(charset.begin(), charset.end());
    return out;
}

Decoder::Decoder(const std::string &dbpath)
    :_db(nullptr) {

    std::string root;
    std::string dbname;

    if (dbpath.find('/')) {
        int pos = dbpath.find_last_of('/');
        root = dbpath.substr(0, pos + 1);
        dbname = dbpath.substr(pos + 1);
    } else {
        root = "./";
    }

    _db = new Database(root, dbname);
}

CharSet
Decoder::decode(const std::string &file) {
    SliceCollection *col = _db->load();

    std::shared_ptr<IImage> target(new Jpeg(file));
    CharSet result;

    result.reserve(10);
    target->makeGray();
    target->binaryZate();

    auto tcol = target->cut();
    size_t len = tcol->size();

    //iterate target slices
    for (size_t i = 0; i < len; i++) {
        auto tsc = tcol->at(i);
        //iterate database
        bool found = false;

        std::shared_ptr<Slice> max;
        max = col->at(0);

        float lastr = 0.0f;
        float maxr = tsc->similarTo(*max);

        size_t dblen = col->size();

        for (size_t k = 1; k < dblen; ++k) {
            lastr = tsc->similarTo(*col->at(k));
            if (lastr > maxr) {
                maxr = lastr;
                max = col->at(k);
            }
        }

        result.push_back(max->getSymbol());
    }
    return result;
}

Decoder::~Decoder() {
    if (_db) {
        delete _db;
        _db = nullptr;
    }
}
