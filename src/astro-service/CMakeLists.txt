HAWC_PROJECT (astro-service
  DOCS_DIR docs)

# Build the IGRF model using the Geopack Fortran source (default=OFF)
OPTION (BUILD_IGRF_MODEL "Build in geomagnetic field model" OFF)

IF (BUILD_IGRF_MODEL)
  # If Fortran exists, build in support for IGRF model
  CHECK_LANGUAGE_WORKS (Fortran Fortran_LANGUAGE_WORKS)
  IF (DEFINED Fortran_LANGUAGE_WORKS AND Fortran_LANGUAGE_WORKS)
    ENABLE_LANGUAGE (Fortran)
    ADD_DEFINITIONS (-DCOMPILE_FORTRAN_COMPONENTS)
    NO_DOTFILE_GLOB (AS_SOURCES src/*.cc src/*.f)
    MESSAGE (STATUS "    + Fortran found.  Will build IGRF model.")
  # Otherwise, ignore the IGRF model
  ELSE ()
    MESSAGE (STATUS "    - Fortran not found.  Will NOT build IGRF model.")
    NO_DOTFILE_GLOB (AS_SOURCES src/*.cc)
  ENDIF ()
ELSE ()
  NO_DOTFILE_GLOB (AS_SOURCES src/*.cc)
ENDIF ()

HAWC_ADD_LIBRARY (astro-service
  HEADERS include/astro-service/*.h
  include/astro-service/catalogs/*.h
  SOURCES ${AS_SOURCES}
  src/catalogs/*.cc
  src/catalogs/*.c
  src/catalogs/*.cpp
  USE_PROJECTS hawcnest data-structures
  USE_PACKAGES HEALPix)

HAWC_ADD_PYBINDINGS (astro_service
  SOURCES src/pybindings/*.cc
  USE_PACKAGES Python Boost
  USE_PROJECTS hawcnest data-structures astro-service)

HAWC_ADD_EXECUTABLE (moonsight
  SOURCES examples/moonsight.cc
  USE_PROJECTS hawcnest data-structures astro-service
  NO_PREFIX)

HAWC_ADD_EXECUTABLE (source-sight
  SOURCES examples/source-sight.cc
  USE_PROJECTS hawcnest data-structures astro-service
  NO_PREFIX)

HAWC_ADD_EXECUTABLE (drawBField
  SOURCES examples/drawBField.cc
  USE_PROJECTS hawcnest data-structures astro-service
  USE_PACKAGES Root
  NO_PREFIX)

HAWC_ADD_TEST (astro-service
  SOURCES src/test/*.cc
  SCRIPTS src/test/*.py
  USE_PROJECTS hawcnest data-structures astro-service
  USE_PACKAGES Boost
  NO_PREFIX)
