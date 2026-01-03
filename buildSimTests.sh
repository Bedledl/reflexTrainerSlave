mkdir build-simtests-avr
pushd build-simtests-avr
cmake .. --preset default-avr
for simtestname in ../sim-tests/test-programs/*.cpp
do
    filename_without_suffix=${simtestname%.cpp}
    name=${filename_without_suffix##*/}
    echo "MAKE ${name}"
    make ${name}
done
popd

mkdir build-simtests
pushd build-simtests
cmake .. --preset host-test
make simavr-test1
popd
