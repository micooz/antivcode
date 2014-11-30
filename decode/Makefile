###vars definations
target = decode
#headers = *.h
cpps = *.cpp
objs = *.o
###path definations
boost_include_path = /usr/local/boost_1_56_0
boost_lib_path = /usr/local/boost_1_56_0/stage/lib

libjpeg_include_path = ../libjpeg
libjpeg_lib_path = ../libjpeg

VPATH = ./
###dependence libs
boost_perfix = boost_
libs =  -l$(boost_perfix)program_options \
                -l$(boost_perfix)system \
                -l$(boost_perfix)filesystem \
                -lpthread \
        -ljpeg

###target starts here
decode: $(objs)
        g++ $(objs) \
                -o $(target) \
                -L $(boost_lib_path) \
        -L $(libjpeg_lib_path) \
                $(libs) \
                -std=c++11 \
        -static
$(objs): $(cpps)
        g++ -I $(boost_include_path) -I $(libjpeg_include_path) -c $(cpps) -std=c++11

:PHONY: clean
clean:
        rm $(objs)
