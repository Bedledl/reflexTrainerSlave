mkdir build-unittests
pushd build-unittests
cmake .. --preset host-test
make unittestss
popd
