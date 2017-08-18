HAWC_PROJECT (grmodel-services
  DOCS_DIR docs)

HAWC_ADD_LIBRARY (grmodel-services
  HEADERS include/grmodel-services/*.h
          include/grmodel-services/diffuse/*.h
          include/grmodel-services/ebl/*.h
          include/grmodel-services/spectra/*.h
          include/grmodel-services/grb/*.h
  SOURCES src/*.cc
          src/diffuse/*.cc
          src/ebl/*.cc
          src/spectra/*.cc
          src/grb/*.cc
  CONFIGS config/*.txt config/*.xml config/*.xml.in config/*.xsd.in
  USE_PROJECTS hawcnest data-structures rng-service
  USE_PACKAGES cfitsio xercesc)

HAWC_ADD_PYBINDINGS (grmodel_services
  SOURCES src/pybindings/*.cc
  USE_PACKAGES Python Boost
  USE_PROJECTS hawcnest data-structures rng-service grmodel-services)

HAWC_ADD_TEST (grmodels
  SOURCES src/test/*.cc
  CONFIGS config/CREAM2-spectrum.xml.in
          config/ATIC2-spectrum.xml.in
          config/HESS-electron-spectrum.xml.in
          config/Combined-electron-spectrum.xml.in
          config/TeV-src-catalog.xml.in
          config/1FHL-PS-catalog.xml.in
          config/2FGL-PS-catalog.xml.in
  USE_PROJECTS hawcnest data-structures rng-service grmodel-services
  USE_PACKAGES Boost
  NO_PREFIX)

