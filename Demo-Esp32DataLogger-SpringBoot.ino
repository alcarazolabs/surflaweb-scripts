/*
* ESP32-LOGGER-TO-GOOGLE-SHEETS
* Version estable
*/

#include <HTTPClient.h>

unsigned long timeRead = 0;

const char* ssid = "WILLIAN-2.4G-pJAt"; //--> Your wifi name or SSID.
const char* password = "942900777"; //--> Your wifi password.

const char* serverName = "http://192.168.18.42:7777/api/esp32"; //GOOGLE APPS SCRIPT URL
void setup() {
  
  Serial.begin(115200);

  initWifi();
}

void loop() {

  if(millis() - timeRead > 2000){ //leer lectura del sensor cada 2 segundos.
    
     auto randomTemperature = random(18,40); // random number between 18-40
     auto randomHumidity = random(30, 100); // random number between 30-100 
     Serial.print("Temperature = ");
     Serial.print(randomTemperature);
     Serial.println(" *C");
     Serial.print("Humidity = ");
     Serial.print(randomHumidity);
     Serial.println(" %");

      sendToSpringBoot(randomTemperature, randomHumidity);
    
     timeRead = millis();
  }
       
}


void initWifi(){
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED && (timeout-- > 0)){

    delay(250);
    Serial.print(".");
  }

  Serial.println("");

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Failed to Connect!");
  }

  Serial.print("WiFi connected with IP Address: ");
  Serial.println(WiFi.localIP());
  
  
}

//subrutina
void sendToSpringBoot(float temp, int hum){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
  
    String jsonData = "{\"temperature\":\"" + String(temp) + "\", \"humidity\":\"" + String(hum)+ "\"}";

    int httpResponseCode = http.POST(jsonData);

    if(httpResponseCode > 0){
       String response = http.getString();
       Serial.println(httpResponseCode);
       Serial.println(response);
    } else {
      Serial.print("Wrong Request POST: ");
      Serial.println(httpResponseCode);
    }

  http.end();
    
    
  }

}
