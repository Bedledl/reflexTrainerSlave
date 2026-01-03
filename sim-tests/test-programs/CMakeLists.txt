
set(MCU_SECTION "${CMAKE_CURRENT_SOURCE_DIR}/../../packages/simavr/simavr/sim/avr/")
message(MCU_SECTION="${MCU_SECTION}")

file(GLOB SRC_FILES "*.cpp")

foreach( testsourcefile ${SRC_FILES} )
    message(TESTSOURCEFILE="${testsourcefile}")
    # Cut off the file extension and directory path
    get_filename_component( testname ${testsourcefile} NAME_WE )
    message(TESTNAME="${testname}")
    add_executable( ${testname} ${testsourcefile} ${SRC_FILES_AVR})
    target_include_directories(${testname} PUBLIC ${MCU_SECTION})
    target_link_libraries(${testname} PUBLIC reflexTrainerUtilAvrSpecific::reflexTrainerUtilAvrSpecific avr-libstdcpp::avr-libstdcpp)
endforeach( testsourcefile ${LIB_SOURCES} )

# compile example to elf and axf -> How?
# write test main program that loads the example elf files and run tests -> depend also on gtest

