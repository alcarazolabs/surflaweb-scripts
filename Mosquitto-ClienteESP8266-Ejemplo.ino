//Instalar librería: PubSubClient
//El autor de la librería PubSubClient es Nick O'Leary
//https://c2plabs.com/blog/2022/01/02/esp8266-mqtt-client-tutorial-using-pubsubclient-library/
//https://www.youtube.com/watch?v=WQURoN5IXf0

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED 13

const char *ssid="WLAN_WAL -1";
const char *passwd = "942918777";

WiFiClient wlanclient;
PubSubClient mqttClient(wlanclient);

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.println ("Message arrived on Topic:");
  Serial.println (topic);

  char message[5]={0x00};

  for(int i=0;i<length;i++)
  message[i]=(char)payload[i];

  message[length]=0x00;
  Serial.println (message);
 String str_msg = String(message);
 if(str_msg.equals ("ON")) {
  
   Serial.println(str_msg);
   
 }else if (str_msg.equals("OFF")) {
  
   Serial.println(str_msg);
   
 }

}

void setup() {
  Serial.begin (115200);
  WiFi.begin(ssid,passwd);

  Serial.print ("Connecting to AP");
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.println (".");
    delay(100);
  }
  Serial.println ("Connected to WiFi AP, Got an IP address :");
  Serial.println (WiFi.localIP());

  mqttClient.setServer ("192.168.0.11",1883);
  mqttClient.setCallback(mqttCallback);

  if(mqttClient.connect ("ESP-Client",NULL,NULL)) {

    Serial.println ("Connected to MQTT Broker");
  } else {
    Serial.println("MQTT Broker connection failed");
    Serial.println (mqttClient.state());
    delay(200);
  }

  mqttClient.subscribe("/casa/foco");
  mqttClient.subscribe("/galpon/foco");

}

void loop() {
  mqttClient.loop();
}
