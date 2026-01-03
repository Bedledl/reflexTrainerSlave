add_subdirectory(test-programs)
# Tests
# Test ms of clock, test overflow
# Test timer with and without prescaling

add_executable( simavr-test1 simavr.cpp button.cpp)
find_library(SIMAVR_LIB simavr)
target_link_libraries(simavr-test1 PUBLIC ${SIMAVR_LIB})
target_include_directories(simavr-test1 PUBLIC /usr/local/include/simavr/ )