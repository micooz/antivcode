//detect memory leak
#if defined (WIN32) || (_WIN32)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <iostream>
#include <memory>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "IImage.h"
#include "Jpeg.h"
#include "Study.h"
#include "Decoder.h"

using namespace std;
using namespace boost::program_options;

namespace fs = boost::filesystem;

std::ostream& operator<<(std::ostream &out, const CharSet &charset) {
    out << std::string(charset.begin(), charset.end());
    return out;
}

void study(const string& file, std::shared_ptr<Study> sd, const variables_map *vm) {
    char buf[10];
    memset(buf, 0, sizeof(buf));

    if (vm->count("byname")) {
        string fname = fs::basename(fs::path(file));
        strcpy(buf, fname.c_str());
    } else {
        cout << "please input the characters you saw: " << endl;

        string fullpath = fs::system_complete(fs::path(file)).string();
        string command = string("start ").append(fullpath);
        system(command.c_str());

        std::cin >> buf;
    }
    cout << "Studying... " << file << endl;

    std::shared_ptr<IImage> jpeg(new Jpeg(file));
    jpeg->makeGray();
    jpeg->binaryZate();

    if (vm->count("savetmp")) {
        string tmpfile = file.substr(0, file.find_last_of('.')) + ".bmp";
        jpeg->saveTo(tmpfile);
    }

    auto psc = jpeg->cut();
    while (true) {
        if (psc->size() == std::strlen(buf))
            break;
        std::cout << "error characters[\"" << buf << "\"] length(right is " << psc->size() << "), please try again(q to quit): " << endl;
        std::cin.clear();
        std::cin >> buf;
        if (buf[0] == 'q' && buf[1] == 0) {
            throw std::logic_error("user abort.");
        }
    }
    for (size_t i = 0; i < psc->size(); i++) {
        sd->confirm(psc->at(i), buf[i]);
    }
}

void recognite(std::shared_ptr<Decoder> decoder, const string &file,
               const variables_map *vm, const std::function<void(const CharSet &)> callback) {
    //check the file type
    string extension = fs::extension(fs::path(file));
    if (extension != ".jpg" && extension != ".jpeg") {
        throw std::logic_error("invalid image format.");
    }
    //for test procedure
    bool testproc = (vm->count("test")) ? true : false;
    clock_t time_start = 0;
    if (testproc) {
        time_start = clock();
    }
    //start decoding
    CharSet charset = decoder->decode(file);

    callback(charset);
    //for test procedure
    if (testproc) {
        if (string(charset.begin(), charset.end()) == fs::basename(fs::path(file))) {
            cout << " √ ";
        } else {
            cout << " × ";
        }
        clock_t now = clock();
        cout << "duration: " << (double)(now - time_start) / (double)CLOCKS_PER_SEC * 1000  << " ms" << endl;
    }
}

class examples {
public:
    static examples* getInstance() {
        static examples *_obj = nullptr;
        static gc _gc;

        if (!_obj) {
            _obj = new examples;
        }
        return _obj;
    }

    friend std::ostream& operator<<(std::ostream &out, const examples *obj) {
        for (auto line : obj->_text) {
            out << line << std::endl;
        }
        return out;
    }
private:
    examples() {
        _text.reserve(30);
        ifstream in(_file, ios::in | ios::binary);

        string buf;
        if (in) {
            while (!in.eof()) {
                std::getline(in, buf);
                _text.push_back(buf);
            }
            in.close();
        }
    }

    class gc {
    public:
        ~gc() {
            examples *pIns = examples::getInstance();
            if (pIns) {
                delete pIns;
                pIns = nullptr;
            }
        }
    };
    vector<string> _text;
    const string _file = "./examples";
};

int main(int argc, char **argv) {

#if defined (WIN32) || (_WIN32)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    boost::program_options::options_description opts("decode Usage");
    opts.add_options()
        ("help,h", "show help information")
        ("study,s", "auto create char map database, require -d or -f")
        ("byname,y", "study by filename without input manually")
        ("directory,d", value<string>(), "directory where vcode images saved in")
        ("file,f", value<string>(), "vcode image path")
        ("database,b", value<string>()->default_value("./db"), "database file path")
        ("test,t", "run test procedure, require -d or -f")
        ("newdb,n", "create new database")
        ("savetmp,v", "save binaryzated file, require -s");

    variables_map vm;
    try {
        store(parse_command_line(argc, argv, opts), vm);
    } catch (exception &ex) {
        cout << ex.what() << endl;
        return -1;
    }
    notify(vm);

    try {
        //circulate just once, only one exit at return 0
        do {
            //show help
            if (vm.count("help")) {
                cout << opts << examples::getInstance() << endl;
                break;
            }

            //study process
            if (vm.count("study")) {
                if (!(vm.count("directory") || vm.count("file"))) {
                    throw std::logic_error("-d or -f must be set.");
                }

                fs::path dbpath(vm["database"].as<string>());

                if (vm.count("newdb")) {
                    if (fs::exists(dbpath)) {
                        fs::remove(dbpath);
                    }
                }

                std::shared_ptr<Study> sd(new Study(dbpath.string()));

                if (vm.count("directory")) {
                    fs::path dir(vm["directory"].as<string>());
                    if (!fs::exists(dir)) {
                        fs::create_directory(dir);
                    }
                    fs::recursive_directory_iterator beg(dir);
                    fs::recursive_directory_iterator end;
                    for (; beg != end; beg++) {
                        string extension = fs::extension(fs::path(*beg));
                        if (extension == ".jpg" || extension == ".jpeg") {
                            study(beg->path().string(), sd, &vm);
                        }
                    }
                } else if (vm.count("file")) {
                    study(vm["file"].as<string>(), sd, &vm);
                }

                sd->finish();

                cout << "study finished, database saved at " << vm["database"].as<string>() << endl;

                break;
            }

            //recognition process
            if (vm.count("file") || vm.count("directory")) {
                if (!fs::exists(fs::path(vm["database"].as<string>()))) {
                    throw std::logic_error("database not found.");
                }
                string db = vm["database"].as<string>();
                std::shared_ptr<Decoder> decoder(new Decoder(db));

                if (vm.count("file")) {
                    //for single file
                    string file = vm["file"].as<string>();
                    recognite(decoder, file, &vm, [&] (const CharSet &charset) {
                        cout << file << " ----> " << charset;
                    });
                    break;
                } else if (vm.count("directory")) {
                    //for batch recognition
                    string dir = vm["directory"].as<string>();
                    if (!fs::exists(fs::path(dir))) {
                        throw std::logic_error("directory not found.");
                    }
                    //traversal the folder
                    int right_count = 0, sumfile = 0;
                    fs::recursive_directory_iterator beg(dir);
                    fs::recursive_directory_iterator end;
                    for (; beg != end; beg++) {
                        string extension = fs::extension(fs::path(*beg));
                        if (extension == ".jpg" || extension == ".jpeg") {

                            string file = beg->path().string();

                            recognite(decoder, file, &vm, [&] (const CharSet &charset) {
                                cout << file << " ----> " << charset << endl;
                                if (std::string(charset.begin(), charset.end())
                                    == fs::basename(fs::path(file))) {
                                    right_count++;
                                }
                                //cout << "\taverage: " << (now - time_start) / sumfile << " ms" << endl;
                            });

                            sumfile++;
                        }
                    }
                    cout << "\nright/sum: " << right_count << "/"
                        << sumfile << " = " << (double) right_count / sumfile << endl;

                    break;
                }//if
            }

            cout << opts << examples::getInstance() << endl;

        } while (false);

    } catch (std::logic_error& e) {
        cout << e.what() << endl;
    }

    return 0;
}

