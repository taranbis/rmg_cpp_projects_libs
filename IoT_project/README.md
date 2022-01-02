## IoT_Challenge


### Installation process
I wanted to add Paho MQTT C and CPP as submodules, but they don't fit the purpose; installation is done therefore separately

- install [`Paho MQTT C Client Library`](https://github.com/eclipse/paho.mqtt.c):
```bash
git clone https://github.com/eclipse/paho.mqtt.c
cd paho.mqtt.c
git checkout v1.3.8

cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
    -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig
```

- install [`Paho MQTT C++ Client Library`](https://github.com/eclipse/paho.mqtt.cpp):
```bash
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp

cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
    -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
sudo cmake --build build/ --target install
sudo ldconfig
```

- install Eigen3:
```bash
cd ../vendors/eigen
cmake -Bbuild
cd build
sudo make install
```
