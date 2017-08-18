
SET (SPHINX_EXTRA_OPTIONS "-N;-a;-E" CACHE STRING "Sphinx options")

FIND_PROGRAM (SPHINX_BUILD sphinx-build)

IF (SPHINX_BUILD)
  COLORMSG (HICYAN "Sphinx found, building sphinx documentation")
  SET (SPHINX_PAPER_SIZE "letter" CACHE STRING "Size of paper when generating LaTeX/PDF of documentation.  Options are 'letter' or 'a4'")
  SET (SPHINX_EXTRA_OPTIONS "" CACHE STRING "Extra options to pass to sphinx")
ELSE ()
  COLORMSG (HIMAGENTA "+ sphinx-build NOT found, disabling sphinx documentation")
ENDIF ()

