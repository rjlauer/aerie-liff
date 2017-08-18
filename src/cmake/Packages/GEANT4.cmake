################################################################################
# Module to find GEANT4                                                        #
#                                                                              #
# This sets the following variables:                                           #
#   - GEANT4_CPPFLAGS                                                          #
#   - GEANT4_LDFLAGS                                                           #
#   - GEANT4_LIBRARIES                                                         #
#   - GEANT4_FOUND                                                             #
#   - G4LIB                                                                    #
#   - G4INCLUDE                                                                #
#   - G4INSTALL                                                                #
################################################################################

#SET (GEANT4_FIND_QUIETLY TRUE)
# Set not required
SET (GEANT4_FIND_REQUIRED FALSE)

# If GEANT4 was found already, don't find it again.

# This works because GEANT4_FOUND is not cached
IF (NOT GEANT4_FOUND)

  set (GEANT4_FOUND TRUE)

  IF (NOT G4SYSTEM)
    SET (TMP $ENV{G4SYSTEM})
    IF (TMP)
      SET (G4SYSTEM $ENV{G4SYSTEM})
    ELSE (TMP)
      set (GEANT4_FOUND FALSE)
      MESSAGE (STATUS "FAILED to set G4SYSTEM: $ENV{G4SYSTEM}")
    ENDIF (TMP)
  ENDIF (NOT G4SYSTEM)


  IF (NOT G4INSTALL)
    SET (TMP $ENV{G4INSTALL})
    IF (TMP)
      SET (G4INSTALL $ENV{G4INSTALL})
    ELSE (TMP)
      set (GEANT4_FOUND FALSE)
      MESSAGE (STATUS "FAILED to set G4INSTALL: $ENV{G4INSTALL}")
    ENDIF (TMP)
  ENDIF (NOT G4INSTALL)

  IF (NOT G4INCLUDE)
    SET (TMP $ENV{G4INCLUDE})
    IF (TMP)
      SET (G4INCLUDE $ENV{G4INCLUDE})
    ELSE (TMP)
      set (GEANT4_FOUND FALSE)
      MESSAGE (STATUS "FAILED to set G4INCLUDE: $ENV{G4INCLUDE}")
    ENDIF (TMP)
  ENDIF (NOT G4INCLUDE)


  IF (NOT G4LIB)
    SET (TMP $ENV{G4LIB})
    IF (TMP)
      string(REPLACE "/${G4SYSTEM}" "" G4LIB ${TMP})
    ELSE (TMP)
      set (GEANT4_FOUND FALSE)
      MESSAGE (STATUS "FAILED to set G4LIB: $ENV{G4LIB}")
    ENDIF (TMP)
  ENDIF (NOT G4LIB)


  IF (GEANT4_FOUND)
    # Get the GEANT4 Version
    EXECUTE_PROCESS (COMMAND ${G4INSTALL}/bin/geant4-config --version
      OUTPUT_VARIABLE GEANT4_VERSION)
    STRING (REGEX REPLACE "[ \t\r\n]+" "" GEANT4_VERSION ${GEANT4_VERSION})


    # --------------------------------------------------------------------------
    # Set GEANT4 ENV stuff
    # --------------------------------------------------------------------------
    set(G4UI_USE_TCSH $ENV{G4UI_USE_TCSH})
    if(NOT G4UI_USE_TCSH)
      set(ENV{G4UI_NONE} 1)
    endif(NOT G4UI_USE_TCSH)

    # --------------------------------------------------------------------------
    # Find GEANT4 libraries.
    # --------------------------------------------------------------------------
    FILE(GLOB G4LIBS ${G4LIB}/*.so ${G4LIB}/*.a ${G4LIB}/*.dylib)
    FOREACH (_file ${G4LIBS})
      string(REGEX REPLACE ".*/lib([^/]+)\\.[^\\.]+$" "\\1" newlib ${_file})
      FIND_LIBRARY (libtest ${newlib} ${G4LIB} NO_DEFAULT_PATH)
      if (libtest)
        LIST (APPEND GEANT4_LIBRARIES ${newlib})
        #MESSAGE (STATUS "Adding: ${newlib}")
      endif(libtest)
    ENDFOREACH (_file)


    LINK_DIRECTORIES (${G4LIB} )
    INCLUDE_DIRECTORIES (${G4INCLUDE} )

    SET (_DIRECTORIES)

    IF (NOT DL_LIBRARY)
      FIND_LIBRARY (DL_LIBRARY dl)
      MARK_AS_ADVANCED (DL_LIBRARY)
    ENDIF (NOT DL_LIBRARY)

    SET (GEANT4_LIBRARIES ${GEANT4_LIBRARIES} ${DL_LIBRARY})
    IF (NOT GEANT4_FIND_QUIETLY)
      COLORMSG (HICYAN "GEANT4 version: ${GEANT4_VERSION}")
      MESSAGE (STATUS "  * includes: ${G4INCLUDE}")
      MESSAGE (STATUS "  * libs:     ${G4LIB}")
      MESSAGE (STATUS "  * system:   ${G4SYSTEM}")
    ENDIF (NOT GEANT4_FIND_QUIETLY)

  ELSE (GEANT4_FOUND)
    IF (GEANT4_FIND_REQUIRED)
      MESSAGE (FATAL_ERROR "Could not find GEANT4")
      MESSAGE (STATUS "  * includes: $ENV{G4INCLUDE}")
      MESSAGE (STATUS "  * libs:     $ENV{G4LIB}")
      MESSAGE (STATUS "  * system:   $ENV{G4SYSTEM}")
    else (GEANT4_FIND_REQUIRED)
      if (NOT GEANT4_FIND_QUIETLY)
        MESSAGE (STATUS "Could not find GEANT4")
        MESSAGE (STATUS "  * includes: $ENV{G4INCLUDE}")
        MESSAGE (STATUS "  * libs:     $ENV{G4LIB}")
        MESSAGE (STATUS "  * system:   $ENV{G4SYSTEM}")
      endif (NOT GEANT4_FIND_QUIETLY)
    ENDIF (GEANT4_FIND_REQUIRED)

  ENDIF (GEANT4_FOUND)
ENDIF (NOT GEANT4_FOUND)
