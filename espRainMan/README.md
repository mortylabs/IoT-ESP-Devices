# Rain Man

An old 1960s rain indicator from my grandparents has found a new lease on life! Originally, it featured a comical figurine of a drunkard clinging to a lamppost, with a clever touch ... the lamp housed litmus paper that changed from blue to pink to signal approaching storms.

The litmus paper is long gone, but now the figurine lives on with a modern twist. Hidden inside is an ESP8266 D1 Mini, paired with a barometric pressure and temperature sensor, a motion-activated screen, and RGB LED indicators. It subtly informs you of changing weather conditions, publishing data via MQTT or logging it directly to InfluxDB for full smart home integration i.e. Home Assistant. 

The defunct litmus lamp now glows with pink or blue LEDs, depending on the atmospheric pressure trend. The entire setup is cleverly mounted onto a £2 wooden pencil box: the screen is embedded in the lid, a PIR sensor wakes the display when someone enters the room, and all the electronics are neatly tucked away inside.

**espRainMan** transforms nostalgic whimsy into a sleek, WiFi-enabled ambient weather station—perfectly at home in both vintage decor and modern smart homes.

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
