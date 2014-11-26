#pragma once
#ifndef _STUDY_H_
#define _STUDY_H_

#include <string>
#include <memory>

class Slice;
class Database;

//to auto generate attribute code & save into file.
class Study {
public:
    Study(const std::string &dbpath);
    ~Study();

    void confirm(std::shared_ptr<Slice> slice, char symbol);
    void finish();
private:
    Database *_db;
};

#endif //_STUDY_H_
