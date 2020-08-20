ifndef  CC 
CC        = gcc
endif

ifndef  AR 
AR        = ar
endif

ifndef  CXX 
CXX       = g++
endif

ifndef  CPPFLAGS 
CPPFLAGS  = -D_GNU_SOURCE -D_REENTRANT=1 -D_FILE_OFFSET_BITS=64 -DNDEBUG
endif

ifndef  CXXFLAGS 
CXXFLAGS  =  $(CFLAGS)
endif

ifndef  CFLAGS 
CFLAGS    = -march=native -O2 -fPIC -fomit-frame-pointer -pipe -Wall -Wextra -Wno-unused-result -fno-strict-aliasing -ffunction-sections -fdata-sections
CFLAGS	 += $(shell pkg-config --cflags opencv4)
endif

ifndef  LDFLAGS 
LDFLAGS   = -march=native -s -Xlinker --gc-sections -Xlinker --as-needed
endif

ifndef  DIR 
DIR   = .
endif

ifndef INCLDIR
INCLDIR   = -I.
endif

ifndef STATICLIB_DIR
STATICLIB_DIR = $(DIR)/lib
endif

ifndef LIBDIR
LIBDIR    = -L.
LIBDIR    += -L$(STATICLIB_DIR)
endif

ifndef LIBS
LIBS      = -ldarknet
LIBS	 += $(shell pkg-config --libs opencv4)
endif


ifneq ($(STATICLIB_DIR), $(wildcard $(STATICLIB_DIR)))
$(shell mkdir -p $(STATICLIB_DIR))
endif

CUR_DIR= $(DIR)
CUR_OBJS=\
          $(patsubst %.c,%.o,$(wildcard *.c)) \
          $(patsubst %.cpp,%.o,$(wildcard *.cpp)) \
          $(patsubst %.cxx,%.o,$(wildcard *.cxx)) \
          $(patsubst %.cc,%.o,$(wildcard *.cc))


#搜索当前路径下是否还有其他文件夹
SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')

.SUFFIXES: .c .cpp .cxx .cc .o

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLDIR) $(LIBDIR) -c $(CUR_DIR)/$*.c

.cpp.o:
	$(CXX) -std=c++11  $(CPPFLAGS) $(CXXFLAGS) $(INCLDIR) $(LIBS) -c $(CUR_DIR)/$*.cpp

.cxx.o:
	$(CXX) -std=c++11  $(CPPFLAGS) $(CXXFLAGS) $(INCLDIR) $(LIBS) -c $*.cxx

.cc.o:
	$(CXX) -std=c++11 $(CPPFLAGS) $(CXXFLAGS) $(INCLDIR) $(LIBS) -c $*.cc

#生成目标文件，之后合成静态库至静态库路径
TARGET = main
all: $(CUR_OBJS)
	$(CXX) -std=c++11 -o $(TARGET) $(CUR_OBJS) $(INCLDIR) $(LIBS) # 生成可执行文件


ECHO:
	@echo $(SUBDIRS)

#清除目标文件和文件夹对应的静态库文件
.PHONY: clean
clean:	
	$(RM) $(CUR_OBJS)


