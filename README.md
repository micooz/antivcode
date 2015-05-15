# Antivcode

An identification code validation procedures implement by C++ with boost and libjpeg.

Note: For **personal use** at present.

## Dependence

* jpeglib v9(**included** in the project) http://www.ijg.org/files/

## Compile

CMake building system makes it very easy to compile and run, The simplest way is use the default option of cmake: 

```bash
cmake ./
make
./antivcode -h
```

# Usage

First you should let the program study and generate attribute code database.

To get help information:

```
>> antivcode -h
Or
>> antivcode --help
```

To study by filenames from ./folder and create a new database named "db" in ./database:

```
>> antivcode -syn -d ./folder/ -b ./database/db
Or
>> antivcode -s -y -n -d ./folder/ -b ./database/db
```

To study sigle file and append to database by default(./db):

```
>> antivcode -s -f simples/studied.jpg
```

if you want to see binaryzated image, just add -v option.

When finish studying, you can run a test or recognition.

To test a single file:

```
>> antivcode -t -f simples/tested.jpg
```

To test a series of images:

```
>> antivcode -t -d test/
```
Note: test procedure require that file named with a right name.

To recognite a single file:

```
>> antivcode -f simples/recognited.jpg
```

To recognite a series of files:

```
>> antivcode -d simples/
```

# Author

Micooz
 
November 30, 2014 in Chengdu University of Technology.

Last Update: May 15, 2015.

Contact me by e-mail: **micooz@hotmail.com**