#
#

SHELL	= /bin/bash
MKDIR	:= mkdir 
CP	:= cp
JAVAC	:= javac
JAR	:= jar
CURAPP	:= $(shell basename $(shell pwd))
ARCH	:= $(shell uname -p )


.PHONY  : build clean


#test:
	echo $(shell basename $(shell pwd))

clean:
	$(RM) -r build
	$(RM) -r lib libs
	$(RM) *.jar

build-dir:
	$(MKDIR) -p build/{classes,dependency-cache,libs,tmp}


lib-dir:
	$(MKDIR) -p ./{lib,libs}


local-jars:	lib-dir
	$(CP) $${AIO_LIB_DIR}/wrappers/java/AIOUSB.jar libs

local-libs: lib-dir
	$(CP) $${AIO_LIB_DIR}/wrappers/java/lib*.so lib
	$(CP) $${AIO_LIB_DIR}/libaiousbdbg.so lib

javac-files: lib-dir build-dir
	$(JAVAC) -d  build/classes -classpath $$(find libs -name "*.jar" -printf "%p:")  $$(find src -name "*.java")

cp-libs: local-libs local-jars build-dir
	cp libs/* build/libs/


jar:	cp-libs javac-files
	$(JAR) Mcvf  $(CURAPP)-all-1.0.jar META-INF -C build/classes . -C build libs -C lib .




