#include "Arduino.h"
#include <secrets.h>
#include "mortylabs_esp8266_common.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>


#include <ArduinoOTA.h>

//https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
//https://randomnerdtutorials.com/power-esp32-esp8266-solar-panels-battery-level-monitoring/
//https://www.engineersgarage.com/esp8266/nodemcu-battery-voltage-monitor/
//https://i1.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/04/voltage-divider-battery-level.png?ssl=1
//https://www.google.com/search?q=wemos+d1+mini+pin+numbers&rlz=1C1CHBF_en-GBGB900GB900&sxsrf=ALeKk02095OPPh7549Qa0TajX8Sl5qPvmA:1598383048645&tbm=isch&source=iu&ictx=1&fir=rlOU9h7q2xv-VM%252CATCX2EsURb-ZQM%252C_&vet=1&usg=AI4_-kTa4VJtkfshyKzcq9o-byFqPw0_xA&sa=X&ved=2ahUKEwia99DuiLfrAhWymFwKHRylDNYQ9QEwAXoECAwQHQ&biw=1536&bih=754#imgrc=rlOU9h7q2xv-VM
//https://www.google.com/search?q=nodemcu+pin+mapping&rlz=1C1CHBF_en-GBGB900GB900&sxsrf=ALeKk01xR4ORn3KbWGN_-4Ej_Pk1V6Ca3Q:1598383086958&tbm=isch&source=iu&ictx=1&fir=bF3MDYvnicyjeM%252CATCX2EsURb-ZQM%252C_&vet=1&usg=AI4_-kTi6x3AKXpF8CcjqkCqwjc0UI0T5A&sa=X&ved=2ahUKEwjMtfOAibfrAhWimFwKHUkODHIQ9QEwAHoECAoQIQ&biw=1536&bih=754#imgrc=bF3MDYvnicyjeM
//https://forum.arduino.cc/index.php?topic=420527.0



MyCommon::MyCommon(char* hostname, PubSubClient mqttClient, char* mqtt_topic_heartbeat, topicArray_t mqtt_topics[], boolean debug,  boolean battery, float deep_sleep_ms = 0)
{
  _hostname 			= hostname;
  _ssid					= WIFI_SSID;
  _ssid_pass			= WIFI_PASS;
  _debug    			= debug;
  _millis_start 		= millis();
  _battery_last_checked = millis()- 300e3; 
  _battery_enabled		= battery;
  _battery_perc 		= -999;
  _battery_volts 		= -999;
  _deep_sleep_ms		= deep_sleep_ms;
  _mqttClient			= mqttClient;
  _mqtt_topic_heartbeat = mqtt_topic_heartbeat;
  memcpy(_mqtt_topics, mqtt_topics, sizeof(mqtt_topics));
  wifiMulti.addAP(_ssid, _ssid_pass);
}

	
boolean MyCommon::mqtt_reconnect() {
  unsigned long ms_started = millis();
  
  while (!_mqttClient.connected() and (millis() - ms_started <= 10E3)) {
      if (_debug) {Serial.print("Attempting MQTT connection...");}
    if (_mqttClient.connect( _hostname, MQTT_USER, MQTT_PASS)) {
      if (_debug) {Serial.println("publishing Home Assistant auto discovery...");}
	  
	  String payload = "";

	  for (int x = 0; x <= sizeof(_mqtt_topics)/sizeof(_mqtt_topics[0]); x++) {
		payload = "{\"name\":\"";
	    payload = payload + _mqtt_topics[x].name+"\",\"stat_t\":\"";
        payload = payload + _mqtt_topics[x].topic_state + "\",\"avty_t\":\"" + _mqtt_topic_heartbeat + "\",\"unit_of_measurement\":\""+_mqtt_topics[x].unit_of_measurement+"°C\", \"uniq_id\":\""+_mqtt_topics[x].unique_id+"\"}";;
        Serial.println(_mqttClient.publish(_mqtt_topics[x].topic_config, payload.c_str(), true));

      }
      
      if (_debug) {Serial.println("publishing /avail as online...");}
      Serial.println(_mqttClient.publish(_mqtt_topic_heartbeat, "online", true));
      
    } else {
	    if (_debug) {
		Serial.print("failed, rc=");
		Serial.print(_mqttClient.state());
		Serial.println(". retrying...");
		}
		delay(500);
    }

  }
}

void MyCommon::begin()
{
  if (_debug) {Serial.println("MyCommon:begin -> debug is ENABLED");}
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(_hostname);
  
  connect_wifi();
    
  ArduinoOTA.setHostname(_hostname);
  ArduinoOTA.setPassword(_ssid_pass);
  ArduinoOTA.begin();
}

void MyCommon::connect_wifi()
{
  //if ( _debug ) { Serial.println("MyCommon:connect_wifi() - checking..."); }
  
  unsigned long led_status  = 1;
  unsigned int  wifi_status_initial = wifiMulti.run();
  unsigned long connect_start_millis = millis();
  if (_debug and wifi_status_initial != WL_CONNECTED) {
  	  Serial.print("MyCommon:connect_wifi() -> not connected ");}
  WiFi.setOutputPower(20.5);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(200);
    if (_debug) { Serial.print("."); }
	if (get_elapsed_secs(connect_start_millis) >= 15) {
	  if (_deep_sleep_ms > 0) { 
			if (_debug ) {Serial.print("MyCommon:connect_wifi() -> WiFi FAIL, deep sleep for "); Serial.println(_deep_sleep_ms /1000000.00);}
			ESP.deepSleep(_deep_sleep_ms );
	  }
	  else {
			if (_debug ) {Serial.print("MyCommon:connect_wifi() -> ESP restart after "); }
			delay(100);
			ESP.restart();
	  }
    }
    if (led_status == 1) {
      led_status = 0;
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      led_status = 1;
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  if (_debug) {
			String _msg = "still connected to ";
			if (wifi_status_initial != WL_CONNECTED) { Serial.println(""); _msg = "now connected to "; }
			Serial.print ("MyCommon:connect_wifi() -> ");
			Serial.print( _msg);
			Serial.print (WiFi.SSID());
			Serial.print (" ");
			Serial.print(WiFi.RSSI());
			Serial.print ("dBm : ");
			Serial.println (WiFi.localIP().toString());
	   }
}


String MyCommon::getHandleRootHeader(String src_file, boolean html) 
{
  String a;
  if (html) {a =a + "<b>";}
  a = a + "Morty Labs!";
  if (html) {a =a + "</b>";}
  a = a + "\n\napp     : ";
  if (html) {a =a + "<b>";}
  a = a + src_file;
  if (html) {a =a + "</b><BR>";}
  a = a + " deployed ";
  
  if (html) {a =a + "<b>";}
  a = a + __DATE__;
  a = a + " " ;
  a = a + __TIME__ ;
  if (html) {a =a + "</b>";}

  a = a + "\nwifi    : ";
  if (html) {a =a + "<b>";}
  a = a + WiFi.SSID();
  a = a + " ";
  a = a + WiFi.RSSI();
  a = a + " dB  ";
  if (html) {a =a + "</b>";}
  a = a + " (";
  a = a + WiFi.macAddress();
  a = a + ")";

  a = a + "\nstartup : ";
  a = a + ESP.getResetReason();
  
  if (_battery_enabled) {
	  a = a + "\nbattery : ";
	  a = a + get_battery_perc();
	  a = a + "%  ";
	  a = a + get_battery_volts();
	  a = a + "v  (A0 ";
	  a = a + _analog_value;
	  a = a + "v)";
  }
  a = a + "\n";
  a = a + get_uptime();

  a = a + "\n\n";
  if (html) { a.replace("\n", "<BR>"); }
  
  return a; 
}

unsigned long  MyCommon::get_elapsed_secs(unsigned long _start_millis) 
{
  return (millis() - _start_millis) / 1000;
}

String MyCommon::get_uptime() 
{
  unsigned long secs_elapsed = get_elapsed_secs(_millis_start);
  unsigned long hours = secs_elapsed / 60 / 60;
  unsigned long mins = secs_elapsed / 60.00 - hours * 60;
  unsigned long secs = secs_elapsed - hours * 60 * 60 - mins * 60;
  unsigned long days = hours / 24;
  hours = hours - days*24;
  String up_time = "uptime  : ";

  up_time = up_time + days;
  up_time = up_time + "d ";
  if (hours < 10) {
    up_time += "0";
  }
  up_time = up_time + hours;
  up_time = up_time + ":";
  if (mins < 10 ) {
    up_time += "0";
  }
  up_time = up_time + mins;
  up_time = up_time + ":";
  if (secs < 10 ) {
    up_time += "0";
  }
  up_time = up_time + secs;
  return up_time;
}


String MyCommon::getWifiNetworkList() 
{
	String a = "no networks found";
	int n = WiFi.scanNetworks();
	if (n > 0) {
		a = n;
		a = a + " networks found\n";
	
		int indices[n];
		for (int i = 0; i < n; i++) {
			indices[i] = i;
		}
    
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
					std::swap(indices[i], indices[j]);
				}
			}
		}
		
		for (int i = 0; i < n; ++i) {
			a = a + "\n";
			String b = WiFi.SSID(indices[i]);
			while (b.length() < 15) { b = b + " "; }
			a = a + b;
			a = a + WiFi.RSSI(indices[i]);
			a = a + " dBm";
			if (WiFi.encryptionType(indices[i]) == ENC_TYPE_NONE) {
				a = a + " non-encrypted network";
			}
		}
	}
    if (_debug) {Serial.println(a);   }
    return a;
	
}


void MyCommon::check_battery() 
{
  if (_battery_enabled and _battery_last_checked + 10e3 > millis() ) {
	  _analog_value = analogRead(0);
	  _battery_last_checked = millis();
      _battery_volts  = map(_analog_value, 0.0f, 1024.0f, 0, 4200.0f);
      _battery_perc   = map(_battery_volts, 2600.0f, 4200.0f, 0, 100);
      if (_debug) {
			Serial.print("battery analog: ");
			Serial.print (_analog_value);
			Serial.print("  battery% :  ");
			Serial.print(_battery_perc);
			Serial.print("  volt calc:  ");
			Serial.println(_battery_volts);
	  }
  }
}


int MyCommon::get_battery_perc() 
{
	int res =	-99;
	if (_battery_enabled) {
		check_battery();
		res =  _battery_perc;
	}
	return res;
}

int MyCommon::get_battery_volts() 
{
	int res = -99;
	if (_battery_enabled) {
		check_battery();
	    res =  _battery_volts;
	}
	return res;
}
