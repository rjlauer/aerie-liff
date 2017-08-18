################################################################################
# Module to find Qt                                                            #
#                                                                              #
# This module will call the default CMake Qt modules and define:               #
#                                                                              #
#   QT_FOUND                                                                   #
#   QT_VERSION_MAJOR                                                           #
#   QT_VERSION_MINOR                                                           #
#   QT_VERSION_PATCH                                                           #
#   QT_DEFINITIONS                                                             #
#   QT_INCLUDE_DIR                                                             #
#   QT_BINARY_DIR                                                              #
#   QT_LIBRARY_DIR                                                             #
#   QT_LIBRARIES                                                               #
#                                                                              #
# as well as variables useful for calling qmake rules from within CMake.       #
################################################################################

IF (NOT QT_FOUND)

  # Search for Qt4
  FIND_PACKAGE (Qt4 4.0 COMPONENTS QtCore QtGui QtUiTools QUIET)

  # Note: Qt is optional.  Any projects that need Qt should check for the
  # QT_FOUND variable and build if it is true
  IF (QT_FOUND)
    IF (NOT QT_LIBRARIES)
      SET (QT_LIBRARIES "${QT_QTCORE_LIBRARY};${QT_QTGUI_LIBRARY};${QT_QTUITOOLS_LIBRARY}")
    ENDIF (NOT QT_LIBRARIES)

    # Set include directories to all the subdirectories in QT_INCLUDES
    FOREACH (_dir ${QT_INCLUDES})
      LIST (APPEND QT_INCLUDE_DIR ${_dir})
    ENDFOREACH (_dir)

    COLORMSG (HICYAN "Qt4 version ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH} found:")
    MESSAGE (STATUS "  * bin:  ${QT_BINARY_DIR}")
    MESSAGE (STATUS "  * libs: ${QT_LIBRARY_DIR}")
  ENDIF (QT_FOUND)

ENDIF (NOT QT_FOUND)

