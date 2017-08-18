
# For recent CMakes, the system version of CMakeParseArguments is needed to use
# the CMAKE_PARSE_ARGUMENTS macro
IF (${CMAKE_VERSION} VERSION_GREATER "2.8.3")
  INCLUDE (CMakeParseArguments)
ENDIF (${CMAKE_VERSION} VERSION_GREATER "2.8.3")

INCLUDE (CMakePadString)
INCLUDE (CMakeFilterOut)
INCLUDE (CMakeNoDotfileGlob)
INCLUDE (CMakeRepositoryRevision)
INCLUDE (CMakeColor)
INCLUDE (CMakeLanguageWorks)

