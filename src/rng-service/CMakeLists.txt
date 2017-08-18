HAWC_PROJECT (rng-service
  DOCS_DIR docs)

HAWC_ADD_LIBRARY (rng-service
  HEADERS include/rng-service/*.h
  SOURCES src/*.cc
  USE_PROJECTS hawcnest)

HAWC_ADD_PYBINDINGS (rng_service
  SOURCES src/pybindings/*.cc
  USE_PACKAGES Python Boost
  USE_PROJECTS hawcnest rng-service)

HAWC_ADD_EXECUTABLE (test
  SOURCES examples/test.cc
  USE_PROJECTS hawcnest rng-service)

HAWC_ADD_TEST (random-numbers
  SOURCES src/test/*.cc
  SCRIPTS src/test/*.py
  USE_PROJECTS hawcnest data-structures rng-service
  USE_PACKAGES Boost Root
  NO_PREFIX)
