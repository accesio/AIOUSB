#
# $RCSfile: Makefile,v $
# $Revision: 1.12 $
# $Date: 2009/12/20 23:41:12 $
# jEdit:tabSize=4:indentSize=4:collapseFolds=1:
#
# make file for AIOUSB java class library and associated documentation
#



JAR := aiousb.jar
JAVAPACKAGE := com.acces.aiousb
JAVADIR := $(subst .,/,$(JAVAPACKAGE))
JAVASOURCES := $(wildcard $(JAVADIR)/*.java)
JAVACLASSES := $(subst .java,.class,$(JAVASOURCES))
ALLJAVACLASSES := $(JAVADIR)/*.class			# use '*.class' to include anonymous classes

JNILIBNAME := libjavaaiousb.so
JNILIBDIR := lib
JNILIB := $(JNILIBDIR)/$(JNILIBNAME)
JNIHEADERCLASSNAMES := AnalogInputSubsystem \
AnalogOutputSubsystem \
AO16_AnalogOutputSubsystem \
Counter \
CounterSubsystem \
DigitalIOSubsystem \
DIOStreamSubsystem \
USBDevice \
USBDeviceManager
JNIHEADERPREFIX := $(JNILIBDIR)/$(subst .,_,$(JAVAPACKAGE))_
JNIHEADERS := $(addsuffix .h,$(addprefix $(JNIHEADERPREFIX),$(JNIHEADERCLASSNAMES)))
JNISOURCES := $(JNILIBDIR)/Java_AIOUSB.c
JNIOBJS := $(subst .c,.o,$(JNISOURCES))
ifdef AIOUSBLIBDIR
	LIBAIOUSB := $(AIOUSBLIBDIR)/libaiousb.a
else
	LIBAIOUSB := -laiousb						# use default library search path
endif

DOCSRCDIR := .
DOCDIR := doc
DOCINDEX := $(DOCDIR)/index.html				# use index.html as a proxy for all documentation files
DOCOVERVIEW = Overview.html
DOCTITLE := "AIOUSB Java Class Library Reference"

vpath %.class $(JAVADIR)

.PHONY : deprecated
deprecated:
	@echo "!!! This code is deprecated and no longer supported by ACCES I/O Products !!!"	

.PHONY : all 
all: $(JAR) $(JNILIB) $(DOCINDEX) deprecated

.PHONY : mostlyclean
mostlyclean:
	-rm -f $(ALLJAVACLASSES) $(JNIOBJS)

.PHONY : clean
clean:
	-rm -f $(JAR) $(ALLJAVACLASSES) $(JNILIB) $(JNIOBJS) $(DOCINDEX)

$(JAR) : deprecated $(JAVACLASSES)
	-rm -f $(JAR)
	jar cf $(JAR) $(ALLJAVACLASSES)
	chmod a-w $(JAR)

%.class : %.java
	javac -g:none -cp . $<

$(JNILIB) : $(JNISOURCES) $(JNIHEADERS)
	gcc -DNDEBUG -Wall -pthread -fPIC $(JNISOURCES) $(LIBAIOUSB) -lusb-1.0 -o $(JNILIB) -shared -Wl,-soname,$(JNILIBNAME)

$(JNILIBDIR)/com_acces_aiousb_AnalogInputSubsystem.h: $(JAVADIR)/AnalogInputSubsystem.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(JNILIBDIR)/com_acces_aiousb_AnalogOutputSubsystem.h: $(JAVADIR)/AnalogOutputSubsystem.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(JNILIBDIR)/com_acces_aiousb_AO16_AnalogOutputSubsystem.h: $(JAVADIR)/AO16_AnalogOutputSubsystem.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(JNILIBDIR)/com_acces_aiousb_Counter.h: $(JAVADIR)/Counter.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(JNILIBDIR)/com_acces_aiousb_CounterSubsystem.h: $(JAVADIR)/CounterSubsystem.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(JNILIBDIR)/com_acces_aiousb_DigitalIOSubsystem.h: $(JAVADIR)/DigitalIOSubsystem.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(JNILIBDIR)/com_acces_aiousb_DIOStreamSubsystem.h: $(JAVADIR)/DIOStreamSubsystem.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(JNILIBDIR)/com_acces_aiousb_USBDevice.h: $(JAVADIR)/USBDevice.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(JNILIBDIR)/com_acces_aiousb_USBDeviceManager.h: $(JAVADIR)/USBDeviceManager.class
	javah -force -d $(JNILIBDIR) -classpath . $(subst $(JAVADIR)/,$(JAVAPACKAGE).,$(basename $<))

$(DOCINDEX) : $(DOCOVERVIEW) $(JAVACLASSES)
	javadoc -overview $(DOCOVERVIEW) -windowtitle $(DOCTITLE) -public -sourcepath $(DOCSRCDIR) -d $(DOCDIR) $(JAVAPACKAGE)

# end of file
