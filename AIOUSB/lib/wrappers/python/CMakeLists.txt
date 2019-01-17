#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# CMAKE file for building Python wrappers 
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

MESSAGE(STATUS "Rules for building Python wrapper")

project( pythonaiousb )

# Required that we have BUILD_AIOUSB_SHARED ON
#OPTION(SPECIFY_PYTHON_INSTALL_DIRECTORY "Build the Python Interfaces" OFF)

FIND_PACKAGE( SWIG REQUIRED )
FIND_PACKAGE( PythonInterp REQUIRED )
INCLUDE(${SWIG_USE_FILE})

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Python
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
FIND_PACKAGE( PythonLibs )
INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES CPLUSPLUS OFF)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES SWIG_FLAGS "-I. -includeall")

execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import distutils.sysconfig; print distutils.sysconfig.get_python_version()"
  OUTPUT_VARIABLE PY_VERSION 
  RESULT_VARIABLE _res 
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

SWIG_ADD_MODULE(AIOUSB python AIOUSB.i )
SWIG_LINK_LIBRARIES(AIOUSB ${PYTHON_LIBRARIES} aiousb )

IF( PYTHON_INSTALL_DIRECTORY ) 
SET(PYTHON_FOUND_INSTALL_PATH TRUE)
ELSE( PYTHON_INSTALL_DIRECTORY ) 
  SET(PYTHON_INSTALL_DIRECTORY "lib/python${PY_VERSION}" CACHE PATH "Location of your Python library directory"  )
ENDIF( PYTHON_INSTALL_DIRECTORY ) 


INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/AIOUSB.py DESTINATION  ${PYTHON_INSTALL_DIRECTORY} )
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/_AIOUSB.so DESTINATION ${PYTHON_INSTALL_DIRECTORY} )

