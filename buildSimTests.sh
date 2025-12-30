mkdir build-simtests
pushd build-simtests
cmake .. --preset host-test
for simtestname in sim-tests/test-programs/*.cpp
do
    filename_without_suffix = ${simtestname%.cpp}
    name = ${filename_without_suffix##*/}
    make ${name}
done
popd