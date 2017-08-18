################################################################################
# Module to find XCDF                                                          #
#                                                                              #
# This module defines:                                                         #
#                                                                              #
#   XCDF_FOUND                                                                 #
#   XCDF_LIBRARIES                                                             #
#   XCDF_INCLUDE_DIR                                                           #
#   XCDF_LIB_DIR                                                               #
#   XCDF_CPPFLAGS                                                              #
#   XCDF_LDFLAGS                                                               #
#   XCDF_FULL_VERSION                                                          #
################################################################################

SET (XCDF_FIND_QUIETLY TRUE)
SET (XCDF_FIND_REQUIRED FALSE)

IF (NOT XCDF_FOUND)

  # Search user environment for XCDF headers; then default paths
  FIND_PATH (XCDF_INCLUDE_DIR xcdf/XCDF.h
    PATHS $ENV{XCDFROOT}/include
    NO_DEFAULT_PATH)
  FIND_PATH (XCDF_INCLUDE_DIR xcdf/XCDF.h)

  FIND_FILE (XCDF_CONFIG_HEADER xcdf/config.h
    PATHS $ENV{XCDFROOT}/include
    NO_DEFAULT_PATH)
  FIND_FILE (XCDF_CONFIG_HEADER xcdf/config.h)

  # Set the XCDFROOT variable
  GET_FILENAME_COMPONENT (XCDFROOT ${XCDF_INCLUDE_DIR} PATH)

  IF (NOT ENV{XCDFROOT})
    SET (ENV{XCDFROOT} ${XCDFROOT})
  ENDIF (NOT ENV{XCDFROOT})

  IF (XCDFROOT)
    SET (XCDF_VERSION_DEF "1")

    # Extract XCDF version from config.h header if possible
    IF (XCDF_CONFIG_HEADER)
      FILE (READ "${XCDF_CONFIG_HEADER}" _config_h)
      STRING(REGEX REPLACE ";" "\\\\;" _config_h "${_config_h}")
      STRING(REGEX REPLACE "\n" ";" _config_h "${_config_h}")

      FOREACH (_line ${_config_h})
        # Get major version string
        IF (${_line} MATCHES "define XCDF_MAJOR")
          STRING (REGEX REPLACE "#define XCDF_MAJOR_VERSION" "" _major ${_line})
          STRING (STRIP "${_major}" XCDF_VERSION_DEF)
        ENDIF ()
        # Get minor version string
        IF (${_line} MATCHES "define XCDF_MINOR")
          STRING (REGEX REPLACE "#define XCDF_MINOR_VERSION" "" _minor ${_line})
          STRING (STRIP "${_minor}" _minor)
          SET (XCDF_VERSION_DEF "${XCDF_VERSION_DEF}${_minor}")
        ENDIF ()
        # Get patch version string
        IF (${_line} MATCHES "define XCDF_PATCH")
          STRING (REGEX REPLACE "#define XCDF_PATCH_VERSION" "" _patch ${_line})
          STRING (STRIP "${_patch}" _patch)
          SET (XCDF_VERSION_DEF "${XCDF_VERSION_DEF}${_patch}")
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()

    # Add XCDF version string to compiler definitions
    ADD_DEFINITIONS ("-DXCDF_FULL_VERSION=${XCDF_VERSION_DEF}")

    # Search user environment for libraries, then default paths
    SET (XCDF_LIBRARIES)
    FOREACH (_lib "xcdf")
      FIND_LIBRARY (my_${_lib}
        NAMES ${_lib}
        PATHS $ENV{XCDFROOT}/lib
        NO_DEFAULT_PATH)
      FIND_LIBRARY (my_${_lib}
        NAMES ${_lib})
      IF (NOT "${my_${_lib}}" MATCHES ".*NOTFOUND$")
        LIST (APPEND XCDF_LIBRARIES ${my_${_lib}})
      ENDIF ()
    ENDFOREACH ()

    # Set the bin and lib directories
    SET (XCDF_BIN_DIR ${XCDFROOT}/bin)
    SET (XCDF_LIB_DIR ${XCDFROOT}/lib)

  ENDIF (XCDFROOT)

  # Set XCDF_FOUND and error out if cfitsio is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (XCDF
    DEFAULT_MSG XCDF_LIBRARIES XCDF_INCLUDE_DIR)
  ADD_DEFINITIONS ("-I${XCDF_INCLUDE_DIR}")

  IF (XCDF_FOUND)
    # Set flags and print a status message
    COLORMSG (HICYAN "XCDF found:")

    SET (XCDF_CPPFLAGS "-I${XCDF_INCLUDE_DIR}")
    SET (XCDF_LDFLAGS "${XCDF_LIBRARIES}")

    MESSAGE (STATUS "  * includes: ${XCDF_INCLUDE_DIR}")
    MESSAGE (STATUS "  * libs:     ${XCDF_LIBRARIES}")
  ENDIF (XCDF_FOUND)

ENDIF (NOT XCDF_FOUND)

