#ifndef _DECODER_H_
#define _DECODER_H_

#include <string>

class Database;

typedef std::vector<char> CharSet;

class Decoder {
public:
    Decoder(const std::string &db);
    ~Decoder();

    CharSet decode(const std::string &file);

    friend std::ostream& operator<<(std::ostream &out, const CharSet &charset);
private:
    Database *_db;
};

#endif //_DECODER_H_