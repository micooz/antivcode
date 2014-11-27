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

void study(const string& file, std::shared_ptr<Study> sd, const variables_map *vm) {
    char buf[10];
    memset(buf, 0, sizeof(buf));

    if (vm->count("byname")) {
        string fname = fs::basename(fs::path(file));

        strcpy_s(buf, sizeof(buf), fname.c_str());

    } else {
        cout << "please input the characters you saw: " << endl;

        string fullpath = fs::system_complete(fs::path(file));
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
        if (buf[0] == 'q' && buf[1] == 0)
            throw std::exception("user abort.");
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
        ("byname,y", "study by filename without input manually")
        ("directory,d", value<string>(), "directory where vcode images saved in")
        ("file,f", value<string>(), "vcode image path")
        ("database,b", value<string>()->default_value("./db"), "database file path")
        ("test,t", "run test procedure,require -d")
        ("newdb,n", "create new database")
        ("savetmp,v", "save binaryzated file");

    const char *examples = "\nexamples:\n\tdecode -syn -d ./folder/ -b ./database/db\n";

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
                cout << opts << examples << endl;
                break;
            }

            //study process
            if (vm.count("study")) {
                if (!(vm.count("directory") || vm.count("file"))) {
                    throw std::exception("-d or -f must be set.");
                }

                fs::path dbpath(vm["database"].as<string>());

                if (vm.count("newdb")) {
                    if (fs::exists(dbpath)) {
                        fs::remove(dbpath);
                    }
                }

                std::shared_ptr<Study> sd(new Study(dbpath));

                bool savetmp = (vm.count("savetmp")) ? true : false;

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
                    throw std::exception("database not found.");
                }
                string db = vm["database"].as<string>();
                std::shared_ptr<Decoder> decoder(new Decoder(db));

                if (vm.count("file")) {
                    //for single file
                    string file = vm["file"].as<string>();

                    string extension = fs::extension(fs::path(file));
                    if (extension != ".jpg" && extension != ".jpeg") {
                        throw std::exception("invalid image format.");
                    }

                    CharSet result = decoder->decode(file);
                    cout << result;

                    break;
                } else if (vm.count("directory")) {
                    //for batch recognition
                    string dir = vm["directory"].as<string>();
                    if (!fs::exists(fs::path(dir))) {
                        throw std::exception("directory not found.");
                    }
                    //whether run a test procedure
                    bool testproc = vm.count("test");
                    clock_t time_start = 0;
                    if (testproc) {
                        time_start = clock();
                    }
                    //traversal the folder
                    int right_count = 0, sumfile = 0;
                    fs::recursive_directory_iterator beg(dir);
                    fs::recursive_directory_iterator end;
                    for (; beg != end; beg++) {
                        string extension = fs::extension(fs::path(*beg));
                        if (extension == ".jpg" || extension == ".jpeg") {
                            string file = beg->path().string();

                            CharSet result = decoder->decode(file);

                            if (std::string(result.begin(), result.end())
                                == fs::basename(fs::path(file))) {
                                right_count++;
                            }
                            sumfile++;

                            cout << file << " ----> " << result << endl;
                        }
                    }
                    if (testproc) {
                        cout << "\nright/sum: " << right_count << "/"
                            << sumfile << " = " << (double) right_count / sumfile << endl;
                        clock_t now = clock();

                        cout << "duration: " << (now - time_start) << " ms\taverage: "
                            << (now - time_start) / sumfile << " ms" << endl;
                    }
                    break;
                }//if
            }

            cout << opts << examples << endl;

        } while (false);

    } catch (std::exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}
