#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <espWebServerOTAMqtt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

MyCommon  client("espRainMan", "WIFI_SSID_HERE", "WIFI_PASSWORD_HERE", "MQTT_BROKER_IP_HERE", "MQTT_PORT_HERE", "mqtt_user", "MQTT_PASSWORD_HERE");
float LOCALALTUD            = 1469 ; //1500.0; // local altitude in meters


unsigned long ms_lcd_on_for = 60E3;
Adafruit_BMP280 bmp;  
LiquidCrystal_I2C lcd(0x27, 16, 2);

//BMP280 barometer, there are a few "official" formulas calculating air pressure slightly differently
float baro_temperature      = 0;
float pressure_raw          = 0;
float pressure_ICAO         = 0;
float pressure_ICAO_last1   = 0;
float pressure_ICAO_last2   = 0;
float pressure_ICAO_last3   = 0;
float pressure_ICAO_last4   = 0;
float pressure_bosch        = 0;
float pressure_babinet      = 0;


unsigned long ms_startup       = 0;   
unsigned long ms_mqtt_lastsent = 0;   // elapsed millisecond since last mqtt publish
unsigned long ms_pir_lastseen  = 0;
unsigned long ms_pressure_last = 0;
boolean       lcd_status       = false;


#define mqtt_t_avail             "plants/rainman/LWT"
#define mqtt_t_temp_config       "homeassistant/sensor/rainman_temp/config"
#define mqtt_t_temp_state        "plants/rainman/temp/state"
#define mqtt_t_baro_config       "homeassistant/sensor/rainman_baro/config"
#define mqtt_t_baro_state        "plants/rainman/baro/state"
#define mqtt_t_rainman_set       "plants/rainman/set"; 
  
#define LED_BLUE  D5
#define LED_RED   D4
#define PIR_PIN   D6


void update_main_page_sensor_table () {
  String s = "<table><tr><th style='text-align:left;background-color:green;color:white;'>sensor</th><th style='text-align:left;background-color:green;color:white;'s>value</th></tr>";
  s += client.generate_html_table_row ("td","temperature", String(baro_temperature)+ " C");
  s += client.generate_html_table_row ("td","pressure raw", String(pressure_raw / 100.0));
  s += client.generate_html_table_row ("td","indoors (baro_temperature)", String(( pressure_raw  / pow(1.0 - 0.0065 * LOCALALTUD / (baro_temperature + 273.15), 5.255) / 100.0))); // ICAO formula
  s += client.generate_html_table_row ("td","outdoors (ICAO 15C)", String(pressure_ICAO)); 
  s += client.generate_html_table_row ("td","outdoors (BOSCH)", String(pressure_bosch)); 
  s +=client.generate_html_table_row ("td","outdoors (babinet)", String(pressure_babinet)); 
  String a = "";
  s += "</table>";
  client.set_main_page_html_body (s);
}

void setup() {
  ms_startup       = 0;
  while (!Serial) { delay(1); }
  delay(100);
  pinMode(LED_BLUE, OUTPUT);   
  pinMode(LED_RED, OUTPUT);   
  pinMode(PIR_PIN, INPUT);     
  digitalWrite(LED_BLUE, HIGH); 
  digitalWrite(LED_RED, HIGH); 
  delay(100);
  lcd.init();
  lcd.begin(16,2);
  delay(100);
  lcd.backlight();
  delay(200);
  write_lcd ("starting...     ", "                ");
  delay(100);
  
  client.setup();

  if (!bmp.begin((0x76))) {
  //if (!bmp.begin()) { 
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      write_lcd ("ERROR          ", "BMP280 invalid! ");
      delay(3000);    
    } else if (WiFi.softAPIP().toString() == "192.168.4.1") {
        write_lcd ("APMODE       ", WiFi.softAPIP().toString() );
        delay(3000);    
    } 
    else {
       write_lcd ("online!          ", WiFi.localIP().toString() );
      delay(2000);
      ms_pir_lastseen = millis();

    }

  read_sensors();
  pressure_ICAO_last4 = pressure_ICAO;
  pressure_ICAO_last3 = pressure_ICAO;
  pressure_ICAO_last2 = pressure_ICAO;
  pressure_ICAO_last1 = pressure_ICAO;
  digitalWrite(LED_BLUE, HIGH); 
  digitalWrite(LED_RED, HIGH); 
  digitalWrite(LED_BUILTIN, LOW);
  
              
}

void write_lcd (String line1, String line2) {
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}


void read_sensors() {
  baro_temperature = bmp.readTemperature() ;
  
  pressure_raw     = bmp.readPressure(); 
  pressure_ICAO    = (pressure_raw  / pow(1.0 - 0.0065 * LOCALALTUD / (baro_temperature + 273.15), 5.255)) /100.00;
  pressure_bosch   = ( ((pressure_raw)/pow((1-((float)(LOCALALTUD))/44330), 5.255))/100.0); //BOSCH formula , std temp of 15C and good for indoors
  pressure_babinet = (- pressure_raw * (LOCALALTUD + 16000.0 + 64.0 * baro_temperature) / (LOCALALTUD - 16000.0 - 64.0 * baro_temperature))/100.0;
  update_main_page_sensor_table();     
}


void save_pressure_history() {
    if (millis() - ms_pressure_last  > 60*60E3 or ms_pressure_last == 0) {
    ms_pressure_last = millis();
    pressure_ICAO_last4 = pressure_ICAO_last3;
    pressure_ICAO_last3 = pressure_ICAO_last2;
    pressure_ICAO_last2 = pressure_ICAO_last1;
    pressure_ICAO_last1 = pressure_ICAO;
  }
}



void loop() {
  client.loop();
  if (baro_temperature == 0 and millis() - ms_startup >= 180E3) {
    Serial.println("temp = 0 so RESTARTING");
    delay(100);
    ESP.restart();
  }

  if (millis() - ms_startup >= 6.048e+8 ) { 
    Serial.println("SCHEDULED REBOOT");
    ESP.restart();
  }

  if (WiFi.status() != WL_CONNECTED or WiFi.softAPIP().toString() == "192.168.4.1") {
    write_lcd (WiFi.SSID(), WiFi.softAPIP().toString());
  } 


  if (digitalRead(PIR_PIN) == HIGH) {
      ms_pir_lastseen = millis();
      //Serial.println("PIR");
  }
  
  if (millis() - ms_pir_lastseen <= ms_lcd_on_for and !lcd_status) {
      if (pressure_ICAO_last4 - pressure_ICAO > 3 or pressure_ICAO_last1 - pressure_ICAO > 1) {
         digitalWrite(LED_RED, HIGH); 
             delay(100);            
         digitalWrite(LED_BLUE, LOW); 
         delay(100);
      } else {
         digitalWrite(LED_RED, LOW); 
         delay(100);            
         digitalWrite(LED_BLUE, HIGH); 
         delay(100);
      }

      lcd_status = true;
      lcd.backlight();
      lcd.display();
      write_lcd ("Morty Labs         ", String(baro_temperature, 1) + ((char)223) + "C " + String(pressure_ICAO,1) + "hPa");
      delay(100);

  } else if (millis() - ms_pir_lastseen > ms_lcd_on_for and lcd_status) {
    lcd_status = false;
    lcd.off();
    lcd.noDisplay();
    lcd.noBacklight();
    digitalWrite(LED_RED, LOW); //turn off LED
    digitalWrite(LED_BLUE, LOW); //turn off LED
    delay(200);
  }


  
     
 if (millis() - ms_mqtt_lastsent >= 30E3 or ms_mqtt_lastsent == 0){
     read_sensors();
     save_pressure_history();
     if (not isnan(pressure_ICAO) and pressure_ICAO >= 500 and pressure_ICAO <= 2000 ) {
        Serial.println(client.publish(mqtt_t_temp_state, String(baro_temperature).c_str(), true)) ;      
          Serial.println(client.publish(mqtt_t_baro_state, String(pressure_ICAO).c_str(), true)) ;      
     }
     ms_mqtt_lastsent = millis();
  }

 
}
