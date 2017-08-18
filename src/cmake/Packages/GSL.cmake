################################################################################
# Module to find the Gnu Scientific Library (GSL)                              #
#                                                                              #
#   GSL_FOUND                                                                  #
#   GSL_LIBRARIES                                                              #
#   GSL_INCLUDE_DIR                                                            #
#   GSL_BIN_DIR                                                                #
#   GSL_LIB_DIR                                                                #
#   GSL_CPPFLAGS                                                               #
#   GSL_LDFLAGS                                                                #
################################################################################

SET (GSL_FIND_QUIETLY TRUE)
SET (GSL_FIND_REQUIRED TRUE)

IF (NOT GSL_FOUND)

  # Search user environment for gsl-config script; then default paths
  FIND_PATH (GSL_BIN_DIR gsl-config PATHS $ENV{GSLROOT}/bin
    NO_DEFAULT_PATH)
  FIND_PATH (GSL_BIN_DIR gsl-config)

  # Set the GSLROOT variable
  GET_FILENAME_COMPONENT (GSLROOT ${GSL_BIN_DIR} PATH)

  IF (NOT ENV{GSLROOT})
    SET (ENV{GSLROOT} ${GSLROOT})
  ENDIF (NOT ENV{GSLROOT})

  IF (GSLROOT)
    # Get version, with a simplified version for preprocessor definitions
    EXECUTE_PROCESS (COMMAND ${GSL_BIN_DIR}/gsl-config --version
      OUTPUT_VARIABLE GSL_VERSION)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" GSL_VERSION ${GSL_VERSION})

    # Strip out the "." and any patch version information in the version string
    STRING (REGEX REPLACE "\\." "" GSL_VERSION_DEF ${GSL_VERSION})
    STRING (REGEX REPLACE "[-]+[a-z]+" "" GSL_VERSION_DEF ${GSL_VERSION_DEF})
    ADD_DEFINITIONS ("-DHAVE_GSL -DGSL_VERSION=${GSL_VERSION_DEF}")

    # Get compiler flags
    EXECUTE_PROCESS (COMMAND ${GSL_BIN_DIR}/gsl-config --cflags
      OUTPUT_VARIABLE GSL_CPPFLAGS)
    STRING (REGEX REPLACE "[ \t\r\n]+" " " GSL_CPPFLAGS ${GSL_CPPFLAGS})
    SET (GSL_CPPFLAGS
      "${GSL_CPPFLAGS} -DHAVE_GSL -DGSL_VERSION=${GSL_VERSION_DEF}")

    # Get linker flags
    EXECUTE_PROCESS (COMMAND ${GSL_BIN_DIR}/gsl-config --libs-without-cblas
      OUTPUT_VARIABLE GSL_LDFLAGS)
    STRING (REGEX REPLACE "[ \t\r\n]+" " " GSL_LDFLAGS ${GSL_LDFLAGS})

    # Get library directory and search for GSL libraries:
    EXECUTE_PROCESS (COMMAND
      ${GSL_BIN_DIR}/gsl-config --prefix
      OUTPUT_VARIABLE GSL_PREFIX)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" GSL_PREFIX ${GSL_PREFIX})
    SET (GSL_LIB_DIR "${GSL_PREFIX}/lib")

    SET (_lib_names "gsl" "gslcblas")
    FOREACH (_lib ${_lib_names})
      FIND_LIBRARY (_LIBY${_lib} ${_lib} ${GSL_LIB_DIR} NO_DEFAULT_PATH)
      IF (_LIBY${_lib})
        LIST (APPEND GSL_LIBRARIES ${_LIBY${_lib}})
      ENDIF (_LIBY${_lib})
      SET (_LIBY${_lib} ${_LIBY${_lib}} CACHE INTERNAL "" FORCE)
    ENDFOREACH (_lib)

    # Set include directories
    SET (GSL_INCLUDE_DIR "$ENV{GSLROOT}/include")

  ENDIF (GSLROOT)

  ## Check for GSL_FOUND; error out if GSL is required
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (GSL
    DEFAULT_MSG GSL_LIBRARIES GSL_INCLUDE_DIR)

  IF (GSL_FOUND)
    COLORMSG (HICYAN "GSL version ${GSL_VERSION} found:")
    MESSAGE (STATUS "  * bin:  ${GSL_BIN_DIR}")
    MESSAGE (STATUS "  * libs: ${GSL_LIB_DIR}")
  ENDIF (GSL_FOUND)

ENDIF (NOT GSL_FOUND)

