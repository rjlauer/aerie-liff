HAWC_PROJECT (hawcnest
  DOCS_DIR docs)

ADD_DEFINITIONS(-DSHARED_LIBRARY_SUFFIX=${CMAKE_SHARED_LIBRARY_SUFFIX})

HAWC_ADD_LIBRARY (hawcnest
  HEADERS include/hawcnest/*.h
          include/hawcnest/impl/*.h
          include/hawcnest/processing/*.h
          include/hawcnest/test/*.h
          include/hawcnest/test/*.ixx
          include/hawcnest/xml/*.h
  BUILD_HEADERS include/hawcnest/config.h.in
  SOURCES src/*.cc
          src/xml/*.cc
  CONFIGS config/*.xsd
  USE_PACKAGES Python Boost XercesC)

HAWC_ADD_PYBINDINGS (hawcnest
  SOURCES src/pybindings/*.cc
  USE_PACKAGES Python Boost
  USE_PROJECTS hawcnest)

IF (ENABLE_PYTHON_BINDINGS)
  # Install __init__.py into lib/hawc, and make a soft link in the build tree
  # to enable unit testing of python modules
  INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/pybindings/__init__.py
                 ${CMAKE_CURRENT_SOURCE_DIR}/src/pybindings/load_pybindings.py
           DESTINATION lib/hawc
           PERMISSIONS OWNER_READ OWNER_WRITE
                       GROUP_READ GROUP_WRITE
                       WORLD_READ)

  ADD_CUSTOM_COMMAND (TARGET hawcnest POST_BUILD
    COMMAND mkdir -p "${CMAKE_BINARY_DIR}/libtest/hawc"
    COMMAND ln -sfn "${CMAKE_CURRENT_SOURCE_DIR}/src/pybindings/__init__.py"
                    "${CMAKE_BINARY_DIR}/libtest/hawc/__init__.py")

  # Special case: HAWCNest.py has to go into the python lib directory.  Make a
  # soft link in the build tree to enable unit testing of python modules
  INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/python/HAWCNest.py DESTINATION lib
           PERMISSIONS OWNER_READ OWNER_WRITE
                       GROUP_READ GROUP_WRITE
                       WORLD_READ)

  ADD_CUSTOM_COMMAND (TARGET hawcnest POST_BUILD
    COMMAND mkdir -p "${CMAKE_BINARY_DIR}/libtest"
    COMMAND ln -sfn "${CMAKE_CURRENT_SOURCE_DIR}/python/HAWCNest.py"
                    "${CMAKE_BINARY_DIR}/libtest/HAWCNest.py")

ENDIF (ENABLE_PYTHON_BINDINGS)

# Old-style I3Test: does not work with boost > 1.45
SET (_boost_version_number
  "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")

# New-style boost unit test, integrated into CTest framework
HAWC_ADD_TEST (framework
  SOURCES src/test/*.cc
  SCRIPTS src/test/*.py
  USE_PROJECTS hawcnest
  USE_PACKAGES Boost Root
  NO_PREFIX)

