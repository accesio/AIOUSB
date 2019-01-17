
MESSAGE(STATUS "Rules for JAVA Swig")

project( java_swig )


# Required that we have BUILD_AIOUSB_SHARED ON

FIND_PACKAGE( SWIG REQUIRED )
INCLUDE( ${SWIG_USE_FILE} )


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Java
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
FIND_PACKAGE(Java 1.7 )
FIND_PACKAGE(Java COMPONENTS Development )
FIND_PACKAGE(JNI REQUIRED)
INCLUDE(UseJava)

INCLUDE_DIRECTORIES( ${Java_INCLUDE_DIRS}  )
MESSAGE(STATUS "Using Java dirs: ${JAVA_INCLUDE_DIRS}  " )
MESSAGE(STATUS "Using Java libs: ${JAVA_LIBRARIES}  " )
MESSAGE(STATUS "Using JNI libs: ${JNI_INCLUDE_DIRS} " )

SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES CPLUSPLUS OFF)
SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES SWIG_FLAGS "-I${AIOUSB_INCLUDE_DIR} -includeall")
INCLUDE_DIRECTORIES( ${JNI_INCLUDE_DIRS} )


SET( CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/AIOUSB" )

SWIG_ADD_MODULE(AIOUSBJava java AIOUSB.i )
SET_TARGET_PROPERTIES( AIOUSBJava PROPERTIES OUTPUT_NAME AIOUSB ) 
SWIG_LINK_LIBRARIES( AIOUSBJava  ${JNI_LIBRARIES} )

SET(JAR_COMMAND sh -c "jar cf AIOUSB.jar ${CMAKE_CURRENT_BINARY_DIR}/libAIOUSB.so ${CMAKE_CURRENT_BINARY_DIR}/AIOUSB/*.java" )

ADD_CUSTOM_COMMAND(OUTPUT AIOUSB.jar
                   DEPENDS AIOUSBJava
                   COMMAND ${JAR_COMMAND}
                   COMMENT "Creating Jar File"
                   )

ADD_CUSTOM_TARGET( build_jar ALL
                  DEPENDS AIOUSB.jar 
                  COMMAND cmake -E echo "Finnished build Jar"
                  )

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/AIOUSB.jar DESTINATION  share/accesio/jars )
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libAIOUSB.so DESTINATION lib )

