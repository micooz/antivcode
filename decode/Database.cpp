#include <iostream>
#include <fstream>
#include <memory>
#include "Database.h"

std::ofstream&
operator<<(std::ofstream &out, Slice *item) {
    AttributeCode &code = item->getCode();
    char symbol = item->getSymbol();
    size_t len = code.size();

    out.write((&symbol), sizeof(char));

    out.write((char*) &(len), sizeof(size_t));

    out.write(std::string(code.begin(), code.end()).c_str(), code.size());

    return out;
}

std::ifstream&
operator>>(std::ifstream &in, Slice *item) {
    char symbol;
    size_t len;

    if (!in.read(&(symbol), sizeof(char))) {
        return in;
    }

    in.read((char*) &len, sizeof(size_t));

    char *buffer = new char[len];

    in.read(buffer, len);

    item->setSymbol(symbol);
    item->setCode(AttributeCode(buffer, buffer + len));

    delete[] buffer;
    buffer = nullptr;

    return in;
}

Database::Database(const std::string &root, const std::string &dbname)
    :_collection(nullptr), _root(root), _dbname(dbname) {

    _collection = new SliceCollection;
}

void
Database::add(std::shared_ptr<Slice> slice) {
    _collection->push_back(slice);
}

SliceCollection*
Database::load() {
    if (_collection) {
        if (!_collection->empty())
            save();
    } else {
        _collection = new SliceCollection;
    }
    _collection->reserve(30);
    //
    std::string file = _root + _dbname;
    std::ifstream stream(file, std::ios::in | std::ios::binary);

    while (true) {
        std::shared_ptr<Slice> it(new Slice());

        if (!stream.eof() && stream >> it.get()) {
            _collection->push_back(it);
        } else {
            break;
        }
    }

    stream.close();
    return _collection;
}

void
Database::save() {
    auto file = _root + _dbname;
    std::ofstream stream(file, std::ios::binary | std::ios::out | std::ios::app);
    for (std::shared_ptr<Slice> slice : *_collection) {
        stream << slice.get();
    }
    stream.flush();
    stream.close();

    _collection->clear();
}

Database::~Database() {
    if (_collection) {
        delete _collection;
        _collection = nullptr;
    }
}
