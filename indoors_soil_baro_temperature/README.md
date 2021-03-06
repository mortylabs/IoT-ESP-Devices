## Overview
This esp8266 is indoors and powered by a USB power adapter.  It measures the air temperature, atmospheric pressure, and the soil moisture of an indoor plant.

Sensor data is published to an MQTT broker in a format that Home Assistant and its "auto discovery" feature can use. 

The device has a simple webpage that you can view to see the status of the device and its sensors:
![image](https://user-images.githubusercontent.com/31904545/128088460-5d00357c-ca6f-425b-b76e-0fba93cebd14.png)

Of course Home Assistant can display the data in a much cleaner format, for example:

![image](https://user-images.githubusercontent.com/31904545/128089304-bbe95de3-8fb6-43ad-81dc-e397c726fc8f.png)

Or you could also use Grafana to view the barometer other sensors:

![image](https://user-images.githubusercontent.com/31904545/126867018-0083f65e-70a9-48dd-8357-560c2c76c1f6.png)



## WiFI / MQTT Configuration

On startup, if the device doesn't have any saved config, or fails to connect to a WiFi network, it will enter AP mode and broadcast its own SSID. Connect to the AP and navigate to **192.168.4.1** where you can enter credentials on the **setup tab** 

![image](https://user-images.githubusercontent.com/31904545/128343205-7b3e722e-ad16-476e-8971-19f513c11999.png)
* if already connected to your home WiFi network, just click on the same **setup tab** to modify credentials: ![image](https://user-images.githubusercontent.com/31904545/128088647-1e573e9c-77d1-4a0b-9fd5-87567a74625b.png)



## Wiring

For the BMP280, connect D1 to SCL and D2 to SDA. 
For the soil moisture sensor, connect A0 to AOUT. NB Only purchase **Capacitive Soil Moisture Sensor** as seen in the diagram below, do not purchase a two-prong sensor as they rust and stop working very quickly. 

![image](https://user-images.githubusercontent.com/31904545/128090830-8379d853-08c3-46d9-8894-ae3b82259865.png)
