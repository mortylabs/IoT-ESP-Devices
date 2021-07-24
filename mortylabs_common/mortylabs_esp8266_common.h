#ifndef mortylabs_common_h
#define mortylabs_common_h

#include "Arduino.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <secrets.h>

typedef struct topicArray {
   char* unique_id;
   char* name;
   char* unit_of_measurement;
   char* topic_config;
   char* topic_state;
} topicArray_t;


class MyCommon {
  public:
	ESP8266WiFiMulti wifiMulti;
	
    MyCommon(char* hostname, PubSubClient mqttClient, char* mqtt_topic_heartbeat, topicArray_t mqtt_topics[], boolean debug, boolean battery, float deep_sleep_ms);
	
	void   				begin();
    void   				connect_wifi();
	String 				get_uptime();
	unsigned long 		get_elapsed_secs(unsigned long _start_millis);
	int  				get_battery_perc();
	int  				get_battery_volts();

    String 				getHandleRootHeader(String src_file, boolean html);
	String 				getWifiNetworkList();
	boolean				mqtt_reconnect();
	void				print_debug(char* msg);

  private:
	void				check_battery();
	char* 				_hostname;
	char*				_ssid;
	char*				_ssid_pass;
	boolean 			_debug;
	unsigned long 		_millis_start;
	String  			_esp_reset_reason = "unknown";
	boolean				_battery_enabled;
	int					_battery_perc;
	int					_battery_volts;
	unsigned long		_battery_last_checked;
	int					_analog_value;
	float				_deep_sleep_ms = 0;
	PubSubClient		_mqttClient;
	char*				_mqtt_topic_heartbeat;
	topicArray_t 		_mqtt_topics[];
};


#endif
