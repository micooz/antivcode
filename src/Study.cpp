#include "Study.h"
#include "Database.h"

Study::Study(const std::string &dbpath)
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

void
Study::confirm(std::shared_ptr<Slice> slice, char symbol) {
    slice->setSymbol(symbol);
    _db->add(slice);
}

void
Study::finish() {
    _db->save();
}

Study::~Study() {
    if (_db) {
        delete _db;
        _db = nullptr;
    }
}
