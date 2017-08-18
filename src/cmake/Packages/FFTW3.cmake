################################################################################
# Module to find fftw3                                                         #
#                                                                              #
# This module will find the FFTW3 discrete Fourier transform C library, and in #
# the process define the variables:                                            #
#                                                                              #
#   FFTW3_FOUND                                                                #
#   FFTW3_LIBRARIES                                                            #
#   FFTW3_LIB_DIR                                                              #
#   FFTW3_INCLUDE_DIR                                                          #
#   FFTW3_CPPFLAGS                                                             #
#   FFTW3_LDFLAGS                                                              #
################################################################################

SET (FFTW3_FIND_QUIETLY TRUE)
SET (FFTW3_FIND_REQUIRED FALSE)

IF (NOT FFTW3_FOUND)

  # Search user environment for headers, then default paths
  FIND_PATH (FFTW3_INCLUDE_DIR fftw3.h
    PATHS $ENV{FFTW3ROOT}/include
    NO_DEFAULT_PATH)
  FIND_PATH (FFTW3_INCLUDE_DIR fftw3.h)
  GET_FILENAME_COMPONENT (FFTW3ROOT ${FFTW3_INCLUDE_DIR} PATH)

  # Search user environment for libraries, then default paths
  FIND_LIBRARY (FFTW3_LIBRARIES NAMES fftw3
    PATHS $ENV{FFTW3ROOT}/lib
    NO_DEFAULT_PATH)
  FIND_LIBRARY (FFTW3_LIBRARIES NAMES fftw3)
  GET_FILENAME_COMPONENT (FFTW3_LIB_DIR ${FFTW3_LIBRARIES} PATH)

  # Set FFTW3_FOUND and error out if fftw3 is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (FFTW3
    DEFAULT_MSG FFTW3_LIBRARIES FFTW3_INCLUDE_DIR)

  IF (FFTW3_FOUND)
    COLORMSG (HICYAN "FFTW3 found")

    # Set flags and print a status message
    SET (FFTW3_CPPFLAGS "-I${FFTW3_INCLUDE_DIR}")
    SET (FFTW3_LDFLAGS "${FFTW3_LIBRARIES}")
    
    MESSAGE (STATUS "  * includes: ${FFTW3_INCLUDE_DIR}")
    MESSAGE (STATUS "  * libs:     ${FFTW3_LIBRARIES}")
  ENDIF (FFTW3_FOUND)

ENDIF (NOT FFTW3_FOUND)

