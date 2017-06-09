LIBTARDIR=lib

BASEDIR=$(PWD)


INCLUDE= -Iinclude
LFLAGS= -shared -Wl,-soname,libXiDriver.so
CFLAGS= -pipe -g -Wall -W -fPIC
CXXFLAGS=-pipe -fPIC -std=c++11
LIBS    = -lxml2 -pthread -llog4cpp

TARGET0  =$(LIBTARDIR)/libXiDriver.so
SRCDIR  = src
OBJECTS = $(SRCDIR)/XiPictureDriver.o \
		  $(SRCDIR)/XiLog.o \
		  $(SRCDIR)/XiImageDevice.o \
		  $(SRCDIR)/XiList.o \
		  $(SRCDIR)/XiDriver.o \
		  $(SRCDIR)/XiXmlDriver.o \
		  $(SRCDIR)/XiXmlDevice.o \
		  $(SRCDIR)/libnetwork.o \
		  $(SRCDIR)/libdate.o

all: $(TARGET0) test install

$(TARGET0):$(OBJECTS)
	$(CXX) $(LFLAGS) $(CXXFLAGS) $(OBJECTS) -o $(TARGET0) $(LIBS)
	cp src/*.h include
	cp lib/* bin -rfa

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o "$@" "$<"

.c.o:
	$(CC)  -c -o "$@" "$<" 

.PHONY:test

testobj = Picture ImageDevice t_network t_date Driver XmlDevice memorytest Register

testobjtemp := $(testobj:%=bin/%)
test:$(testobjtemp)

TESTCXXFLAGS = -Iinclude -Llib -std=c++11 -Wl,--rpath=./ -lXiDriver

bin/Picture:test/Picture.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS) -lopencv_core -lopencv_highgui -ljrtp 

bin/ImageDevice:test/ImageDevice.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS)

bin/t_network:test/t_network.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS)

bin/t_date:test/t_date.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS)

bin/Driver:test/Driver.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS) -llog4cpp 

bin/XmlDevice:test/XmlDevice.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS) -lxml2

bin/memorytest:test/memorytest.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS) 

bin/Register:test/register.o
	$(CXX) $^ -o $@ 


install:
	cp bin/* /tftpboot/nfsroot/home/root/bin/  -rf
	cp lib/* /tftpboot/nfsroot/home/root/lib/ -rf
	cp config/* /tftpboot/nfsroot/home/root/config/ -rf


clean:
	rm $(testobj:%=test/%.o) -rf
	rm $(testobjtemp) -rf
	rm $(OBJECTS) -rf
	rm $(TARGET0) -rf
	rm include/* -rf
bsp:
	tar zcvf libdriver.tar.bz include lib doc config
