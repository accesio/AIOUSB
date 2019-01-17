# - Try to find gtesttap-1.0
# Once done this will define
#
# GTESTTAP_FOUND - system has gtesttap
# GTESTTAP_INCLUDE_DIRS - the gtesttap include directory
# GTESTTAP_DEFINITIONS - Compiler switches required for using gtesttap
#
# Adapted from cmake-modules Google Code project
#
# Copyright (c) 2006 Jimi Damon  <jdamon@accesio.org>
#
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


if (GTESTTAP_INCLUDE_DIRS)
  # in cache already
  set(GTESTTAP_FOUND TRUE)
else ( GTESTTAP_INCLUDE_DIRS)
  find_path(GTESTTAP_INCLUDE_DIR
    NAMES
    tap.h
    PATHS
      $ENV{GMOCK_TEST}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  set(GTESTTAP_INCLUDE_DIRS
    ${GTESTTAP_INCLUDE_DIR}
  )

  if ( GTESTTAP_INCLUDE_DIRS )
     set(GTESTTAP_FOUND TRUE)
  endif (GTESTTAP_INCLUDE_DIRS)

  if (GTESTTAP_FOUND)
    if (NOT gtesttap_FIND_QUIETLY)
      message(STATUS "Found GTest tap.h")
message(STATUS " - Includes: ${GTESTTAP_INCLUDE_DIRS}")
    endif (NOT gtesttap_FIND_QUIETLY)
  else (GTESTTAP_FOUND)
    if (gtesttap_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find GTest tap.h")
    endif (gtesttap_FIND_REQUIRED)
  endif (GTESTTAP_FOUND)

  # show the GTESTTAP_INCLUDE_DIRS and GTESTTAP_LIBRARIES variables only in the advanced view
  mark_as_advanced(GTESTTAP_INCLUDE_DIRS GTESTTAP_LIBRARIES)

endif (GTESTTAP_INCLUDE_DIRS)
