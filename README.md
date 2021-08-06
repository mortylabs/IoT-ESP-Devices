# IoT ESP8266 / ESP32 firmware
C++ source code / firmware for home-grown WiFi sensors and devices, with a focus on gardening and the outdoors.

All code uses the common library **mortylabs_common**. With 30+ wifi devices I found I had a lot of repeating code that was copied & pasted to each new device. Of course the usual maintence overhead reared its ugly head whenever I made a change to that common code, so it has all been abstracted into a common library. 

To use the common library, simply copy it into your **arduino/libraries** folder. See [mortylabs_common](https://github.com/mortylabs/IoT-firmware/blob/main/mortylabs_common/README.md) for instructions. 



 script | device | mqtt | webpage | sensors | Description |
| :--- | :--- | :---: | :---: | :--- | :--- |
| indoors_soil_baro_temperature | esp8266 | Y | Y | barometer & temperature (BMP280), soil moisture | monitors a plant in my conservatory |
| more coming soon | - | - | - | - | - |


