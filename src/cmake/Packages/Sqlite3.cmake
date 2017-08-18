#############################################################################
# Module to find Sqlite3
#                                                                           #
#   SQLITE3_FOUND                                                           #
#   SQLITE3_LIBRARIES                                                       #
#   SQLITE3_INCLUDE_DIR                                                     #
#   SQLITE3_BIN_DIR                                                         #
#   SQLITE3_LIB_DIR                                                         #
#   SQLITE3_CPPFLAGS                                                        #
#   SQLITE3_LDFLAGS                                                         #
#############################################################################

SET (SQLITE3_FIND_QUIETLY TRUE)
SET (SQLITE3_FIND_REQUIRED FALSE)

IF (NOT SQLITE3_FOUND)

  #Search user's env for headers before using the default paths
  FIND_PATH(SQLITE3_INCLUDE_DIR sqlite3.h
    PATHS $ENV{SQLITEROOT}/include
    NO_DEFAULT_PATH)
  FIND_PATH(SQLITE3_INCLUDE_DIR sqlite3.h
    PATHS
    /opt/local/include/
    /sw/include/
    /usr/local/include/
    /usr/include/
    NO_DEFAULT_PATH
    )
  GET_FILENAME_COMPONENT (SQLITEROOT ${SQLITE3_INCLUDE_DIR} PATH)

  #Serach user's env for libraries, then default paths
  FIND_LIBRARY (SQLITE3_LIBRARIES sqlite3
    PATHS $ENV{SQLITEROOT}/lib
    NO_DEFAULT_PATH)
  FIND_LIBRARY (SQLITE3_LIBRARIES sqlite3
    PATHS
    /opt/local/lib
    /sw/lib
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
    NO_DEFAULT_PATH
    )

  # Set SQLITE3_FOUND and error out if sqlite3 is not found
  INCLUDE (FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS (SQLITE3
    DEFAULT_MSG SQLITE3_LIBRARIES SQLITE3_INCLUDE_DIR)

  IF (SQLITE3_FOUND)
    COLORMSG (HICYAN "Sqlite3 found")
    #MESSAGE (STATUS "Sqlite3 found")

    # Set flags and print a status message
    SET (SQLITE3_CPPFLAGS "-I${SQLITE3_INCLUDE_DIR}")
    SET (SQLITE3_LDFLAGS "${SQLITE3_LIBRARIES}")

    MESSAGE (STATUS "  * includes: ${SQLITE3_INCLUDE_DIR}")
    MESSAGE (STATUS "  * libs:     ${SQLITE3_LIBRARIES}")
  ELSE (SQLITE3_FOUND)
    MESSAGE (STATUS "Sqlite3 not found")
  ENDIF (SQLITE3_FOUND)

ENDIF (NOT SQLITE3_FOUND) 
