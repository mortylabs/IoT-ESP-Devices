#include "secrets.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <espWebServerOTAMqtt.h>
  
MyCommon  client("espConservatory1");
Adafruit_BMP280 bmp;

float LOCALALTUD           = 35.0; // set your local altitude in meters
int   analog_value         = 0;    // in this sketch, the soil moisture sensor is connected to anolog A0
int   analog_soil_max      = 720;  // every soil sensor gives different max/min readings, do some tests and adjust here
int   analog_soil_min      = 250;  
float soil_perc            = 0;

//BMP280 barometer, there are a few "official" formulas calculating air pressure slightly differently
float baro_temperature     = 0;
float pressure_raw         = 0;
float pressure_ICAO        = 0;
float pressure_bosch       = 0;
float pressure_babinet     = 0;

unsigned long ms_mqtt_lastsent = 0;
#define mqtt_t_avail                  "plants/conservatory1/LWT"
#define mqtt_t_temp_config            "homeassistant/sensor/conservatory1_sensor_temp/config"
#define mqtt_t_temp_state             "plants/conservatory1/temp/state"
#define mqtt_t_baro_config            "homeassistant/sensor/conservatory1_sensor_baro/config"
#define mqtt_t_baro_state             "plants/conservatory1/baro/state"
#define mqtt_t_soil1_config           "homeassistant/sensor/conservatory1_sensor_soil1/config"
#define mqtt_t_soil1_state            "plants/conservatory1/soil1/state"
  

//void onConnectionEstablished() {
//  Serial.println("yo");  
//}

void set_html_index_body() {
  String s = "<table><tr><th style='text-align:left;background-color:green;color:white;'>sensor</th><th style='text-align:left;background-color:green;color:white;'s>value</th></tr>";
  s= s + client.generate_html_table_row ("td","temperature", String(baro_temperature)+ " C");
  s= s + client.generate_html_table_row ("td","pressure raw", String(pressure_raw / 100.0));
  s= s + client.generate_html_table_row ("td","indoors (baro_temperature)", String(( pressure_raw  / pow(1.0 - 0.0065 * LOCALALTUD / (baro_temperature + 273.15), 5.255) / 100.0))); // ICAO formula
  s= s + client.generate_html_table_row ("td","outdoors (ICAO 15C)", String(pressure_ICAO)); 
  s= s + client.generate_html_table_row ("td","outdoors (BOSCH)", String(pressure_bosch)); 
  s= s + client.generate_html_table_row ("td","outdoors (babinet)", String(pressure_babinet)); 
  String a = "";
  a = a + analog_value;
  a = a + "   (  ";
  a = a + soil_perc;
  a = a + "% )  min=";
  a = a + analog_soil_min;
  a = a + ", max-=";
  a = a + analog_soil_max;
  s = s + client.generate_html_table_row ("td", "analog value (soil)", String(a)); 
  s =s + "</table>";
  client.set_html_index_body(s);
}

void setup() {
  client.setup();
  if (!bmp.begin((0x76))) {
  //if (!bmp.begin()) { 
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      //  while (1);
    }
}


void read_sensors() {
  Serial.println("READ SENSORS!");
  baro_temperature = bmp.readTemperature() ;
  pressure_raw     = bmp.readPressure(); 
  pressure_ICAO    = (pressure_raw  / pow(1.0 - 0.0065 * LOCALALTUD / (baro_temperature + 273.15), 5.255)) /100.00;
  pressure_bosch   = ( ((pressure_raw)/pow((1-((float)(LOCALALTUD))/44330), 5.255))/100.0); //BOSCH formula , std temp of 15C and good for indoors
  pressure_babinet = (- pressure_raw * (LOCALALTUD + 16000.0 + 64.0 * baro_temperature) / (LOCALALTUD - 16000.0 - 64.0 * baro_temperature))/100.0;

  analog_value = analogRead(A0);
  if (analog_value > analog_soil_max) { analog_soil_max = analog_value; }
  if (analog_value < analog_soil_min and analog_value != 0) { analog_soil_min = analog_value; }
  
  soil_perc =  map(analog_value,analog_soil_max,analog_soil_min,0,100);
  if (soil_perc <= 0 or analog_value == 0) { soil_perc = 0; }
  set_html_index_body();     
}


void loop() {
  client.loop();
  if (millis() - ms_mqtt_lastsent >= 60E3 or ms_mqtt_lastsent == 0){
     read_sensors();
     if (not isnan(pressure_ICAO) and pressure_ICAO >= 500 and pressure_ICAO <= 2000 ) {
        Serial.println(client.publish(mqtt_t_temp_state, String(baro_temperature).c_str(), true)) ;      
        Serial.println(client.publish(mqtt_t_baro_state, String(pressure_ICAO).c_str(), true)) ;      
     }
     if (not isnan(soil_perc)  ) { 
        Serial.println(client.publish(mqtt_t_soil1_state, String(soil_perc).c_str(), true)) ;      
     }
     ms_mqtt_lastsent = millis();
  }
  
}
