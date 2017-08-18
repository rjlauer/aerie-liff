
SET (SPHINX_EXTRA_OPTIONS "-N;-a;-E" CACHE STRING "Sphinx options")

FIND_PROGRAM (SPHINX_BUILD sphinx-build)

IF (SPHINX_BUILD)
  COLORMSG (GREEN "+ sphinx-build found, building sphinx documentation")
ELSE ()
  COLORMSG (MAGENTA "+ sphinx-build NOT found, disabling sphinx documentation")
ENDIF ()
