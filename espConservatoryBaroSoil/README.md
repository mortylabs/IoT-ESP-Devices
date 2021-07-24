This is an esp wifi device I havbe in my conservatory.

It measures the air temperature, air pressure, and the soil moisture of an indoor plant.
It then publishes that data to an MQTT broker in a format that Home Assistant and its "auto discovery" feature can use. 

The device has a simple webpage that you can view to see the status of the device and its sensors:

![image](https://user-images.githubusercontent.com/31904545/126866921-ff5f0eab-1f0f-4206-a79b-6ab91be3eb5f.png)

Of course Home Assistant will display the data in a much cleaner format:

6![image](https://user-images.githubusercontent.com/31904545/126866973-4f002833-3d46-4cd7-9c92-1f9949c4b4db.png)

Or you can use Grafana to view the barometer etc:

![image](https://user-images.githubusercontent.com/31904545/126867018-0083f65e-70a9-48dd-8357-560c2c76c1f6.png)
