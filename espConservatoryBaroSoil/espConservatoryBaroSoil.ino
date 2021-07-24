//SCL to D1
//SDA to D2
//Board = NodeMCU 1.0  (ESP-12E Module)

#include <mortylabs_esp8266_common.h>
#include <secrets.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <PubSubClient.h>

char*   hostname      = "espConservatory1";  // give your device a unique name here
float   LOCALALTUD    = 35.0;                // set your local altitude in meters
float   deep_sleep_ms = 000e6;               // 0e6 = 0 secs (sleep disabled), 300e6 = 5 mins sleep, 600e6 = 10 mins sleep etc

#define mqtt_t_heartbeat              "plants/conservatory1/LWT"

#define mqtt_t_config_temp            "homeassistant/sensor/conservatory1_sensor_temp/config"
#define mqtt_t_state_temp             "plants/conservatory1/temp/state"

#define mqtt_t_config_baro            "homeassistant/sensor/conservatory1_sensor_baro/config"
#define mqtt_t_state_baro             "plants/conservatory1/baro/state"

#define mqtt_t_config_soil1           "homeassistant/sensor/conservatory1_sensor_soil1/config"
#define mqtt_t_state_soil1            "plants/conservatory1/soil1/state"



topicArray_t mqtt_topics[] = {
//{"unique_id", "friendly name", "unit_of_measurement", "topic_config", "topic_state"}

  {"conservatory_temperature", "Conservatory Temperature", "°C",  mqtt_t_config_temp,  mqtt_t_state_temp},
  {"conservatory_baro",        "Conservatory Baro",        "hPa", mqtt_t_config_baro,  mqtt_t_state_baro},
  {"conservatory_soil1",       "Conservatory Soil 1",       "%",  mqtt_t_config_soil1, mqtt_t_state_soil1}
};


Adafruit_BMP280 bmp;

unsigned long   ms_mqtt_lastsent     = 0;
unsigned long   ms_sensors_lastread  = 0;
unsigned long   ms_startup           = millis();

int   analog_value         = 0;
int   analog_value_max      = 780; // every soil sensor gives different max/min readings, do some tests and adjust here
int   analog_value_min      = 250;  
float soil_moisture       = 0;
float baro_temperature    = 0;
float pressure_raw        = 0;
float pressure_ICAO       = 0;
float pressure_bosch      = 0;
float pressure_babinet    = 0;


ESP8266WebServer server(80);
WiFiClient       espClient;
PubSubClient     mqttClient(espClient); 

MyCommon         myCommon(/*hostname*/ hostname,/*mqttClient*/ mqttClient, /*mqtt_topic_heartbeat*/ mqtt_t_heartbeat, /*mqtt_topics*/ mqtt_topics, /*debug*/ true, /*battery*/ false, deep_sleep_ms);



void setup()
{
    Serial.begin(115200);
    delay(100);
    while (!Serial) { delay(1); }

    if (!bmp.begin((0x76))) {
    //if (!bmp.begin()) { 
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        while (1);
    }

    myCommon.begin();
    
    server.on("/", handleRoot);  
    server.on("/reboot", []() { server.send(200, "text/plain", "Rebooting..."); delay(1000); ESP.restart();});
    server.on("/wifi",   []() { server.send(200, "text/plain", myCommon.getWifiNetworkList()); });
    server.on("/test",   []() { server.send(200, "text/plain", "Other URL"); });
    server.onNotFound(   []() { server.send(404, "text/plain", "Ungaas baba, I 404 you..."); });
    server.begin();  

    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    //mqttClient.setCallback(callback);   //no need for callback as we are only transmitting data, not awaiting commands
    if (!mqttClient.connected()) { myCommon.mqtt_reconnect();  }
}

void read_sensors(){
  baro_temperature = bmp.readTemperature() ;
  pressure_raw     = bmp.readPressure(); 
  pressure_ICAO    = (pressure_raw  / pow(1.0 - 0.0065 * LOCALALTUD / (baro_temperature + 273.15), 5.255)) /100.00;
  pressure_bosch   = ( ((pressure_raw)/pow((1-((float)(LOCALALTUD))/44330), 5.255))/100.0); //BOSCH formula , std temp of 15C and good for indoors
  pressure_babinet = (- pressure_raw * (LOCALALTUD + 16000.0 + 64.0 * baro_temperature) / (LOCALALTUD - 16000.0 - 64.0 * baro_temperature))/100.0;

  analog_value = 0;
  analog_value = analogRead(A0);
  if (analog_value > analog_value_max) {
      analog_value_max = analog_value;
  }
  if (analog_value < analog_value_min) {
      analog_value_min = analog_value;
  }
  soil_moisture =  map(analog_value,analog_value_max,analog_value_min,0,100);
  if (soil_moisture < 0) {
      soil_moisture = 0;
  }
  ms_sensors_lastread = millis();
  
}

void loop() {
   if (WiFi.status() != WL_CONNECTED) { myCommon.connect_wifi(); }
   server.handleClient();                    
   ArduinoOTA.handle(); 

   if (!mqttClient.connected()) { myCommon.mqtt_reconnect();  }
   if (mqttClient.connected() ) { mqttClient.loop(); }

   if (millis() - ms_startup > 72000E3) {
    Serial.println("daily reboot");
    Serial.println(mqttClient.publish(mqtt_t_heartbeat, "offline"));
    delay(100);
    ESP.restart();
  }
  
  
  if (millis() - ms_sensors_lastread >= 10E3 or ms_sensors_lastread == 0 ){
    read_sensors();
  }      

  if (millis() - ms_mqtt_lastsent >= 60E3 or ms_mqtt_lastsent == 0 ){
      if (WiFi.status() != WL_CONNECTED) { myCommon.connect_wifi(); }
      if (not isnan(pressure_ICAO) ) {
        Serial.println(mqttClient.publish(mqtt_t_state_temp, String(baro_temperature).c_str(), true)) ;      
        Serial.println(mqttClient.publish(mqtt_t_state_baro, String(pressure_ICAO).c_str(), true)) ;      
      }
      if (not isnan(soil_moisture)  ) { 
          Serial.println(mqttClient.publish(mqtt_t_state_soil1, String(soil_moisture).c_str(), true)) ;      
       }
      ms_mqtt_lastsent = millis();
  }

 delay(100);
 if (deep_sleep_ms > 0) { Serial.print("sleeping for "); Serial.println(deep_sleep_ms/1000000.00); ESP.deepSleep(deep_sleep_ms); }
}


void handleRoot() {
  String a = ""; //"<html>" 
  a = a + myCommon.getHandleRootHeader(__FILE__, /* html */ false);
  
  

  a = a + "\n\nair pressure\n-------------\n";
  read_sensors();
  a = a + "baro temp           : ";
  a = a + baro_temperature;
  a = a + " C\npressure raw        : ";
  a = a + pressure_raw / 100.0;
  a = a + "\n\nindoors (baro_temperature)  : ";
  a = a + ( pressure_raw  / pow(1.0 - 0.0065 * LOCALALTUD / (baro_temperature + 273.15), 5.255) / 100.0); // ICAO formula

  a = a + "\noutdoors (ICAO 15C) : ";
  a = a + pressure_ICAO;
  a = a + "\noutdoors (BOSCH)    : ";
  a = a + pressure_bosch;
  a = a + "\noutdoors (babinet)  : ";
  a = a + pressure_babinet;
  a = a + "\n\nanalog value (soil) : ";
  analog_value = analogRead(A0);
      
  a = a + analog_value;
  a = a + "   (  ";
  a = a + soil_moisture;
  a = a + "% )";
  a = a + "  min=";
  a = a + analog_value_min;
  a = a + "  max=";
  a = a + analog_value_max;

  server.send(200, "text/plain", a);   // Send HTTP status 200 (Ok) and send some text to the browser/client
}
