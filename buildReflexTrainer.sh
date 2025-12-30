mkdir build-reflexTrainer
pushd build-reflexTrainer
cmake .. --preset default-avr
make reflexTrainerMain
popd
