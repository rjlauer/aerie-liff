HAWC_PROJECT (data-structures
  DOCS_DIR docs)

IF (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/scaler-classes)
  MESSAGE (STATUS "     Project 'scaler-classes' does not exist.")
  MESSAGE (STATUS "     Scaler I/O sources will not be compiled.")

  SET (SCALER_CLASSES_NOT_FOUND 1)
  ADD_DEFINITIONS ("-DSCALER_CLASSES_NOT_FOUND=1")
  SET (PROJECTS "")
ELSE (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/scaler-classes)
  SET (PROJECTS scaler-classes)
ENDIF (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/scaler-classes)

HAWC_ADD_LIBRARY (data-structures
  HEADERS include/data-structures/*.h
          include/data-structures/astronomy/*.h
          include/data-structures/detector/*.h
          include/data-structures/event/*.h
          include/data-structures/geometry/*.h
          include/data-structures/hardware/*.h
          include/data-structures/hardware/caen/*.h
          include/data-structures/io/*.h
          include/data-structures/iterator/*.h
          include/data-structures/math/*.h
          include/data-structures/online/*.h
          include/data-structures/physics/*.h
          include/data-structures/reconstruction/*.h
          include/data-structures/reconstruction/*/*.h
          include/data-structures/time/*.h
  SOURCES src/time/*.cc
          src/geometry/*.cc
          src/astronomy/*.cc
          src/detector/*.cc
          src/event/*.cc
          src/hardware/caen/*.cc
          src/math/*.cc
          src/physics/*.cc
          src/reconstruction/*/*.cc
  USE_PROJECTS hawcnest ${PROJECTS}
  USE_PACKAGES Boost GSL PhotoSpline Root)

HAWC_ADD_PYBINDINGS (data_structures
  SOURCES src/pybindings/*.cc
  USE_PACKAGES Python Boost
  USE_PROJECTS hawcnest data-structures)

HAWC_ADD_EXECUTABLE (special-functions
  SOURCES examples/math/sf.cc
  USE_PROJECTS hawcnest data-structures)

HAWC_ADD_EXECUTABLE (integrator
  SOURCES examples/math/integrator.cc
  USE_PROJECTS hawcnest data-structures)

HAWC_ADD_EXECUTABLE (bayes
  SOURCES examples/math/bayes.cc
  USE_PROJECTS hawcnest rng-service data-structures)

HAWC_ADD_EXECUTABLE (bayes-root
  SOURCES examples/math/bayes-root.cc
  USE_PACKAGES Root
  USE_PROJECTS hawcnest rng-service data-structures)

HAWC_ADD_EXECUTABLE (bayes-sigma
  SOURCES examples/math/bayes-sigma.cc
  USE_PACKAGES Root
  USE_PROJECTS hawcnest rng-service data-structures)

HAWC_ADD_EXECUTABLE (bayes-sigma-trials
  SOURCES examples/math/bayes-sigma-trials.cc
  USE_PACKAGES Root
  USE_PROJECTS hawcnest rng-service data-structures)

HAWC_ADD_EXECUTABLE (bayes-test
  SOURCES examples/math/bayes-test.cc
  USE_PACKAGES Root
  USE_PROJECTS hawcnest rng-service data-structures)

HAWC_ADD_EXECUTABLE (bayes-test-quick
  SOURCES examples/math/bayes-test-quick.cc
  USE_PACKAGES Root
  USE_PROJECTS hawcnest rng-service data-structures)

HAWC_ADD_EXECUTABLE (bayes-prior
  SOURCES examples/math/bayes-prior.cc
  USE_PACKAGES Root
  USE_PROJECTS hawcnest rng-service data-structures)

HAWC_ADD_EXECUTABLE (bayes-source
  SOURCES examples/math/bayes-source.cc
  USE_PACKAGES Root
  USE_PROJECTS hawcnest rng-service data-structures)

HAWC_ADD_EXECUTABLE (leaps
  SOURCES examples/time/leaps.cc
  USE_PROJECTS hawcnest data-structures)

HAWC_ADD_TEST (data-structures
  SOURCES src/test/*.cc
  SCRIPTS src/test/*.py
  USE_PROJECTS hawcnest data-structures
  USE_PACKAGES Boost
  NO_PREFIX)

