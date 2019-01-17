#
# Date: $Format: %ad$
# Author: $Format: %an <%ae>$
# Version: $Format: %t$
# make file for AIOUSB library
#

override CEXTRAOBJS	+= 
override CORE_COBJS += \
cJSON.o \
AIOUSB_ADC.o \
AIOUSB_Core.o \
AIOUSB_CTR.o \
AIOUSB_CustomEEPROM.o \
AIOUSB_DAC.o \
AIOUSB_Log.o \
AIOUSB_DIO.o \
AIOUSB_WDG.o \
AIOUSB_Properties.o \
AIOBuf.o \
AIOCmd.o \
AIOContinuousBuffer.o \
AIOChannelMask.o \
AIODeviceInfo.o \
AIODeviceQuery.o \
AIODeviceTable.o \
AIOEither.o \
AIOUSBDevice.o \
DIOBuf.o \
ADCConfigBlock.o \
AIOCommandLine.o \
AIOConfiguration.o \
AIOChannelRange.o \
AIOCountsConverter.o \
AIOFifo.o\
AIOList.o\
AIOProductTypes.o\
AIOPlugNPlay.o\
AIOTuple.o\
CStringArray.o\
USBDevice.o


HEADERS		:= $(wildcard *.h) AIOUSB_Version.h
TESTOBJS	:= $(wildcard $(PWD)/tests/*_test)
TEST_EXTRA_CXXFLAGS	:= -std=gnu++11

TESTFILES	:= $(shell grep  -lP "\bmain\b" *.c)
CPPTEST_FILES 	:= $(patsubst %.c,%_cpp_test,$(TESTFILES))


include $(AIOUSB_ROOT)/Mkfiles/oses.inc

ifeq ("$(OSTYPE)","Darwin") 
CC_SHFLAGS := -dynamiclib
else 
CC_SHFLAGS := -shared -Wl,-soname
endif

CORE_CDBGOBJS 		+= $(CORE_COBJS:.o=.dbg.o)
CORE_CPPOBJS 		+= $(CORE_COBJS:.o=.cpp.o)
CORE_CPPDBGOBJS 	+= $(CORE_COBJS:.o=.cpp.dbg.o)

override COBJS		+= $(CORE_COBJS) $(CEXTRAOBJS)
override CDBGOBJS 	+= $(CORE_COBJS:.o=.dbg.o) $(CEXTRAOBJS)
override CPPEXTRAOBJS   = $(CEXTRAOBJS:.o=.cpp.o)
override CPPOBJS 	+= $(CORE_COBJS:.o=.cpp.o) $(CPPEXTRAOBJS)
override CPPDBGOBJS 	+= $(CORE_COBJS:.o=.cpp.dbg.o) $(CPPEXTRAOBJS)


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Library names
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

SHCDBGLIB 	:= $(LIBPREFIX)aiousbdbg.$(LIBSUFFIX)
SHCPPLIB	:= $(LIBPREFIX)aiousbcpp.$(LIBSUFFIX)
SHCPPDBGLIB 	:= $(LIBPREFIX)aiousbcppdbg.$(LIBSUFFIX)
SHCLIB		:= $(LIBPREFIX)aiousb.$(LIBSUFFIX)

CLIB 		:= $(LIBPREFIX)aiousb.$(STATICSUFFIX)
CDBGLIB 	:= $(LIBPREFIX)aiousbdbg.$(STATICSUFFIX)
CPPLIB 		:= $(LIBPREFIX)aiousbcpp.$(STATICSUFFIX)
CPPDBGLIB 	:= $(LIBPREFIX)aiousbcppdbg.$(STATICSUFFIX)


ifdef DEBUG
	DEBUGOPTS	+= -g
else
        DEBUGOPTS       := -DNDEBUG
endif

DEBUG_FLAGS	:= -g

LIBS 		:= $(CLIB) $(CDBGLIB) $(CPPLIB) $(CPPDBGLIB)  $(SHCLIB) $(SHCDBGLIB) $(SHCPPLIB) $(SHCPPDBGLIB)
override CFLAGS	+= -I. -I/usr/include/libusb-1.0 -std=gnu99
TESTFLAGS	:= -g
override SHARED_LIBS	+=  -lusb-1.0 -pthread -lm
override CXXFLAGS += -I. -I/usr/include/libusb-1.0 -D__aiousb_cplusplus

ifeq ("$(OSTYPE)","CYGWIN")
override CFLAGS		+= -Wno-char-subscripts 
override CXXFLAGS	+= -Wno-char-subscripts 
endif

REFDIR=$(shell readlink -f $$(pwd)/../../)
DIR=$(shell basename $(REFDIR))
RELEASENAME=$(patsubst AIOUSB-%,%,$(DIR))
CURDATE=$(shell git log -n 1 --format="%ad" 2>/dev/null || date )

.PHONY : all bist

all : $(LIBS) AIOUSB_Version.h

AIOUSB_Version.h: AIOUSB_Version.h.in
	@echo "Geting versioning"
	@tmp=`git describe --tags --always 2>/dev/null || echo $(RELEASENAME)  ` && \
	perl -ne "s/\@ACCESIO_TAG_VERSION\@/$$tmp/; s/\@ACCESIO_DATE\@/$(CURDATE)/g;print;" $< > $@

testit:
	@echo BAR
	tmp=`git describe --tags --always 2>/dev/null || echo $(RELEASENAME)  ` && echo $$tmp

bist:
	@echo $(CPPDBGOBJS)
	@echo $(OSTYPE)
	@echo $(CC_SHFLAGS)
	@echo $(CYGWINTYPE)
	@echo $(SHCPPLIB)
	@echo "CFLAGS=$(CFLAGS)"
	@echo "COBJS=$(COBJS) $(CEXTRAOBJS)"
	@echo "CDBGOBJS=$(CDBGOBJS)"
	@echo "CPPOBJS=$(CPPOBJS)"
	@echo "CPPDBGOBJS=$(CPPDBGOBJS)"
	@echo "CPPEXTRAOBJS=$(CPPEXTRAOBJS)"
	@echo "Headers=$(HEADERS)"
	@echo "tests: $(TESTOBJS)"
	@echo "CPPTESTFILES: $(CPPTEST_FILES)"

GTAGS_FILES	:= GPATH GRTAGS GSYMS GTAGS

all_cscope: $(LIBS) cscope

test:	$(CPPTEST_FILES) 
	for i in $(CPPTEST_FILES); \
		do ./$$i 2>/dev/null >/dev/null; \
		if [ "$$?" != "0" ] ; then echo "$$i had an error"; exit 1; fi \
	done




#*******************************  LIBRARIES  ********************************

$(CLIB) : $(COBJS)
	$(AR) cr $(CLIB) $^
$(CDBGLIB) : $(CDBGOBJS)
	$(AR) cr $(CDBGLIB) $^
$(CPPLIB) : $(CPPOBJS)
	$(AR) cr $(CPPLIB) $^
$(CPPDBGLIB) : $(CPPDBGOBJS)
	$(AR) cr $(CPPDBGLIB) $^

ifeq ("$(OSTYPE)","Darwin")
$(SHCLIB) : $(COBJS) $(CEXTRAOBJS)
	$(CC)  $(LDFLAGS) $(CC_SHFLAGS) -o $@ $^ $(SHARED_LIBS)
$(SHCDBGLIB): 	$(CDBGOBJS)
	$(CC) $(CC_SHFLAGS) $(LDFLAGS) -o $@ $^ $(SHARED_LIBS) 
$(SHCPPLIB) : $(CPPOBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(CC_SHFLAGS) -o $@ $^ $(SHARED_LIBS)
$(SHCPPDBGLIB) : $(CPPDBGOBJS)
	$(CXX) $(CXXFLAGS)  $(LDFLAGS) $(CC_SHFLAGS) -o $@ $^ $(SHARED_LIBS)
else ifeq ("$(OSTYPE)","CYGWIN")

$(SHCLIB) : $(COBJS) $(CEXTRAOBJS)
	$(CC) $(LDFLAGS) $(CC_SHFLAGS) -shared -o $@  -Wl,--out-implib=$(subst cyg,lib,$@).a -Wl,--export-all-symbols -Wl,--enable-auto-import  -Wl,--whole-archive $^ -Wl,--no-whole-archive $(SHARED_LIBS)


$(SHCDBGLIB): 	$(CDBGOBJS)
	$(CC) $(CC_SHFLAGS) $(LDFLAGS) -shared -o $@  -Wl,--out-implib=$(subst cyg,lib,$@).a -Wl,--export-all-symbols -Wl,--enable-auto-import  -Wl,--whole-archive $^ -Wl,--no-whole-archive $(SHARED_LIBS)

$(SHCPPLIB) : $(CPPOBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(CC_SHFLAGS) -shared -o $@  -Wl,--out-implib=$(subst cyg,lib,$@).a -Wl,--export-all-symbols -Wl,--enable-auto-import  -Wl,--whole-archive $^ -Wl,--no-whole-archive $(SHARED_LIBS)

$(SHCPPDBGLIB) : $(CPPDBGOBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(CC_SHFLAGS) -shared -o $@  -Wl,--out-implib=$(subst cyg,lib,$@).a -Wl,--export-all-symbols -Wl,--enable-auto-import  -Wl,--whole-archive $(CPPOBJS) -Wl,--no-whole-archive $(SHARED_LIBS)

else 
$(SHCLIB) : $(COBJS) $(CEXTRAOBJS)
	$(CC)  $(CFLAGS) $(LDFLAGS) $(CC_SHFLAGS),$@ -o $@ $^ $(SHARED_LIBS)
$(SHCDBGLIB):  $(CDBGOBJS)
	$(CC)  $(CFLAGS) $(LDFLAGS) $(CC_SHFLAGS),$@ -o $@ $^ $(SHARED_LIBS) 
$(SHCPPLIB) : $(CPPOBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(CC_SHFLAGS),$@ -o $@ $^ $(SHARED_LIBS)
$(SHCPPDBGLIB) : $(CPPDBGOBJS)
	$(CXX) -g $(CXXFLAGS) $(LDFLAGS) $(CC_SHFLAGS),$@ -o $@ $^ $(SHARED_LIBS)
endif


#********************************  OBJECTS  *********************************

$(COBJS) $(CDBGOBJS) $(CPPOBJS) $(CPPDBGOBJS) : aiousb.h AIOUSB_Core.h

OBJOPTS			:= -c 
COMMONOPTS		:= -Wall -pthread
ifneq ("$(OSTYPE)","CYGWIN")
COMMONOPTS		+= -fPIC
endif


EXTRA_COPTS		:= -D_GNU_SOURCE
SELF_TEST_FLAGS		:= -DSELF_TEST
SELF_TEST_LIBS		:= -lm -laiousbdbg -lusb-1.0
SELF_TEST_CXX_FLAGS	:= -D__aiousb_cplusplus -DSELF_TEST -std=c++11 $(CXXFLAGS)
SELF_TEST_CXX_LIBS	:= -lm -lusb-1.0 -laiousbcppdbg -lgmock -lgtest -lpthread

PYCFLAGS		= $(DEBUGOPTS) $(COMMONOPTS)
COPTS			:= $(DEBUGOPTS) $(COMMONOPTS) $(EXTRA_COPTS)
CPPOPTS			:= $(DEBUGOPTS) $(COMMONOPTS)


$(CORE_COBJS) : %.o : %.c $(HEADERS)
	$(CC) $(OBJOPTS) $(CFLAGS) $(COPTS) $< -o $@

$(CEXTRAOBJS) : %.o : %.c
	$(CC) $(OBJOPTS) $(CFLAGS) $(COPTS) $< -o $@

$(CORE_CDBGOBJS) : %.dbg.o : %.c
	$(CC) $(OBJOPTS) $(DEBUG_FLAGS) $(CFLAGS) $(COPTS) $< -o $@

$(CORE_CPPOBJS) : %.cpp.o : %.c
	$(CXX) $(CXXFLAGS) $(OBJOPTS) $(CPPOPTS) $< -o $@

$(CPPEXTRAOBJS) : %.cpp.o : %.c
	$(CXX) $(CXXFLAGS) $(OBJOPTS) $(CPPOPTS) $< -o $@

$(CORE_CPPDBGOBJS) : %.cpp.dbg.o : %.c
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) $(OBJOPTS) $(CPPOPTS) $< -o $@

pywrapper:
	CC=$(CC) CFLAGS="$(EXTRA_COPTS)  -lusb-1.0" python setup.py build_ext


#
# Self testing files
#
%_c_test: %.c $(SHCDBGLIB) $(SHCPPDBGLIB)
	$(CC) $(TESTFLAGS) $(CFLAGS) $(SELF_TEST_FLAGS) $< -o $@ -L. $(SELF_TEST_LIBS)

%_cpp_test: %.c %.h $(SHCDBGLIB) $(SHCPPDBGLIB) $(SHCLIB) $(SHCPPLIB) 
	$(CXX) $(TESTFLAGS) $(SELF_TEST_CXX_FLAGS) $(subst _cpp_test,.c,$<) -o $@ -L. $(SELF_TEST_CXX_LIBS) $(LDFLAGS)

tests/%_cpp_test: tests/%.cpp $(SHCDBGLIB) $(SHCPPDBGLIB) $(SHCLIB) $(SHCPPLIB) 
	$(CXX) $(TESTFLAGS) $(CXXFLAGS) $(TEST_EXTRA_CXXFLAGS) $(SELF_TEST_CXX_FLAGS) $< -o $@ -L. $(SELF_TEST_CXX_LIBS)

mostlyclean:
	-rm -f $(COBJS) $(CDBGOBJS) $(CPPOBJS) $(CPPDBGOBJS) AIOUSB_Version.h

clean:
	-rm -f $(COBJS) $(CDBGOBJS) $(CPPOBJS) $(CPPDBGOBJS) $(GTAGS_FILES) $(TESTOBJS) $(CPPTEST_FILES) AIOUSB_Version.h

distclean: 
	-rm -f $(LIBS) $(COBJS) $(CDBGOBJS) $(CPPOBJS) $(CPPDBGOBJS) $(TESTOBJS) cscope.out *_test* *.tap *.mexglx
gtags:	
	gtags

libaiousb.h: filepp/libaiousb.h.pre filepp/header.h filepp/footer.h 
	perl -I./filepp $$(which filepp)  -I. -I${PWD}/filepp -m Between.pm filepp/libaiousb.h.pre > $@

matlab: 
	@if [ -z "$$MATLAB_BIN_DIR" ] ; then echo "You must set MATLAB_BIN_DIR to the path to matlab binary"; exit 1; fi
	@export CPATH=/usr/include/libusb-1.0 && \
	$$MATLAB_BIN_DIR/mex -v CXX=g++ CC=gcc -lm -output libaiousb $(patsubst %.o,%.c,$(COBJS)) -lusb-1.0
