# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.15...4.0)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  ReflexTrainerUnit
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

file(GLOB SRC_FILES
    "src/*.cpp"
)

file(GLOB SRC_FILES_AVR
    "src/avr-specific/*.cpp"
)

add_subdirectory(packages)

add_library(reflexTrainerUtilGeneral STATIC ${SRC_FILES})
add_library(reflexTrainerUtilAvrSpecific STATIC ${SRC_FILES_AVR})
add_executable(reflexTrainerMain main.cpp ${SRC_FILES_AVR} )


add_library(reflexTrainerUtilGeneral::reflexTrainerUtilGeneral ALIAS reflexTrainerUtilGeneral)
add_library(reflexTrainerUtilAvrSpecific::reflexTrainerUtilAvrSpecific  ALIAS reflexTrainerUtilAvrSpecific)



target_include_directories(reflexTrainerUtilGeneral PUBLIC include)
if( ${CROSS_COMPILING} STREQUAL "TRUE")
target_link_libraries(reflexTrainerUtilGeneral PUBLIC avr-libstdcpp::avr-libstdcpp)
endif()

target_link_libraries(reflexTrainerUtilAvrSpecific PUBLIC reflexTrainerUtilGeneral)
target_link_libraries(reflexTrainerMain PUBLIC reflexTrainerUtilAvrSpecific::reflexTrainerUtilAvrSpecific)

add_subdirectory(sim-tests)

################# Unittests #################
# Enable GoogleTest

if( NOT ${CROSS_COMPILING} STREQUAL "TRUE")
  find_package(GTest REQUIRED)
  enable_testing()
  file (GLOB TEST_FILES
      "unittests/*.cpp"
  )
  file(GLOB SRC_FILES_AVR_TIMER
    "src/avrTimer*"
  )
  add_executable(
    unittestss
    EXCLUDE_FROM_ALL
    ${TEST_FILES}
    ${SRC_FILES_AVR_TIMER}
  )
  target_link_libraries(
    unittestss
    PRIVATE
    GTest::gtest_main
    GTest::gmock_main
    GTest::gtest
  )
  target_include_directories(
    unittestss
    PRIVATE
    include
  )

  include(GoogleTest)
  gtest_discover_tests(unittestss)

endif()

