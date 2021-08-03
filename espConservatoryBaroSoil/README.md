This esp8266 lives indoors, in a conservatory. 

It measures the air temperature, atmospheric pressure, and soil moisture of an indoor plant.
It then publishes sensor data to an MQTT broker in a format that Home Assistant and its "auto discovery" feature can use. 

The device has a simple webpage that you can view to see the status of the device and its sensors:
![image](https://user-images.githubusercontent.com/31904545/128088460-5d00357c-ca6f-425b-b76e-0fba93cebd14.png)

It also has a setup page so you can reconfigure the WiFi and MQTT settings:
![image](https://user-images.githubusercontent.com/31904545/128088647-1e573e9c-77d1-4a0b-9fd5-87567a74625b.png)



Of course Home Assistant will display the data in a much cleaner format:

![image](https://user-images.githubusercontent.com/31904545/128089304-bbe95de3-8fb6-43ad-81dc-e397c726fc8f.png)


Or you can use Grafana to view the barometer etc:

![image](https://user-images.githubusercontent.com/31904545/126867018-0083f65e-70a9-48dd-8357-560c2c76c1f6.png)

**Wiring**

For the BMP280, connect D1 to SCL and D2 to SDA. 
For the soil moisture sensor, connect A0 to AOUT. NB Only purchase **Capacitive Soil Moisture Sensor** as seen in the diagram below, do not purcahse a two-prong sensor as they rust and stop working very quickly. 

![image](https://user-images.githubusercontent.com/31904545/128090830-8379d853-08c3-46d9-8894-ae3b82259865.png)
