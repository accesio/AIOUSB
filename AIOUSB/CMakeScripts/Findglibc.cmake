# - Try to find glibc version
# Once done this will define
#
# GLIBC_FOUND - system has GLIBC
# GLIBC_VERSION - verion number of GLIBC
#
# (Changes for libusb) Copyright (c) 2016 Jimi Damon <jdamon@accesio.com>
#
# Redistribution and use is allowed according to the terms of the New BSD license.
#
# CMake-Modules Project New BSD License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# * Neither the name of the CMake-Modules Project nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#


if ( GLIBC_FOUND AND GLIBC_VERSION )
  # in cache already
  set(GLIBC_FOUND TRUE)
else ()
  if ( UNIX AND NOT APPLE ) 
    execute_process( COMMAND ldd --version RESULT_VARIABLE RESVALUE
      OUTPUT_VARIABLE LDDOUT
      ERROR_VARIABLE LDDOUT
      OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE 
      )
    #message(STATUS "Value is ${LDDOUT}")
    string(REPLACE "\n" ";" BLAH ${LDDOUT} )
    #message(STATUS "Value is ${BLAH}" )
    list(GET BLAH 0 TMP)
    #message(STATUS "Value is ${TMP}" )
    string(REGEX REPLACE "^.*([0-9]+\\.[0-9]+)" "\\1" GLIBC_VERSION "${TMP}" )
    #message(STATUS "Value is ${GLIBC_VERSION}")
    set(GLIBC_FOUND TRUE )
  elseif( CYGWIN )
    message(FATAL "Can't find GLIBC")
  elseif( WINDOWS )
    message(FATAL "Can't find GLIBC")
  endif( UNIX AND NOT APPLE )
endif( GLIBC_FOUND AND GLIBC_VERSION )

if (GLIBC_FOUND)
  if (NOT GLIBC_FIND_QUIETLY)
    message(STATUS "Found GLIBC:")
    message(STATUS " - VERSION: ${GLIBC_VERSION}")
  endif (NOT GLIBC_FIND_QUIETLY)
else (GLIBC_FOUND)
  if (GLIBC_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find GLIBC")
  endif (GLIBC_FIND_REQUIRED)
endif (GLIBC_FOUND)

# show the LIBUSB_1_INCLUDE_DIRS and LIBUSB_1_LIBRARIES variables only in the advanced view
mark_as_advanced(GLIBC_VERSION)

# endif (LIBUSB_1_LIBRARIES AND LIBUSB_1_INCLUDE_DIRS)
