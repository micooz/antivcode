#Antivcode

An identification code validation procedures implement by C++ with boost and libjpeg.

Note: For *personal use* at present.

##Dependence
* boost v1_56_0 http://www.boost.org/users/history/version_1_56_0.html
* jpeglib v9(**included**) http://www.ijg.org/files/
* gcc 4.8.2 or above
* msvc compiler which support c++11
##Compile

####Linux

Suppose you have compiled boost library in **/usr/local/boost_1_56_0/** as well as the **.a** static library are in
**/usr/local/boost_1_56_0/stage/lib**.

If not, you should probably modify the **Makefile**.

When all of the work is finished,just do following steps:

```
>> cd ./libjpeg/
>> make
>> cd ../decode/
>> make
>> ./decode
```

####Windows
* Open **.sln** file.
* Configure include path of boost and libjpeg.
* Compile libjpeg project first in the solution.
* Configure attach library folder of boost and libjpeg.
* Compile decode project.
* F5 or Ctrl+F5 to run this application.

#Usage

First you should let the program study and generate attribute code database.

To get help information:

```
>> decode -h
Or
>> decode --help
```

To study by filenames from ./folder and create a new database named "db" in ./database:

```
>> decode -syn -d ./folder/ -b ./database/db
Or
>> decode -s -y -n -d ./folder/ -b ./database/db
```

To study sigle file and append to database by default(./db):

```
>> decode -s -f simples/studied.jpg
```

if you want to see binaryzated image, just add -v option.

When finish studying, you can run a test or recognition.

To test a single file:

```
>> decode -t -f simples/tested.jpg
```

To test a series of images:

```
>> decode -t -d test/
```
Note: test procedure require that file named with a right name.

To recognite a single file:

```
>> decode -f simples/recognited.jpg
```

To recognite a series of files:

```
>> decode -d simples/
```
#Author

Micooz
 
November 30, 2014 in Chengdu University of Technology.

Contact me by e-mail: **micooz@hotmail.com**