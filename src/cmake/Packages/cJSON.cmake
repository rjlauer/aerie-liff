################################################################################
# Module to find the cJSON headers and libraries                               #
#                                                                              #
# This module defines:                                                         #
#                                                                              #
#   CJSON_FOUND                                                                #
#   CJSON_LIBRARIES                                                            #
#   CJSON_INCLUDE_DIR                                                          #
#   CJSON_CPPFLAGS                                                             #
#   CJSON_LDFLAGS                                                              #
################################################################################

SET (CJSON_FIND_QUIETLY TRUE)
SET (CJSON_FIND_REQUIRED FALSE)

IF (NOT CJSON_FOUND)

  SET (CJSONROOT $ENV{CJSONROOT})

  IF (NOT CJSONROOT)
    SET (CJSONROOT "/usr/local/cJSON")
  ENDIF (NOT CJSONROOT)
  
  # Search user environment for headers, then default paths
  FIND_PATH (CJSON_INCLUDE_DIR cJSON.h
    PATHS ${CJSONROOT}/include
    NO_DEFAULT_PATH)

  FIND_PATH (CJSON_INCLUDE_DIR cJSON.h)
  GET_FILENAME_COMPONENT (CJSONROOT ${CJSON_INCLUDE_DIR} PATH)

  # Search user environment for libraries, then default paths
  FIND_LIBRARY (CJSON_LIBRARIES NAMES cJSON
    PATHS ${CJSONROOT}/lib
    NO_DEFAULT_PATH)
  FIND_LIBRARY (CJSON_LIBRARIES NAMES cJSON)

  # Set CJSON_FOUND and error out if cJSON is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (CJSON
    DEFAULT_MSG CJSON_LIBRARIES CJSON_INCLUDE_DIR)

  IF (CJSON_FOUND)
    COLORMSG (HICYAN "cJSON found")

    # Check to see if functions are prefixed with cJSON_
#    INCLUDE (CheckLibraryExists)
#    CHECK_LIBRARY_EXISTS (${CJSON_LIBRARIES} cJSON_Parse "" CJSON_LIBRARY_EXISTS)

    # Set flags and print a status message
    SET (CJSON_CPPFLAGS "-I${CJSON_INCLUDE_DIR}")
    SET (CJSON_LDFLAGS "${CJSON_LIBRARIES}")
    
    MESSAGE (STATUS " * includes: ${CJSON_INCLUDE_DIR}")
    MESSAGE (STATUS " * libs:     ${CJSON_LIBRARIES}")
#    MESSAGE (STATUS " * CJSON_FOUND          : ${CJSON_FOUND}")
#    MESSAGE (STATUS " * CJSON_LIBRARIES      : ${CJSON_LIBRARIES}")
#    MESSAGE (STATUS " * CJSON_INCLUDE_DIR    : ${CJSON_INCLUDE_DIR}")
#    MESSAGE (STATUS " * CJSON_CPPFLAGS       : ${CJSON_CPPFLAGS}")
#    MESSAGE (STATUS " * CJSON_LDFLAGS        : ${CJSON_LDFLAGS}")
#    MESSAGE (STATUS " * CJSON_LIBRARY_EXISTS : ${CJSON_LIBRARY_EXISTS}")
    IF (CJSON_LIBRARY_EXISTS)
      MESSAGE (STATUS " * confirm:  Library functions exist")
    ENDIF (CJSON_LIBRARY_EXISTS)
  ELSE (CJSON_FOUND)
    MESSAGE (STATUS "cJSON not found")
  ENDIF (CJSON_FOUND)

ENDIF (NOT CJSON_FOUND)

