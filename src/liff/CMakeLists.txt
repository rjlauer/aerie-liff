HAWC_PROJECT(liff
        DOCS_DIR docs)

HAWC_ADD_LIBRARY(liff
#include <healpix_map_fitsio.h>
        HEADERS include/liff/*.h
        include/liff/skymaps/*.h
        SOURCES src/*.cc
        src/skymaps/*.cc
        src/*.cxx
        USE_PROJECTS hawcnest data-structures grmodel-services
        USE_PACKAGES CFITSIO HEALPix ROOT FFTW3 Boost)

HAWC_ADD_EXECUTABLE(HealpixSigFluxMap
        SOURCES examples/HealpixSigFluxMap.cc
        USE_PROJECTS hawcnest liff astro-service
        USE_PACKAGES cfitsio healpix)

HAWC_ADD_PYBINDINGS (liff_3ML
  SOURCES src/pybindings/3ML_hawc.cc
          src/pybindings/Submodule_3ML.cc
  USE_PACKAGES Python Boost CFITSIO HEALPix
  USE_PROJECTS liff)

HAWC_ADD_SCRIPTS(liff
        SCRIPTS scripts/*.py)

#HAWC_ADD_TEST (liff
#  SOURCES src/test/*.cc
#  USE_PROJECTS hawcnest data-structures liff
#  USE_PACKAGES Boost
#  NO_PREFIX)

# Install python module stuff into $HAWC_INSTALL/lib/hawc/liff.
# This is a bit of a hack; it should become part of the regular CMake build
# macros.
IF (ENABLE_PYTHON_BINDINGS)
    # Install ElectronIC.py and __init__.py into lib/hawc/liff
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/pybindings/__init__.py
            ${CMAKE_CURRENT_SOURCE_DIR}/src/pybindings/ElectronIC.py
            DESTINATION lib/hawc/liff
            PERMISSIONS OWNER_READ OWNER_WRITE
            GROUP_READ GROUP_WRITE
            WORLD_READ)
ENDIF ()
