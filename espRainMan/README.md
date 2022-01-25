# Rain Man

An old 1960s rain indicator belonging to my grandparents has a new life!

It's a humerous figurene of a drunkard hanging onto a lamp post. The lamp once contained litmus paper that changed from blue to pink if a storm was approaching.

The defuct litmus paper was replaced with blue / pink led bulbs, and the whole gadet was mounted onto a cheap £2 wooden pencil box. A screen was mounted into the lid of the pencil box, a motion sensor added to activate the screen when someone enters the room, and the esp and temperature / barometer are of course hidden away within the box. 

![image](https://user-images.githubusercontent.com/31904545/150983205-7499dedd-7216-423d-82da-c9b90381ee18.png)


Source code has only 2 lines which need to be configured:
```
MyCommon  client("espRainMan", "WIFI_SSID_HERE", "WIFI_PASSWORD_HERE", "MQTT_BROKER_IP_HERE", "MQTT_PORT_HERE", "mqtt_user", "MQTT_PASSWORD_HERE");
float LOCALALTUD            = 1469 ;  // local altitude in meters
```




