#include <vector>
#include "Decoder.h"
#include "IImage.h"
#include "Jpeg.h"
#include "Database.h"

Decoder::Decoder(const std::string& dbpath)
        : _db(nullptr) {

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
Decoder::decode(const std::string& file) {
  SliceCollection* col = _db->load();

  if (col->empty()) {
    throw std::logic_error("database empty");
  }

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

    std::shared_ptr<Slice> max = col->at(0);

    float lastr = 0.0f;
    float maxr = 0.0f;// tsc->similarTo(*max);

    size_t dblen = col->size();
    //iterate database
    for (size_t k = 0; k < dblen; ++k) {
      lastr = tsc->similarTo(*col->at(k));
      if (lastr > maxr) {
        maxr = lastr;
        max = col->at(k);
      }
      //#ifdef _DEBUG
      //            std::cout << "compared with " << col->at(k)->getSymbol() << " in db, rate = " << lastr << std::endl;
      //#endif
    }
    //#ifdef _DEBUG
    //        std::cout << "\nhit at " << max->getSymbol() << " " << maxr << std::endl;
    //        std::cout << "tc    " << tsc->getCode() << std::endl;
    //        std::cout << "sc(" << max->getSymbol() << ") " << max->getCode() << std::endl;
    //        std::cout << std::endl;
    //#endif

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
