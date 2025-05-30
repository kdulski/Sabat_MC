cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(Sabat_exe)

option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR})
# We presume the existence of three directories
#file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cc)
#file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.hh)
#file(GLOB_RECURSE macros RELATIVE ${PROJECT_SOURCE_DIR} macros/*.mac)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB macros RELATIVE ${PROJECT_SOURCE_DIR} macros/*.mac)


# Enable macros for out-of-source build
foreach(_file ${macros})
  configure_file(
    ${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
    )
endforeach()

configure_file(
    initConfig.dat
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
    )

add_executable(Sabat_exe main.cc ${sources} ${headers})
target_link_libraries(Sabat_exe ${Geant4_LIBRARIES})
target_link_libraries(Sabat_exe ${cadmesh_LIBRARIES})

install(TARGETS Sabat_exe DESTINATION bin)
