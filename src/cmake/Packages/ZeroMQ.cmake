################################################################################
# Module to find ZeroMQ headers and libraries                                  #
#                                                                              #
# This module will find the ZeroMQ socket library and define the variables:    #
#                                                                              #
#   ZEROMQ_FOUND                                                               #
#   ZEROMQ_LIBRARIES                                                           #
#   ZEROMQ_INCLUDE_DIR                                                         #
#   ZEROMQ_CPPFLAGS                                                            #
#   ZEROMQ_LDFLAGS                                                             #
#   ZEROMQ_VERSION                                                             #
################################################################################

SET (ZEROMQ_FIND_QUIETLY TRUE)
SET (ZEROMQ_FIND_REQUIRED FALSE)

IF (NOT ZEROMQ_FOUND)

  # Search user environment for headers, then default paths
  FIND_PATH (ZEROMQ_INCLUDE_DIR zmq.hpp zmq.h
    PATHS $ENV{ZEROMQROOT}/include
    NO_DEFAULT_PATH)
  FIND_PATH (ZEROMQ_INCLUDE_DIR zmq.hpp zmq.h)
  GET_FILENAME_COMPONENT (ZEROMQROOT ${ZEROMQ_INCLUDE_DIR} PATH)

  # Extract ZEROMQ_VERSION
  FILE (WRITE "${CMAKE_BINARY_DIR}/ztest.cxx"
    "#include <zmq.h>
     #include <iostream>
     int main()
     { std::cout << ZMQ_VERSION_MAJOR << '.' << ZMQ_VERSION_MINOR << '.'
                 << ZMQ_VERSION_PATCH;
       return 0; }")
  TRY_RUN (ZMQ_EXIT ZMQ_COMPILED
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/ztest.cxx
    COMPILE_DEFINITIONS "-I${ZEROMQ_INCLUDE_DIR}"
    COMPILE_OUTPUT_VARIABLE ZMQ_VERSION_COMPILED
    RUN_OUTPUT_VARIABLE ZEROMQ_VERSION)

  # Search user environment for libraries, then default paths
  FIND_LIBRARY (ZEROMQ_LIBRARIES zmq
    PATHS $ENV{ZEROMQROOT}/lib
    NO_DEFAULT_PATH)
  FIND_LIBRARY (ZEROMQ_LIBRARIES zmq)

  # Set ZEROMQ_FOUND and error out if zmq is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (ZEROMQ
    DEFAULT_MSG ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIR)

  IF (ZEROMQ_FOUND)
    COLORMSG (HICYAN "ZeroMQ version: ${ZEROMQ_VERSION}")

    # Set flags and print a status message
    SET (ZEROMQ_CPPFLAGS "-I${ZEROMQ_INCLUDE_DIR}")
    SET (ZEROMQ_LDFLAGS "${ZEROMQ_LIBRARIES}")

    # add compiler definition so we can access ZEROMQ_FOUND
    ADD_DEFINITIONS ("-DZEROMQ_FOUND")  

    MESSAGE (STATUS "  * includes: ${ZEROMQ_INCLUDE_DIR}")
    MESSAGE (STATUS "  * libs:     ${ZEROMQ_LIBRARIES}")
  ELSE (ZEROMQ_FOUND)
    MESSAGE (STATUS "ZeroMQ not found")
  ENDIF (ZEROMQ_FOUND)

ENDIF (NOT ZEROMQ_FOUND)

