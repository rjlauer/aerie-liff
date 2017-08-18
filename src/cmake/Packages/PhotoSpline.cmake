################################################################################
# Module to find PhotoSpline                                                   #
#                                                                              #
# This module defines:                                                         #
#                                                                              #
#   PHOTOSPLINE_FOUND                                                          #
#   PHOTOSPLINE_VERSION                                                        #
#   PHOTOSPLINE_LIBRARIES                                                      #
#   PHOTOSPLINE_INCLUDE_DIR                                                    #
#   PHOTOSPLINE_LIB_DIR                                                        #
#   PHOTOSPLINE_CPPFLAGS                                                       #
#   PHOTOSPLINE_LDFLAGS                                                        #
################################################################################

SET (PHOTOSPLINE_FIND_QUIETLY TRUE)
SET (PHOTOSPLINE_FIND_REQUIRED FALSE)

IF (NOT PHOTOSPLINE_FOUND)

  # Search user environment for photospline-config script; then default paths
  FIND_PATH (PHOTOSPLINE_BIN_DIR photospline-config
    PATHS $ENV{PHOTOSPLINEROOT}/bin
    NO_DEFAULT_PATH)
  FIND_PATH (PHOTOSPLINE_BIN_DIR photospline-config)

  # Set the PHOTOSPLINEROOT variable
  GET_FILENAME_COMPONENT (PHOTOSPLINEROOT ${PHOTOSPLINE_BIN_DIR} PATH)

  IF (NOT ENV{PHOTOSPLINEROOT})
    SET (ENV{PHOTOSPLINEROOT} ${PHOTOSPLINEROOT})
  ENDIF (NOT ENV{PHOTOSPLINEROOT})

  IF (PHOTOSPLINEROOT)
    # Get version, with a simplified version for preprocessor definitions
    EXECUTE_PROCESS (COMMAND ${PHOTOSPLINE_BIN_DIR}/photospline-config --version
      OUTPUT_VARIABLE PHOTOSPLINE_VERSION)
    STRING (REGEX REPLACE "[ \t\r\n]+" ""
            PHOTOSPLINE_VERSION ${PHOTOSPLINE_VERSION})

    # Strip out the "." and any patch version information in the version string
    STRING (REGEX REPLACE "\\." ""
            PHOTOSPLINE_VERSION_DEF ${PHOTOSPLINE_VERSION})
    STRING (REGEX REPLACE "[-]+[a-z]+" ""
            PHOTOSPLINE_VERSION_DEF ${PHOTOSPLINE_VERSION_DEF})
    ADD_DEFINITIONS ("-DHAVE_PHOTOSPLINE -DPHOTOSPLINE_VERSION=${PHOTOSPLINE_VERSION_DEF}")

    # Search user environment for libraries, then default paths
    SET (PHOTOSPLINE_LIBRARIES)
    FOREACH (_lib "photospline" "spglam")
      FIND_LIBRARY (my_${_lib}
        NAMES ${_lib}
        PATHS $ENV{PHOTOSPLINEROOT}/lib
        NO_DEFAULT_PATH)
      FIND_LIBRARY (my_${_lib}
        NAMES ${_lib})
      IF (NOT "${my_${_lib}}" MATCHES ".*NOTFOUND$")
        LIST (APPEND PHOTOSPLINE_LIBRARIES ${my_${_lib}})
      ENDIF ()
    ENDFOREACH ()

    # Set the include directory
    SET (PHOTOSPLINE_INCLUDE_DIR ${PHOTOSPLINEROOT}/include)
    SET (PHOTOSPLINE_LIB_DIR ${PHOTOSPLINEROOT}/lib)

  ENDIF (PHOTOSPLINEROOT)

  # Set PHOTOSPLINE_FOUND and error out if cfitsio is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (PHOTOSPLINE
    DEFAULT_MSG PHOTOSPLINE_LIBRARIES PHOTOSPLINE_INCLUDE_DIR)
  ADD_DEFINITIONS ("-I${PHOTOSPLINE_INCLUDE_DIR}")

  IF (PHOTOSPLINE_FOUND)
    # Set flags and print a status message
    COLORMSG (HICYAN "PhotoSpline version ${PHOTOSPLINE_VERSION} found:")

    SET (PHOTOSPLINE_CPPFLAGS "-I${PHOTOSPLINE_INCLUDE_DIR}")
    SET (PHOTOSPLINE_LDFLAGS "${PHOTOSPLINE_LIBRARIES}")
    
    MESSAGE (STATUS "  * includes: ${PHOTOSPLINE_INCLUDE_DIR}")
    MESSAGE (STATUS "  * libs:     ${PHOTOSPLINE_LIBRARIES}")
  ENDIF (PHOTOSPLINE_FOUND)

ENDIF (NOT PHOTOSPLINE_FOUND)

