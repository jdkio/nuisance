if(NOT TARGET GENIE::All)

include(FindPackageHandleStandardArgs)

find_path(GENIE_INC_DIR
  NAMES EVGCore/EventRecord.h
  PATHS ${GENIE}/include/GENIE)

find_path(GENIE_LIB_DIR
  NAMES libGEVGCore.so
  PATHS ${GENIE}/lib)

find_package_handle_standard_args(GENIE
  REQUIRED_VARS 
    GENIE 
    GENIE_INC_DIR 
    GENIE_LIB_DIR
    LOG4CPP_INC_DIR
    LOG4CPP_LIB_DIR
    PYTHIA6_LIB_DIR
  VERSION_VAR GENIE_VERSION
)

if(GENIE_FOUND)

  include(ParseConfigApps)

  GetLibs(CONFIG_APP genie-config ARGS --libs OUTPUT_VARIABLE GENIE_LIBS)

  string(REGEX REPLACE "\([0-9]\)\.\([0-9]*\).*" "\\1\\2" GENIE_SINGLE_VERSION ${GENIE_VERSION})
  string(LENGTH "${GENIE_SINGLE_VERSION}" GENIE_SINGLE_VERSION_STRLEN)
  if(GENIE_SINGLE_VERSION_STRLEN LESS 3)
    string(APPEND GENIE_SINGLE_VERSION "0")
  endif()
  
  LIST(APPEND GENIE_DEFINES -DGENIE_ENABLED -DGENIE_VERSION=${GENIE_SINGLE_VERSION})

  string(REGEX MATCH "ReinSeghal" RS_FOUND "${GENIE_LIBS}")
  if(RS_FOUND AND "${GENIE_SINGLE_VERSION}" STREQUAL "210")
    STRING(REPLACE "ReinSeghal" "ReinSehgal" GENIE_LIBS "${GENIE_LIBS}")
  endif()

  LIST(REMOVE_ITEM GENIE_LIBS "GReWeight")
  if(EXISTS ${GENIE_LIB_DIR}/libGReWeight.so)
    set(GENIEReWeight_ENABLED TRUE)
    LIST(APPEND GENIE_LIBS GReWeight)
  endif()

  if(EXISTS ${GENIE_INC_DIR}/ReWeight/GReWeightXSecEmpiricalMEC.h)
    SET(GENIE2_XSECEMPMEC_ENABLED TRUE)
  endif()

  #duplicate because CMake gets its grubby mitts on repeated -Wl,--start-group options
  SET(GENIE_LIBS "-Wl,--no-as-needed;${GENIE_LIBS};${GENIE_LIBS};-Wl,--as-needed")

  cmessage(STATUS "GENIE 2 (Version: ${GENIE_VERSION})")
  cmessage(STATUS "                GENIE: ${GENIE}")
  cmessage(STATUS "              OPTIONS: GENIEReWeight: ${GENIEReWeight_ENABLED}, XSecEmpiricalMECReWeight: ${GENIE2_XSECEMPMEC_ENABLED}")
  cmessage(STATUS " GENIE_SINGLE_VERSION: ${GENIE_SINGLE_VERSION}")
  cmessage(STATUS "        GENIE DEFINES: ${GENIE_DEFINES}")
  cmessage(STATUS "       GENIE INC_DIRS: ${GENIE_INC_DIR}")
  cmessage(STATUS "       GENIE LIB_DIRS: ${GENIE_LIB_DIR}")
  cmessage(STATUS "           GENIE LIBS: ${GENIE_LIBS}")
  cmessage(STATUS "        DEPS INC_DIRS: ${LOG4CPP_INC_DIR}")
  cmessage(STATUS "        DEPS LIB_DIRS: ${LOG4CPP_LIB_DIR};${PYTHIA6_LIB_DIR}")
  cmessage(STATUS "            DEPS LIBS: Pythia6;log4cpp;LibXml2::LibXml2;GSL::gsl")

  if(NOT TARGET GENIE::All)
    add_library(GENIE::All INTERFACE IMPORTED)
    set_target_properties(GENIE::All PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GENIE_INC_DIR};${LOG4CPP_INC_DIR}"
        INTERFACE_COMPILE_OPTIONS "${GENIE_DEFINES}"
        INTERFACE_LINK_DIRECTORIES "${GENIE_LIB_DIR};${LOG4CPP_LIB_DIR};${PYTHIA6_LIB_DIR}"
        INTERFACE_LINK_LIBRARIES "${GENIE_LIBS};EGPythia6;Pythia6;log4cpp;LibXml2::LibXml2;GSL::gsl"
    )
  endif()
endif()

endif()
