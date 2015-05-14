#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <string>
#include <memory>
#include "IImage.h"
#include "Slice.h"

//provide attribute code storage algorithm.
class Database {
 public:
  Database(const std::string& root, const std::string& dbname);

  ~Database();

  void add(std::shared_ptr<Slice> slice);

  void save();

  SliceCollection* load();

 private:
  SliceCollection* _collection;
  std::string _dbname;
  std::string _root;
};

#endif //_DATABASE_H_