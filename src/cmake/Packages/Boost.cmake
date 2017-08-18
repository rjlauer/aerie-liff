################################################################################
# Module to set up Boost                                                       #
#                                                                              #
# Calls the CMake FindBoost module and sets up the following variables (in     #
# addition to the Boost_* variables set up by CMake)                           #
#                                                                              #
#    BOOST_FOUND                                                               #
#    BOOST_INCLUDE_DIR                                                         #
#    BOOST_LIBRARIES                                                           #
#    BOOST_LIB_DIR                                                             #
#    BOOST_CPPFLAGS                                                            #
#    BOOST_LDFLAGS                                                             #
#                                                                              #
################################################################################

SET (Boost_USE_STATIC_LIBS OFF)
IF (CMAKE_VERSION)                      # Catch CMake > 2.6.2
  SET (Boost_USE_MULTITHREADED OFF)     # Variable name in CMake > 2.6.2
ELSE (CMAKE_VERSION)
  SET (Boost_USE_MULTITHREAD OFF)       # Variable name in CMake < 2.6.4
ENDIF (CMAKE_VERSION)
SET (Boost_ADDITIONAL_VERSIONS "1.45.0")

# List the Boost libraries we want to pull in
SET (LIB_COMPONENTS filesystem
                    iostreams
                    program_options
                    python
                    random
                    regex
                    system
                    thread
                    unit_test_framework)

IF (NOT BOOST_FOUND)
  FIND_PACKAGE (Boost 1.40 REQUIRED QUIET COMPONENTS ${LIB_COMPONENTS})

  IF (NOT Boost_FOUND)
    MESSAGE (FATAL_ERROR "Boost not found")
  ENDIF (NOT Boost_FOUND)

  # Set up package variables
  SET (BOOST_FOUND TRUE)
  SET (BOOST_INCLUDE_DIR ${Boost_INCLUDE_DIR})
  SET (BOOST_LIBRARIES ${Boost_LIBRARIES})
  SET (BOOST_LIB_DIR ${Boost_LIBRARY_DIRS})
  SET (BOOST_CPPFLAGS "-I${Boost_INCLUDE_DIR}")
  FOREACH (_lib ${BOOST_LIBRARIES})
    SET (BOOST_LDFLAGS "${BOOST_LDFLAGS} ${_lib}")
  ENDFOREACH (_lib ${BOOST_LIBRARIES})

  # List found components
  IF (CMAKE_VERSION)                      # Catch CMake > 2.6.2
    FOREACH (_comp ${LIB_COMPONENTS})
      STRING (TOUPPER "${_comp}" _COMP)
      IF (Boost_${_COMP}_FOUND)
        LIST (APPEND BOOST_FIND_COMPONENTS ${_comp})
      ENDIF (Boost_${_COMP}_FOUND)
    ENDFOREACH (_comp ${LIB_COMPONENTS})
  ELSE (CMAKE_VERSION)
    SET (BOOST_FIND_COMPONENTS ${Boost_FIND_COMPONENTS})
  ENDIF (CMAKE_VERSION)

  INCLUDE_DIRECTORIES (${BOOST_INCLUDE_DIR})

  COLORMSG (HICYAN "Boost version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
  MESSAGE (STATUS "  * includes:   ${BOOST_INCLUDE_DIR}")
  MESSAGE (STATUS "  * components: ${BOOST_FIND_COMPONENTS}")

ENDIF (NOT BOOST_FOUND)

