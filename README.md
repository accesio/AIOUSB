[![ACCES I/O Products Inc. Logo](http://accesio.com/images/weblogo2.png)](http://accesio.com)





# <a name="AIOUSBLibrary"></a>AIOUSB driver library

[![Build Status](https://travis-ci.org/accesio/AIOUSB.svg?branch=master)](https://travis-ci.org/accesio/AIOUSB)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
[![Github Releases](https://img.shields.io/github/release/accesio/AIOUSB.svg)](https://github.com/accesio/AIOUSB/releases)
[![Github Issues](https://img.shields.io/github/issues/accesio/AIOUSB.svg)](http://github.com/accesio/AIOUSB/issues)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/28dc815eb6bc4c718a3e921f63569187)](https://www.codacy.com/app/accesio/AIOUSB?utm_source=github.com&utm_medium=referral&utm_content=accesio/AIOUSB&utm_campaign=badger)

# Overview

This project contains USB drivers and APIs for ACCES I/O Product's line of USB based data acquisition modules. This driver represents a large API collection for communicating with one or more of ACCES I/O Product's line of USB based data acquisition products. All of the core functionality that exists and is supported by the Windows software is implemented in this library for non-Windows based operating systems.  This code base compiles using either GCC and Clang compilers to both shared and static libraries that be can used in applications that need to perform highspeed USB data acquisition.

The entire set of drivers are rely on functionality provided by the [libusb-1.0](http://libusb.info/) library. Please see the [prequisites](#Prerequisites) section to find out about required software for building the driver.

Currently, this project provides full support to the following platforms:

* Linux 
* Mac OS X
* Free / Net BSD
* [Rasberry Pi](https://www.raspberrypi.org/) 
* [Beagle Board](http://beagleboard.org/bone)
* POSIX compliant operating systems that can successfully compile and use libusb.
* Windows with Cygwin


## <a name="Prerequisites"></a>Prerequisites
The functionality in this driver depends on the following installed packages.

1. [libusb-1.0](http://www.libusb.info/)
2. [cmake]( http://www.cmake.org/cmake/resources/software.html )
3. [swig](http://swig.org/)
4. (Optionally for python bindings ) [python-dev](http://python.org)

#### Ubuntu / Debian

~~~bash
sudo apt-get install libusb-1.0 libusb-1.0-0-dev cmake swig python-dev
~~~


#### Fedora / Red Hat
~~~bash
sudo yum install libusb-1.0 cmake swig
~~~

#### Open SUSE
~~~bash
sudo zypper install libusb-1.0 cmake swig
~~~

#### Mac OS X

##### Homebrew

~~~bash
brew install libusb  cmake
~~~

##### Darwin Ports

~~~bash
sudo port install libusb cmake
~~~

##### Rasberry Pi

~~~bash
sudo apt-get install libusb-1.0-0 libusb-1.0-0-dev  cmake 
sudo apt-get install python-dev # If you want Python bindings
~~~


##### Beagle Board
~~~bash
sudo apt-get install libusb-1.0-0 libusb-1.0-0-dev cmake 
sudo apt-get install python-dev # If you want Python bindings
~~~


# <a name="UnixBuild"></a>Building on Linux/ MacOS / BSD / *NIX systems

Building ACCES I/O Products' Driver library amounts to compiling C source files to produce C and C++ based shared ( .so ) or static (.a) libraries.  The build process relies on either GNU make or Cmake.  The first method of building ( see [non-cmake users](#NonCmakeBuild) is a little more involved but will give you the ability to [build wrapper language packs](#BuildWrapperLanguages).  Currently ,the simplified cmake system is easier to build and install the general libraries but we have been unable to use it to deploy the Swig based wrappers as we would have liked.   The other option is the [CMake build](#CMakeBuild).

## <a name="NonCmakeBuild"></a> <a href="http://www.gnu.org/software/make">GNU make</a> build


You will need to do the following

~~~bash
cd AIOUSB
source sourceme.sh
cd ${AIO_LIB_DIR} && make && cd -
cd ${AIO_CLASSLIB_DIR} && make && cd -
cd samples/USB_SAMPLE_OF_CHOICE
make sample AIOUSBLIBDIR=${AIO_LIB_DIR} AIOUSBCLASSLIBDIR=${AIO_CLASSLIB_DIR} DEBUG=1
~~~

## <a name="CMakeBuild"></a>Build with <a href="https://cmake.org/">CMake</a>

~~~bash
cd AIOUSB
mkdir build
cd build
cmake ..
make
sudo make install
~~~

## <a name="OptionalStaticLibraries"></a>Building static libraries with CMake</a>

~~~bash
cd AIOUSB
mkdir build
cd build
cmake -DBUILD_AIOUSBCPPDBG_SHARED=OFF \
 -DBUILD_AIOUSBCPP_SHARED=OFF \
 -DBUILD_AIOUSBDBG_SHARED=OFF \
 -DBUILD_AIOUSB_SHARED=OFF ..
make
sudo make install
~~~

## <a name="Installation"></a>Installation


### Linux Installation

1. Install fxload either using the appropriate installation tool for
your platform or by installing from
https://github.com/accesio/fxload.  Copy fxload to a standard location
in your $PATH.

2. sudo cp AIOUSB/Firmware/*.hex /usr/share/usb/

3. sudo cp AIOUSB/Firmware/10-acces*.rules /etc/udev/rules.d


### Mac Installation (work in progress!!)

1. Build and Install fxload from https://github.com/accesio/fxload and
copy fxload to a standard location in your $PATH.


2. Determine the raw USB Device ID for your card by looking for the
Vendor ID 1605 in your System Profiler. Set the variable PRODUCTID to be
this value.


3. Manually upload your corresponding firmware to your device by
running the following:

fxload -t fx2lp -I AIOUSB/Firmware/CORRESPONDING_HEXFILE.hex -D 1605:${PRODUCTID}



### Windows Installation

1. Un-install all of the AIOUSB drivers that have been installed and are associated with your device. The procedure to do this is as follows:
   1. Plug in your card
   2. Go to device manager , search for data acqusition products and remove the AIOUSB / ACCES I/O driver associated with your card. 
   3. Click the Scan For Hardware Changes toolbar icon, or the equivalent menu item.
   4. If Windows detects and reinstalls the device, go back to step C. You may have to repeat this loop many times, depending on how (and how many times) you've installed our drivers. If the device shows up as an "unknown" of some kind, proceed to step F.

2. Download the [WinUSB 8.10 drivers](http://www.accesio.com/files/forever/ACCESDrivers810.zip) and
   extract them to a saved directory.

3. In Device Manager right click on the original USB Data acquisition
device that should now have no driver associated with it and it should
be listed as a generic USB device.  Right click on the device and
click "Update Driver" and instruct Windows to look for the drivers in
the "Saved_Directory" from step 2.

4. After it has installed, under Device Manager the device should now
be listed as a Data Acquisition product.  In addition, Check the name
the device ended up with; it should have a (WinUSB) tag, like "ACCES
USB-IIRO-16 (WinUSB)". If it has a (CyUSB) tag instead, something went
wrong, please tell us.

5. Make sure that you already have a full Cygwin installation along with the following packages:
   - python
   - python-development
   - cmake
   - gnumake
   - libusb-1.0
   - gcc
   - g++ 
   - swig ( if you want to build wrapper language support )


6. Follow the instructions listed at either [Cmake build](#CMakeBuild) or [GNU make](#NonCmakeBuild)

## Extra Language Support
In addition, to providing fully functional C Shared and Static libraries, this project also provides
wrapper language support for the following languages:

* Java
* Perl
* Python
* Ruby
* PHP
* Octave
* R

## <a name="BuildWrapperLanguages"></a>How to build Wrapper languages

### <a name="CMakeBuildWrappers"></a>CMake

This is the easiest way to build the wrapper languages. Perform the following

~~~bash
cmake  -DCMAKE_INSTALL_PREFIX=/some/path/Dir  -DBUILD_PERL=ON -DBUILD_JAVA=ON ..
~~~

This will build the languages for Perl and Java. The remaining languages that can be built are
Python ( -DBUILD_PYTHON=ON ) , Ruby (-DBUILD_RUBY=ON), PHP (-DBUILD_PHP=ON) and R (-DBUILD_R=ON) 
while Octave is currently not ready yet. The installation of these wrapper scripts will default be written
to the CMAKE_INSTALL_PREFIX. To better customize the installation, you should use 

~~~bash
ccmake -DCMAKE_INSTALL_PREFIX=/some/path/Dir ..
~~~

or if you have installed cmake-gui, then

~~~bash
cmake-gui -DCMAKE_INSTALL_PREFIX=/some/path/Dir ..
~~~



### <a name="RegularBuildWrappers"></a> Regular Make system for building wrapper language support

Perform this step *AFTER* you have already followed the instructions
for building the aiousb libraries.  

#### Perl
~~~bash
cd AIOUSB/lib/wrappers
make -f GNUMakefile inplace_perl
cd perl
sudo make install

~~~

#### Java

You must make sure that you have the Java Development Kit installed (
JDK ). 
~~~bash
export CPATH=$CPATH:$JAVA_HOME/include # example /usr/lib/jvm/java-7-openjdk-i386/include
cd AIOUSB/lib/wrappers
make -f GNUMakefile inplace_java
sudo cp java/{AIOUSB.jar,libaiousb.jar} $JAR_FOLDER

~~~

#### Python
~~~bash
pyver=$(python  -c 'import platform; print platform.python_version()')
cd AIOUSB/lib/wrappers
make -f GNUMakefile inplace_python
sudo cp python/build/lib.linux-$(uname -m)-${pyver}/* /usr/lib/python${pyver}/

~~~

#### Ruby
~~~bash
cd AIOUSB/lib/wrappers
make -f GNUMakefile inplace_ruby

~~~

#### Octave
~~~bash
cd AIOUSB/lib/wrappers
make -f GNUMakefile inplace_octave

~~~

#### R
~~~bash
cd AIOUSB/lib/wrappers
make -f GNUMakefile inplace_R
~~~




Users who wish to build web applications around the ACCES I/O Product line might consider one of these
for faster development cycles. Suggestions for additional languages and features are well received and can 
be made to suggestions _AT_  accesio _DOT_ com


Sincerely,

The ACCES I/O Development team.
