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

file(GLOB AVR_STDLIB_SRC "packages/avr-libstdcpp/src/functexcept.cc")

add_executable(reflexTrainer main.cpp  ${SRC_FILES} ${SRC_FILES_AVR} ${AVR_STDLIB_SRC})

#add_subdirectory(packages/avr-nec-ir-decoder)
#target_link_libraries(reflexTrainer PRIVATE ir_receiver_driver)
target_include_directories(reflexTrainer PRIVATE include)

target_include_directories(
  reflexTrainer
  PRIVATE
  packages/avr-libstdcpp/include
)


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