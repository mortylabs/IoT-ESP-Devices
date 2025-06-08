# Rain Man

An old 1960s rain indicator belonging to my grandparents has a new life!
It's a comical figurene of a drunkard hanging onto a lamp post. The lamp once contained litmus paper that changed from blue to pink if a storm was approaching.


The litmus paper has long perished, but in this new life, the interior has been stuffed with a esp8266 D1, barmoetric pressure and temperature sensor with a motion-activated screen and RGB LED feedback to subtly inform you of changing weather conditions, with all data published on a MQTT topic of your choice or written to InfluxDB. 

**espRainMan** is a sleek, WiFi-enabled weather monitoring and ambient notification device, which publishes data to InfluxDB and MQTT, and integrates into Home Assistant. 

The defuct litmus paper was replaced with blue / pink led bulbs, and the whole gadet was mounted onto a cheap £2 wooden pencil box. A screen was mounted into the lid of the pencil box, a motion sensor added to activate the screen when someone enters the room, and the esp and temperature / barometer are of course hidden away within the box. 

![image](https://user-images.githubusercontent.com/31904545/151181954-4964c2b9-37b3-4220-8ef0-231306940c99.png)

## ✨ Features

- 🧠 **Barometric Trend Detection**: Monitors rapid pressure drops—perfect for anticipating incoming storms.
- 🔴🔵 **Visual Alerts**: Red light means pressure is dropping fast (potential bad weather); blue means stable skies.
- 🕵️ **Motion-Activated Display**: PIR sensor detects presence and lights up the OLED screen with weather data.

## 🧰 Hardware Requirements

- [Wemos D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)
- BMP280 or BME280 (Barometric Pressure + Temp)
- PIR Motion Sensor
- OLED Display (SSD1306 or SH1106)
- WS2812 or RGB LED
- USB Power Supply

## 🚀 Getting Started

1. Flash using PlatformIO or Arduino IDE.
2. Configure your WiFi credentials and pin mappings.
3. Power it up and watch as it tracks the sky for you!

## 🧠 How It Works

- Barometer readings are smoothed to avoid noise.
- Pressure trends are analyzed over a rolling window.
- LED turns red if a steep pressure drop is detected (storm warning).
- Motion triggers OLED screen to show real-time data.


## 🌱 Ideal For

- Weather hobbyists
- Smart home enthusiasts
- Educational STEM projects
- Subtle ambient information systems

## 📸 Gallery

Coming soon! Want to share your build? Send a PR or open an issue!

---

🌦️ *Stay ahead of the storm with espRainMan!*
