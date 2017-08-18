################################################################################
# Module to find Xerces-C                                                      #
#                                                                              #
# This module will find the Xerces-C parser which binds XML and C++.  The env- #
# ironment variables defined are:                                              #
#                                                                              #
#   XERCESC_FOUND                                                              #
#   XERCESC_LIBRARIES                                                          #
#   XERCESC_LIB_DIR                                                            #
#   XERCESC_INCLUDE_DIR                                                        #
#   XERCESC_CPPFLAGS                                                           #
#   XERCESC_LDFLAGS                                                            #
#   XERCESCROOT                                                                #
################################################################################

SET (XERCESC_FIND_QUIETLY TRUE)
SET (XERCESC_FIND_REQUIRED FALSE)

IF (NOT XERCESC_FOUND)

  # Search user environment for headers, then default paths
  FIND_PATH (XERCESC_INCLUDE_DIR xercesc/util/XercesVersion.hpp
    PATHS $ENV{XERCESCROOT}/include
    NO_DEFAULT_PATH)
  FIND_PATH (XERCESC_INCLUDE_DIR xercesc/util/XercesVersion.hpp)
  GET_FILENAME_COMPONENT (XERCESCROOT ${XERCESC_INCLUDE_DIR} PATH)

  # Search user environment for libraries, then default paths
  FIND_LIBRARY (XERCESC_LIBRARIES NAMES xerces-c
    PATHS $ENV{XERCESCROOT}/lib
    NO_DEFAULT_PATH)
  FIND_LIBRARY (XERCESC_LIBRARIES NAMES xerces-c)
  GET_FILENAME_COMPONENT (XERCESC_LIB_DIR ${XERCESC_LIBRARIES} PATH)

  # Set XERCESC_FOUND and error out if Xerces-C is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (XERCESC
    DEFAULT_MSG XERCESC_LIBRARIES XERCESC_INCLUDE_DIR)

  # Find the current version number of the software
  FILE (WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/xver.cxx"
        "#include <xercesc/util/XercesVersion.hpp>
         #include <iostream>
         int main() { std::cout << XERCES_FULLVERSIONDOT; return 0; }")

  TRY_RUN (_xver_EXITCODE _xver_COMPILED
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/xver.cxx
    COMPILE_DEFINITIONS "-I${XERCESCROOT}/include"
    COMPILE_OUTPUT_VARIABLE _xver_compile_OUTPUT
    RUN_OUTPUT_VARIABLE _xver_run_OUTPUT
    )

  IF (XERCESC_FOUND)
    IF ("${_xver_EXITCODE}" EQUAL 0)
      COLORMSG (HICYAN "Xerces-C version ${_xver_run_OUTPUT} found:")
    ELSE ("${_xver_EXITCODE}" EQUAL 0)
      COLORMSG (HICYAN "Xerces-C found:")
    ENDIF ("${_xver_EXITCODE}" EQUAL 0)

    # Set flags and print a status message
    SET (XERCESC_CPPFLAGS "-I${XERCESC_INCLUDE_DIR}")
    SET (XERCESC_LDFLAGS "${XERCESC_LIBRARIES}")
    
    MESSAGE (STATUS "  * includes: ${XERCESC_INCLUDE_DIR}")
    MESSAGE (STATUS "  * libs:     ${XERCESC_LIBRARIES}")
  ENDIF (XERCESC_FOUND)

ENDIF (NOT XERCESC_FOUND)

