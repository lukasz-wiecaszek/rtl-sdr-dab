cmake_minimum_required(VERSION 3.3)

project(rtl-sdr-dab VERSION 1.0.0)

# sets various paths used in e.g. pc.in files as well as install target
include(GNUInstallDirs)

message(STATUS "Processing CMakeLists.txt for: " ${PROJECT_NAME} " " ${PROJECT_VERSION})

# if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise
# this is the top level directory of your build tree
message(STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR})

# if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this
# is the directory where the compiled or generated files from the current CMakeLists.txt will go to
message(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})

# this is the directory, from which cmake was started, i.e. the top level source directory
message(STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR})

# this is the directory where the currently processed CMakeLists.txt is located in
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

option(RTL_SDR_GPS_TESTS "Enable testing" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
         "Choose the type of build. Options are: {Release, Debug}." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

add_compile_options(-Wall -Werror)

if("${CMAKE_BUILD_TYPE_LOWER}" STREQUAL "debug")
    add_compile_options(-ggdb3)
endif()

if(RTL_SDR_DAB_TESTS)
    add_compile_options(-fprofile-arcs -ftest-coverage)
endif()

set(RTL_SDR_DAB_SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(RTL_SDR_DAB_SRCS
    ${RTL_SDR_DAB_SRC_DIR}/main.cpp
    ${RTL_SDR_DAB_SRC_DIR}/flowgraph.cpp
)

configure_file(version.h.in version.h)
configure_file(rtl-sdr-dab.sh rtl-sdr-dab.sh COPYONLY)
file(COPY stations/ DESTINATION stations/)

#------------------------------------------------------------------------------
#                                 DEPENDENCIES
#------------------------------------------------------------------------------
add_subdirectory(cmdlineflags)
add_subdirectory(cfgitems)

find_package(Boost REQUIRED COMPONENTS chrono)

find_package(Gnuradio "3.8.0" REQUIRED COMPONENTS runtime blocks fft)

set(gnuradio-bad_DIR "/usr/local/lib/cmake/bad")
find_package(gnuradio-bad REQUIRED)

set(gnuradio-osmosdr_DIR "/usr/local/lib/cmake/osmosdr")
find_package(gnuradio-osmosdr REQUIRED)

#------------------------------------------------------------------------------
#                                 MAIN TARGETS
#------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    ${RTL_SDR_DAB_SRCS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${RTL_SDR_DAB_SRC_DIR}
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        cmdlineflags
        cfgitems
        gnuradio::gnuradio-runtime
        gnuradio::gnuradio-blocks
        gnuradio::gnuradio-fft
        gnuradio::gnuradio-bad
        gnuradio::gnuradio-osmosdr
)

#------------------------------------------------------------------------------
#                                 INSTALLATION
#------------------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(EXPORT ${PROJECT_NAME}-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

#------------------------------------------------------------------------------
#                                    TESTS
#------------------------------------------------------------------------------
if(RTL_SDR_DAB_TESTS)
    enable_testing()
    add_subdirectory(tst)
endif()
