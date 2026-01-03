message("CMAKEFILE: PACKAGES")
file(GLOB AVR_STDLIB_SRC "avr-libstdcpp/src/functexcept.cc")
add_library(avr-libstdcpp STATIC ${AVR_STDLIB_SRC})
add_library(avr-libstdcpp::avr-libstdcpp ALIAS avr-libstdcpp)
target_include_directories(
  avr-libstdcpp
  PUBLIC
  "avr-libstdcpp/include"
)

# simavr
#add_custom_target(
#   simavr-Make
#   COMMAND "make build-simavr"
#   WORKING_DIRECTORY packages/simavr
#)
#
#
#add_library(simavr STATIC IMPORTED GLOBAL)
#set_target_properties(simavr PROPERTIES IMPORTED_LOCATION simavr/simavr/obj-x86_64-linux-gnu/libsimavr.a)
#add_library(simavr::simavr ALIAS simavr)
#add_dependencies(myexecutable extern_lib)
