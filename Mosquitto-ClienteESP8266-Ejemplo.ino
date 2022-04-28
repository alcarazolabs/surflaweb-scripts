//Instalar librería: PubSubClient
//El autor de la librería PubSubClient es Nick O'Leary
//https://c2plabs.com/blog/2022/01/02/esp8266-mqtt-client-tutorial-using-pubsubclient-library/
//https://www.youtube.com/watch?v=WQURoN5IXf0

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED 13

char *server = "192.168.0.11";
int port = 1883;

const char *ssid="WLAN_WAL -1";
const char *passwd = "942918777";

char serial_command = -1;
unsigned long previousMillis = 0;
unsigned long interval = 30000;

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
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.println ("Connected to WiFi AP, Got an IP address :");
  Serial.println (WiFi.localIP());
  //Set The ESP8266 tries to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  mqttClient.setServer (server, port);
  mqttClient.setCallback(mqttCallback);
  //Realizar conexión al MQTT Broker
  if(mqttClient.connect("ESP-Client",NULL,NULL)) {
    Serial.println ("Connected to MQTT Broker");
    
  } else {
    
    Serial.println("MQTT Broker connection failed");
    Serial.println (mqttClient.state());
    delay(200);
    
  }
 
  //suscribirse a los temas.
  mqttClient.subscribe("/casa/foco");
  mqttClient.subscribe("/galpon/foco");

}

void loop() {

 
  printWifiStatus();

  if(Serial.available()>0){

     serial_command = Serial.read();
     if(serial_command == '1'){
        //Publicar mensaje en un topico:
        //QOS del PubSubClient es = 0
        mqttClient.publish("/casa/foco", "El foco se prendio");
     }
  }

  //verificar si no hay conexión con el broker, si es así reconectarse:
   if(!mqttClient.connected()) {
      reconnect();
  }


  
  mqttClient.loop();
}


void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Trying to connect to the MQTT broker...");

    if (mqttClient.connect("ESP-Client",NULL,NULL)) {
      Serial.println("Connected to MQTT Broker");

      //suscribirse a los temas.

       mqttClient.subscribe("/casa/foco");
       mqttClient.subscribe("/galpon/foco");

      
    } else {
      Serial.print("Fallo, rc=");
      Serial.println(mqttClient.state());
      Serial.println("Trying to connect each 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    printWifiStatus();
  }
}


void printWifiStatus(){
      //print the Wi-Fi status every 30 seconds
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >=interval){
      switch (WiFi.status()){
        case WL_NO_SSID_AVAIL:
          Serial.println("Wifi Configured SSID cannot be reached");
          break;
        case WL_CONNECTED:
          Serial.println("Connection Wifi successfully established");
          break;
        case WL_CONNECT_FAILED:
          Serial.println("Wifi Connection failed");
          break;
      }
      Serial.printf("Connection status: %d\n", WiFi.status());
      Serial.print("RRSI: ");
      Serial.println(WiFi.RSSI());
      previousMillis = currentMillis;
  }
  }
