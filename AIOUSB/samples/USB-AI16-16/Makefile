#
# $Revision$
# $Date$
# $Author$
#
# make file for AIOUSB library sample programs
#
SHELL	= /bin/bash
ifdef DEBUG
	DEBUGOPTS 	+= -ggdb
	DEBUGSTEM 	:= dbg
	DEBUG_ARGS	:= DEBUG=-DDEBUG
else
	DEBUGOPTS	+= -DNDEBUG
	DEBUGSTEM	:=
endif

include $(AIOUSB_ROOT)/Mkfiles/oses.inc

override CFLAGS        += -I$${AIO_LIB_DIR} -I/usr/include/libusb-1.0 -std=gnu99
override CXXFLAGS      += -I$${AIO_LIB_DIR} -I$${AIO_CLASSLIB_DIR} -Ilib -I/usr/include/libusb-1.0 -std=c++0x -D__aiousb_cplusplus

override LDFLAGS       += -L$${AIO_LIB_DIR} -L$${AIO_CLASSLIB_DIR}

PYTHON_VERSION	:= $(shell python -V 2>&1 | perl -ne 's/^Python\s+(\d+\.\d+)\..*$$/$$1/g;print;')
SYSNAME		:= $(shell uname -s | perl -pne 'tr/A-Z/a-z/;' )
ARCHNAME	:= $(shell uname -p )

ifeq ("$(OSTYPE)","Darwin") 
CC_SHFLAGS := -dynamiclib
SHLIB_CFLAGS	= $(CFLAGS) -fPIC
else 
ifeq ("$(OSTYPE)","Linux") 
CC_SHFLAGS :=  -fPIC -shared -Wl,-soname
SHLIB_CFLAGS	= $(CFLAGS) -fPIC
GLIBC_VERSION	:=$(shell ldd --version | head -1 | perl -pne 's/^.*?(\d+\.\d+)\s*$$/$$1/;')
ifeq ($(shell /bin/echo -ne ${GLIBC_VERSION} | perl -ne 'print ($$_ < 2.17 ? 0 : 1 ); '),0)
RTLIB=-lrt
else 
RTLIB=
endif
override LIBS += ${RTLIB}
else 
CC_SHFLAGS :=  -fPIC -shared -Wl,-soname
SHLIB_CFLAGS	= $(CFLAGS) -fPIC
endif
endif


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Regular C related Build
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
ifdef AIOUSBLIBDIR
	LIBAIOUSB 		:= $(AIOUSBLIBDIR)/$(LIBPREFIX)aiousb$(DEBUGSTEM).$(STATICSUFFIX)
	LIBAIOUSBCPP 		:= $(AIOUSBLIBDIR)/$(LIBPREFIX)aiousbcpp$(DEBUGSTEM).$(STATICSUFFIX)

	ALL_AIOUSBLIBDIR_CFILES	:= $(wildcard $(AIOUSBLIBDIR)/*.c )
	LIBAIOUSB_CDEP		:= $(LIBAIOUSB) $(wildcard $(AIOUSBLIBDIR)/*.c )
	CFLAGS			+= -I$(AIOUSBLIBDIR) 

#       Dynamic Build Rule for aiousblib
	AIOUSBLIB_BUILD		= $(LIBAIOUSB): $(ALL_AIOUSBLIBDIR_CFILES) ; cd $(AIOUSBLIBDIR) && $(MAKE) CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" $(DEBUG_ARGS)
else
# use default library search path
	LIBAIOUSB 		:= -laiousbdbg
	LIBAIOUSBCPP 		:= -laiousbcppdbg
	LIBAIOUSB_CDEP		:= $(wildcard $(AIO_LIB_DIR)/*.so )
	CFLAGS			+= -I$(AIO_LIB_DIR) 
endif

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# C++ Related Build
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
ifdef AIOUSBCLASSLIBDIR
	LIBCLASSAIOUSB 		:= $(AIOUSBCLASSLIBDIR)/$(LIBPREFIX)classaiousbdbg.$(STATICSUFFIX)
	ALL_AIOUSBLIBDIR_CFILES	:= $(wildcard $(AIOUSBLIBDIR)/*.c )
	ALL_CLASSLIB_CPPFILES	:= $(wildcard $(AIOUSBCLASSLIBDIR)/*.c)
	LIBAIOUSB_CPPDEP	:= $(LIBCLASSAIOUSB) $(ALL_AIOUSBLIBDIR_CFILES)  $(ALL_CLASSLIB_CPPFILES)

	CFLAGS			+=  -I$(AIOUSBCLASSLIBDIR)
#       Dynamic Build rule for C++ lib
	CLASSLIB_BUILD  	=  $(LIBCLASSAIOUSB): $(LIBAIOUSB) $(ALL_AIOUSBLIBDIR_CFILES)  $(ALL_CLASSLIB_CPPFILES) ; cd $(AIOUSBCLASSLIBDIR) && $(MAKE) $(DEBUG_ARGS)
else
	LIBCLASSAIOUSB 		:= -lclassaiousbdbg		# use default library search path
endif

ifndef AIOUSBJAVALIBDIR
	AIOUSBJAVALIBDIR := ../../lib/wrappers/java
endif

CPP			:= g++
CC			:= gcc
JAVAC			:= javac

include $(AIO_LIB_DIR)/wrappers/WrapperTargets.inc


COMPILE_MACROS		:= $(MACROS)
LIBUSB 			:= -lusb-1.0
COMMONOPTS 		:= -Wall -pthread 
ifneq ("$(OSTYPE)","CYGWIN")
	COMMON_OPTS	+= -fPIC $(COMPILE_MACROS)
endif

REVERSE_CAL_DEPS	:= reverse_cal_table.o lib/TestCaseSetup.o 
COMMON_DEPS             := lib/TestCaseSetup.o

COMMON_C_DEPS		:= $(patsubst lib/%.c,lib/%.o,$(wildcard lib/*.c))

LIBRARY_DEPS		:= $(LIBAIOUSB_CDEP) $(LIBAIOUSB_CPPDEP)

OBJFILE_OPTS		:= -c 
INCPATH			:= -Ilib $(CFLAGS)
LIBPATH			:= -Llib $(LDFLAGS)

CPPLIBS			+= $(LIBCLASSAIOUSB) $(LIBAIOUSBCPP)  $(LIBUSB) -lm $(LIBS)
CLIBS			+= $(COMMON_C_DEPS) $(LIBAIOUSB) $(LIBUSB) -lm $(LIBS) 

TARGETS			:= $(wildcard *.cpp)
CTARGETS		:= $(wildcard *.c)
OBJECT_FILES		:= $(patsubst %.cpp,%.o,$(TARGETS)) $(patsubst %.c,%.c.o,$(CTARGETS))
JAVA_TARGETS		:= $(wildcard *.java)
JAVA_OBJECT_FILES	:= $(patsubst %.java,%.class,$(JAVA_TARGETS)) \
				  $(patsubst %.java,%.jar,$(JAVA_TARGETS))

FINAL_TARGETS		:= $(patsubst %.cpp,%,$(TARGETS))  $(patsubst %.c,%,$(CTARGETS))

ALL_CLEAN_TARGETS	:=	$(JAVA_OBJECT_FILES) \
			  	$(OBJECT_FILES) \
			  	$(COMMON_DEPS) \
				$(FINAL_TARGETS)


#*********************************  RULES  **********************************
.PHONY: clean all
.SUFFIXES:

all: $(FINAL_TARGETS)

#
# Debugging targets
#
test:
	@echo "JAVA FILES=$(JAVA_OBJECT_FILES)"
	@echo "LIBRARY_DEPS=$(LIBRARY_DEPS)"
	@echo "CLEAN TARGETS=$(ALL_CLEAN_TARGETS)"
	@echo "CLASSLIB_DIR=$(AIOUSBCLASSLIBDIR)"
	@echo "AIOUSBLIB_DIR= $(AIOUSBLIBDIR)"
	@echo "AIOLIB=$(LIBCLASSAIOUSB)"
	@echo "LIBRARY_DEPS=$(LIBRARY_DEPS)"
	@echo "BUILD_DEPS=$(LIBAIOUSB_CPPDEP)"
	@echo "COMMON_C_DEPS=$(COMMON_C_DEPS)"
	@echo "AIOUSBLIBDIR=:$(AIOUSBLIBDIR):"
	@echo "LIBPREFIX=:$(LIBPREFIX):"
	@echo "CLIBS=$(CLIBS)"
	@echo "GLIBC_VERSION=${GLIBC_VERSION}"
	@echo "RTLIB=${RTLIB}"
	@echo "PYTHON_VERSION=${PYTHON_VERSION}"
	@echo "SYSNAME=${SYSNAME}"

clean:
	$(RM) $(ALL_CLEAN_TARGETS)
	$(RM) *.csv
	$(RM) *.class
	$(RM) *.log
	$(RM) lib/*.so
	$(RM) lib/*.o

#
# This sets up the dependencies for our .a builds
#

$(eval $(call AIOUSBLIB_BUILD))
$(eval $(call CLASSLIB_BUILD))

$(AIO_LIB_DIR)/wrappers/java/$(JAVA_BUILT):
	cd $(AIO_LIB_DIR)/wrappers && $(MAKE) -f GNUMakefile inplace_java

%.jar: %.class $(AIO_LIB_DIR)/wrappers/java/$(JAVA_BUILT)
	mkdir -p tmplib
	cp $(AIOUSBJAVALIBDIR)/AIOUSB.jar tmplib
	@echo -ne "Class-Path: tmplib/AIOUSB.jar " > manifest
	@if [[ "$(shell echo $< | perl -ne 's/^.*(_)([A-Za-z\d]+?)\.class/$$2/g;print;')" == "scala" ]] ; then \
		cp /home/jdamon/Tools/Scala/2.11.0/lib/scala-library.jar tmplib;\
		echo -ne " tmplib/scala-library.jar\n" >> manifest;\
	fi
	@echo "Main-Class: $(shell grep -P "^\s*(object|(public )?class)" $(shell echo $< | perl -ne 's/(_)([A-Za-z\d]+?)\.class/$$1$$2.$$2/g;print;') | perl -ane 'print $$F[1];' )" >> manifest
	jar cvfm $@ manifest $(basename $<)*.class tmplib/*.jar
	rm manifest

%:	%.java
	javac -cp "$${AIO_LIB_DIR}/wrappers/java/AIOUSB.jar:$<" $< 
	LD_LIBRARY_PATH="$${LD_LIBRARY_PATH}:$${AIO_LIB_DIR}/wrappers/java:$${AIO_LIB_DIR}" java -classpath ".:$${AIO_LIB_DIR}/wrappers/java/AIOUSB.jar" read_channels_test_java  $(ARGS)

$(AIO_LIB_DIR)/%.so:	$(wildcard $(AIO_LIB_DIR)/*.c)
	cd $(AIO_LIB_DIR) && $(MAKE) $(nodir $@)

%.class:	%.java
	$(JAVAC) -cp $(AIOUSBJAVALIBDIR)/AIOUSB.jar $<


%.class: 	%.scala
	scalac -classpath $(AIO_LIB_DIR)/wrappers/java $<


%:	%.scala
	scalac -classpath "${AIO_LIB_DIR}/wrappers/java/AIOUSB.jar:." $<
	LD_LIBRARY_PATH="$$LD_LIBRARY_PATH:$$AIO_LIB_DIR/wrappers/java:$${AIO_LIB_DIR}" scala -classpath "${AIO_LIB_DIR}/wrappers/java/AIOUSB.jar:." $(basename $<)  $(ARGS)


Extcal.jar: Extcal.class
	@echo "Class-Path: $(AIOUSBJAVALIBDIR)/AIOUSB.jar" > manifest
	jar cmfe manifest Extcal.jar Extcal Extcal.class
	rm manifest


Extcal.class: Extcal.java
	$(JAVAC) -cp $(AIOUSBJAVALIBDIR)/AIOUSB.jar Extcal.java

# mostlyclean:
# 	-rm -f sample.o extcal.o Sample.class Extcal.class


lib/TestCaseSetup.o: lib/TestCaseSetup.cpp lib/TestCaseSetup.h  $(LIBRARY_DEPS)
	$(CPP) $(DEBUGOPTS) $(CXXFLAGS) $(LIBPATH)  $(OBJFILE_OPTS) $(COMMONOPTS) $(CPPOPTS) $<  $(LIBAIOUSBCPP) $(LIBUSB) -o $@

lib/%.o:lib/%.c 
	$(CC) $(CFLAGS)  -D_GNU_SOURCE $(OBJFILE_OPTS) $(INCPATH) $(DEBUGOPTS) $(LIBPATH) $(COMMONOPTS) $(COPTS) $<  -o $@

# $(CC) $(DEBUGOPTS) $(SHLIB_CFLAGS) -c $< -o $(patsubst %.c,%.o,$<)
# $(CC) $(CC_SHFLAGS),$@ -o $@ $(patsubst %.c,%.o,$<)	
#
# Smart Generic rules that can build any NEW C or C++ file that 
# you add to this directory
#

#
# C++ Rules
#
%.o:%.cpp $(COMMON_DEPS) $(LIBRARY_DEPS)
	$(CPP) $(DEBUGOPTS) $(CXXFLAGS) $(OBJFILE_OPTS) $(COMMONOPTS) $(CPPOPTS) $< $(LIBPATH) -o $@

%:%.o	$(COMMON_DEPS) $(LIBRARY_DEPS) 
	$(CPP)  $(LDFLAGS) $(CXXFLAGS) $(DEBUGOPTS)  $(COMMONOPTS) $(CPPOPTS) $< $(COMMON_DEPS) $(CPPLIBS)  -o $@ $(LDFLAGS)


#
# C Rules
#
%.c.o:%.c	$(LIBRARY_DEPS) $(COMMON_C_DEPS) 
	$(CC) $(CFLAGS)  -D_GNU_SOURCE $(OBJFILE_OPTS) $(INCPATH) $(DEBUGOPTS) $(LIBPATH) $(COMMONOPTS) $(COPTS) $<  -o $@

%:%.c.o		$(COMMON_C_DEPS)
	$(CC) $(DEBUGOPTS) $(LIBPATH) $(COMMONOPTS) $< $(CLIBS) -o $@ $(LDFLAGS)


#
# Python rules
#
python_%:%.py
	 PYTHONPATH="$$AIO_LIB_DIR/wrappers/python/build/lib.$(SYSNAME)-$(ARCHNAME)-$(PYTHON_VERSION)" python $< 
