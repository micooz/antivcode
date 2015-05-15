//detect memory leak
#if defined (WIN32) || (_WIN32)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <iostream>
#include <fstream>
#include <generator/generator.h>
#include <parser/parser.h>
#include <boost/filesystem.hpp>
#include "IImage.h"
#include "Jpeg.h"
#include "Study.h"
#include "Decoder.h"

using namespace std;
using namespace program_options;

namespace fs = boost::filesystem;

std::ostream& operator<<(std::ostream& out, const CharSet& charset) {
  out << std::string(charset.begin(), charset.end());
  return out;
}

void study(const string& file, std::shared_ptr<Study> sd,
           Parser* parser) {
  char buf[10];
  memset(buf, 0, sizeof(buf));

  if (parser->has("byname")) {
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

  if (parser->has("savetmp")) {
    string tmpfile = file.substr(0, file.find_last_of('.')) + ".bmp";
    jpeg->saveTo(tmpfile);
  }

  auto psc = jpeg->cut();
  while (true) {
    if (psc->size() == std::strlen(buf))
      break;
    std::cout << "error characters[\"" << buf << "\"] length(right is " <<
    psc->size() << "), please try again(q to quit): " << endl;
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

void recognize(std::shared_ptr<Decoder> decoder, const string& file,
               Parser* parser,
               const std::function<void(const CharSet&)> callback) {
  //check the file type
  string extension = fs::extension(fs::path(file));
  if (extension != ".jpg" && extension != ".jpeg") {
    throw std::logic_error("invalid image format.");
  }
  //for test procedure
  bool testproc = parser->has("test");
  clock_t time_start = 0;
  if (testproc) {
    time_start = clock();
  }
  //start decoding
  CharSet charset = decoder->decode(file);

  callback(charset);
  //for test procedure
  if (testproc) {
    if (string(charset.begin(), charset.end()) ==
        fs::basename(fs::path(file))) {
      cout << " √ ";
    } else {
      cout << " × ";
    }
    clock_t now = clock();
    cout << "duration: " <<
    (double) (now - time_start) / (double) CLOCKS_PER_SEC * 1000 << " ms" <<
    endl;
  }
}

int main(int argc, const char** argv) {

#if defined (WIN32) || (_WIN32)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  program_options::Generator opts;
  opts.make_usage("antivcode usage:")
          ("h,help", "show help information")
          ("s,study", "auto create char map database, require -d or -f")
          ("y,byname", "study by filename without input manually")
          ("d,directory", "", "directory where vcode images saved in")
          ("f,file", "", "vcode image path")
          ("b,database", "./db", "database file path")
          ("t,test", "run test procedure, require -d or -f")
          ("n,newdb", "create new database")
          ("v,savetmp", "save binaryzated file, require -s");

  auto parser = opts.make_parser();
  parser->parse(argc, argv);

  try {
    //circulate just once, only one exit at return 0
    do {
      //show help
      if (parser->has("help")) {
        cout << opts << endl;
        break;
      }

      //study process
      if (parser->has("study")) {
        if (!parser->has_or({"directory", "file"})) {
          throw std::logic_error("-d or -f must be set.");
        }

        fs::path dbpath(parser->get("database")->val());

        if (parser->has("newdb")) {
          if (fs::exists(dbpath)) {
            fs::remove(dbpath);
          }
        }

        std::shared_ptr<Study> sd(new Study(dbpath.string()));

        if (parser->has("directory")) {
          fs::path dir(parser->get("directory")->val());
          if (!fs::exists(dir)) {
            fs::create_directory(dir);
          }
          fs::recursive_directory_iterator beg(dir);
          fs::recursive_directory_iterator end;
          for (; beg != end; beg++) {
            string extension = fs::extension(fs::path(*beg));
            if (extension == ".jpg" || extension == ".jpeg") {
              study(beg->path().string(), sd, parser);
            }
          }
        } else if (parser->has("file")) {
          study(parser->get("file")->val(), sd, parser);
        }

        sd->finish();

        cout << "study finished, database saved at " <<
        parser->get("database")->val() << endl;

        break;
      }

      //recognition process
      if (parser->has("file") || parser->has("directory")) {
        if (!fs::exists(fs::path(parser->get("database")->val()))) {
          throw std::logic_error("database not found.");
        }
        string db = parser->get("database")->val();
        std::shared_ptr<Decoder> decoder(new Decoder(db));

        if (parser->has("file")) {
          //for single file
          string file = parser->get("file")->val();
          recognize(decoder, file, parser, [&](const CharSet& charset) {
            cout << charset;
          });
          break;
        } else if (parser->has("directory")) {
          //for batch recognition
          string dir = parser->get("directory")->val();
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

              recognize(decoder, file, parser, [&](const CharSet& charset) {
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

      cout << opts << endl;

    } while (false);

  } catch (std::logic_error& e) {
    cout << e.what() << endl;
  }

  return 0;
}

