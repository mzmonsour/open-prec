CXX=g++
RM=rm -f
FLAGS=-g -m32
APIDIR=deps/source-sdk-2013/mp/src

DEFINES=-DPOSIX=1 -DGNUC=1 -D_LINUX

CFLAGS=$(FLAGS) $(DEFINES)
CPPFLAGS=$(FLAGS) -std=c++11 $(DEFINES) -I$(APIDIR)/common -I$(APIDIR)/public -I$(APIDIR)/public/tier0 -I$(APIDIR)/public/tier1 -I$(APIDIR)/public/tier2
LDFLAGS=$(FLAGS) -shared -L$(APIDIR)/lib/public/linux32
LDLIBS=-lvstdlib -ltier0 -l:tier1.a

SRCS=src/main.cpp\
	src/cvar.cpp\
	src/sound.cpp\

OBJS=$(subst .cpp,.o,$(SRCS))
TARGET=open-prec.so

all: plugin

plugin: $(OBJS)
	g++ $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)
