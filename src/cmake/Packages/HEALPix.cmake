################################################################################
# Module to find HEALPix-cxx                                                   #
#                                                                              #
#   HEALPIX_FOUND                                                              #
#   HEALPIX_LIBRARIES                                                          #
#   HEALPIX_INCLUDE_DIR                                                        #
#   HEALPIX_BIN_DIR                                                            #
#   HEALPIX_LIB_DIR                                                            #
#   HEALPIX_CPPFLAGS                                                           #
#   HEALPIX_LDFLAGS                                                            #
################################################################################

SET (HEALPix_FIND_QUIETLY TRUE)
SET (HEALPix_FIND_REQUIRED FALSE)

IF (NOT HEALPIX_FOUND)

  # Search user environment for healpix-cxx config script; then default paths
  FIND_PATH (HEALPIX_BIN_DIR healpix-cxx-config PATHS $ENV{HEALPIXROOT}/bin
    NO_DEFAULT_PATH)
  FIND_PATH (HEALPIX_BIN_DIR healpix-cxx-config)

  # Set the HEALPIXROOT variable
  GET_FILENAME_COMPONENT (HEALPIXROOT ${HEALPIX_BIN_DIR} PATH)

  IF (NOT ENV{HEALPIXROOT})
    SET (ENV{HEALPIXROOT} ${HEALPIXROOT})
  ENDIF (NOT ENV{HEALPIXROOT})

  IF (HEALPIXROOT)
    # Get version, with a simplified version for preprocessor definitions
    EXECUTE_PROCESS (COMMAND ${HEALPIX_BIN_DIR}/healpix-cxx-config --version
      OUTPUT_VARIABLE HEALPIX_VERSION)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" HEALPIX_VERSION ${HEALPIX_VERSION})

    # Strip out the "." and any patch version information in the version string
    STRING (REGEX REPLACE "\\." "" HPX_VERSION_DEF ${HEALPIX_VERSION})
    STRING (REGEX REPLACE "[-]+[a-z]+" "" HPX_VERSION_DEF ${HPX_VERSION_DEF})
    ADD_DEFINITIONS ("-DHEALPIX_VERSION=${HPX_VERSION_DEF}")

    # Get compiler flags
    EXECUTE_PROCESS (COMMAND ${HEALPIX_BIN_DIR}/healpix-cxx-config --cppflags
      OUTPUT_VARIABLE HEALPIX_CPPFLAGS)
    STRING (REGEX REPLACE "[ \t\r\n]+" " " HEALPIX_CPPFLAGS ${HEALPIX_CPPFLAGS})
    SET (HEALPIX_CPPFLAGS
      "${HEALPIX_CPPFLAGS} -DHEALPIX_VERSION=${HPX_VERSION_DEF}")

    # Get linker flags
    EXECUTE_PROCESS (COMMAND ${HEALPIX_BIN_DIR}/healpix-cxx-config --ldflags
      OUTPUT_VARIABLE HEALPIX_LDFLAGS)
    STRING (REGEX REPLACE "[ \t\r\n]+" " " HEALPIX_LDFLAGS ${HEALPIX_LDFLAGS})

    # Get library directory and search for HEALPix libraries:
    # fftpack, cxxsupport, and healpix_cxx
    EXECUTE_PROCESS (COMMAND
      ${HEALPIX_BIN_DIR}/healpix-cxx-config --ld_library_path
      OUTPUT_VARIABLE HEALPIX_LIB_DIR)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" HEALPIX_LIB_DIR ${HEALPIX_LIB_DIR})

    SET (_lib_names "fftpack;cxxsupport;healpix_cxx")
    FOREACH (_lib ${_lib_names})
      FIND_LIBRARY (_LIBY${_lib} ${_lib} ${HEALPIX_LIB_DIR} NO_DEFAULT_PATH)
      IF (_LIBY${_lib})
        LIST (APPEND HEALPIX_LIBRARIES ${_LIBY${_lib}})
      ENDIF (_LIBY${_lib})
      SET (_LIBY${_lib} ${_LIBY${_lib}} CACHE INTERNAL "" FORCE)
    ENDFOREACH (_lib)

    # Set include directories: fftpack, cxxsupport, healpix
    SET (_include_dirs "fftpack;cxxsupport;healpix")
    SET (HEALPIX_INCLUDE_DIR "$ENV{HEALPIXROOT}/include")
    FOREACH (_dir ${_include_dirs})
      LIST (APPEND HEALPIX_INCLUDE_DIR
        "$ENV{HEALPIXROOT}/include/healpix-cxx/${_dir}")
    ENDFOREACH (_dir)

  ENDIF (HEALPIXROOT)

  ## Check for HEALPIX_FOUND; error out if HEALPix is required
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (HEALPix
    DEFAULT_MSG HEALPIX_LIBRARIES HEALPIX_INCLUDE_DIR)

  IF (HEALPIX_FOUND)
    COLORMSG (HICYAN "HEALPix version ${HEALPIX_VERSION} found:")
    MESSAGE (STATUS "  * bin:  ${HEALPIX_BIN_DIR}")
    MESSAGE (STATUS "  * libs: ${HEALPIX_LIB_DIR}")
  ENDIF (HEALPIX_FOUND)

ENDIF (NOT HEALPIX_FOUND)

