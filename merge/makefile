CXX = g++
CC  = gcc
INCLUDES	 =-Iinclude

CXXFLAGS = -g -Wall $(INCLUDES) -static-libstdc++ -static-libgcc -finput-charset=UTF-8 -fextended-identifiers -DDISABLE_VTLZMA -DDISABLE_VTLOG -DDISABLE_VTBZ2 -DDISABLE_VTDIFF02 -DNO_MMAP -DUNUSE_MALLOC -L./ -lvtdiff -lvtsys


ifeq ($(BUILD),debug)   
# "Debug" build - no optimization, and debugging symbols
CXXFLAGS += -O0 -g
else
# "Release" build - optimization, and no debug symbols
CXXFLAGS += -O2 -s -DNDEBUG
endif


ifeq ($(OS),Windows_NT)
	CXXFLAGS += -D_WIN32 -DDISABLE_MYSQL_DTC=1
	OSTYPE = WIN32
	INCLUDES +=-I"lib/mysql/win/x86/include/jdbc" -I"c:/boost/include/boost-1_70"
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        OSARCH=AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            OSARCH =AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            OSARCH =IA32
        endif
    endif
else

	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSTYPE = LINUX
		# CXXFLAGS += -DARMKEIL_ENV
	endif
	ifeq ($(UNAME_S),Darwin)
		OSTYPE = OSX
	endif
		UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		OSARCH =AMD64
	endif
		ifneq ($(filter %86,$(UNAME_P)),)
	OSARCH =IA32
		endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		OSARCH =ARM
	endif
endif



default:vtmerge
diff:vtdiff

ifeq ($(OSTYPE),LINUX)
        BUILDDIR=build/linux
        RM=rm -f
else
		
        BUILDDIR=build/win
		RM = del -Force
		CP = copy /Y
endif

CXXFLAGS +=-D$(OSARCH) -D$(OSTYPE) 



# $(BUILDDIR)/iostream.o:lib/nowide-standalone/src/iostream.cpp
# 	@echo $(BUILDDIR)/$@ $<
# 	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/%.o: src/%.cpp $(DEPS)
	@echo $@ $<
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/%.o: src/%.c $(DEPS)
	@echo $@ $<
	$(CC) -c -o $@ $< $(CXXFLAGS)


$(BUILDDIR)/%.o: src/ioctrl/%.cpp $(DEPS)
	@echo $@ $<
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/%.o: src/ioctrl/%.c $(DEPS)
	@echo $@ $<
	$(CC) -c -o $@ $< $(CXXFLAGS)

_OBJ =IOFile.o vt_merge.o vt_merge01.o vt_merge02.o vt_merge01util.o  vtdiffcodec.o main.o

OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

debug:
	make "BUILD=debug"

vtmerge: $(OBJ) 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCLUDES)


lib: $(OBJ)
	@echo $@ $<
	ar -r vtmerge.a $^ $(CXXFLAGS) $(INCLUDES)

clean:
	$(RM) $(BUILDDIR)/*.o
