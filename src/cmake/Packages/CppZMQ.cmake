################################################################################
# Module to find ZeroMQ C++ binding header                                     #
#                                                                              #
# This module will define the variables:                                       #
#                                                                              #
#   CPPZMQ_FOUND                                                               #
#   CPPZMQ_INCLUDE_DIR                                                         #
#   CPPZMQ_CPPFLAGS                                                            #
################################################################################

SET (CPPZMQ_FIND_QUIETLY TRUE)
SET (CPPZMQ_FIND_REQUIRED FALSE)

IF (NOT CPPZMQ_FOUND)

  # Search user environment for headers, then default paths
  FIND_PATH (CPPZMQ_INCLUDE_DIR zmq.hpp
    PATHS $ENV{CPPZMQROOT}/include
    NO_DEFAULT_PATH)
  FIND_PATH (CPPZMQ_INCLUDE_DIR zmq.hpp)
  GET_FILENAME_COMPONENT (CPPZMQROOT ${CPPZMQ_INCLUDE_DIR} PATH)

  # Extract CPPZMQ_VERSION
  SET (CPPZMQ_VERSION ${ZEROMQ_VERSION})

  # Set CPPZMQ_FOUND and error out if zmq is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (CPPZMQ
    DEFAULT_MSG CPPZMQ_INCLUDE_DIR)

  IF (CPPZMQ_FOUND)
    COLORMSG (HICYAN "CppZMQ version: ${CPPZMQ_VERSION}")

    # Set flags and print a status message
    SET (CPPZMQ_CPPFLAGS "-I${CPPZMQ_INCLUDE_DIR}")
    
    MESSAGE (STATUS "  * includes: ${CPPZMQ_INCLUDE_DIR}")
  ELSE (CPPZMQ_FOUND)
    MESSAGE (STATUS "CppZMQ not found")
  ENDIF (CPPZMQ_FOUND)

ENDIF (NOT CPPZMQ_FOUND)

