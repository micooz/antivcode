//detect memory leak
#if defined (WIN32) || (_WIN32)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <iostream>
#include <filesystem>
#include <memory>
#include <boost/program_options.hpp>
#include "IImage.h"
#include "Jpeg.h"
#include "Study.h"
#include "Decoder.h"

using namespace std;
using namespace boost::program_options;

namespace fs = std::tr2::sys;

void study(const string& file, std::shared_ptr<Study> sd) {
    char buf[10];
    memset(buf, 0, sizeof(buf));
    cout << "please input the characters you saw: " << endl;

    string fullpath = fs::system_complete(fs::path(file));
    string command = string("start ").append(fullpath);

    system(command.c_str());
    std::cin >> buf;
    cout << "Studying... " << file << endl;

    std::shared_ptr<IImage> jpeg(new Jpeg(file));
    jpeg->makeGray();
    jpeg->binaryZate();
    auto psc = jpeg->cut();
    while (true) {
        if (psc->size() == std::strlen(buf))
            break;
        std::cout << "error characters length(right is " << psc->size() << "), please try again(q to quit): " << endl;
        std::cin.clear();
        std::cin >> buf;
        if (buf[0] == 'q' && buf[1] == 0)
            exit(0);
    }
    for (size_t i = 0; i < psc->size(); i++) {
        sd->confirm(psc->at(i), buf[i]);
    }
}

int main(int argc, char **argv) {

#if defined (WIN32) || (_WIN32)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    boost::program_options::options_description opts("decode Usage");
    opts.add_options()
        ("help,h", "show help information")
        ("study,s", "auto create char map database,require -d or -f")
        ("directory,d", value<string>(), "directory where vcode images saved in")
        ("file,f", value<string>(), "vcode image path")
        ("database,b", value<string>()->default_value("./db"), "database file path")
        ("newdb,n", "create new database");

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
                cout << opts << endl;
                break;
            }

            //study process
            if (vm.count("study")) {
                if (!(vm.count("directory") || vm.count("file"))) {
                    throw "-d or -f must be set.";
                }

                fs::path dbpath(vm["database"].as<string>());

                if (vm.count("newdb")) {
                    if (fs::exists(dbpath))
                        fs::remove(dbpath);
                }

                std::shared_ptr<Study> sd(new Study(dbpath));

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
                            study(beg->path().string(), sd);
                        }
                    }
                } else if (vm.count("file")) {
                    study(vm["file"].as<string>(), sd);
                }

                sd->finish();

                cout << "study finished, database saved at " << vm["database"].as<string>() << endl;

                break;
            }

            //recognition process
            if (vm.count("file")) {
                if (!fs::exists(fs::path(vm["database"].as<string>()))) {
                    throw "database not found.";
                }
                string file = vm["file"].as<string>();
                string db = vm["database"].as<string>();
                std::shared_ptr<Decoder> decoder(new Decoder(db));

                CharSet result = decoder->decode(file);
                cout << result << endl;
                break;
            }

            cout << opts << endl;

        } while (false);

    } catch (std::exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}
