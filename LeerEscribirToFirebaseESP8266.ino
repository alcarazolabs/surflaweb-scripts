// By surflaweb - Youtube | @alcarazolabs - Github
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Set these to run example.
#define FIREBASE_HOST "xxxxxxxxxx.firebaseio.com"
#define FIREBASE_AUTH "xxxxxxxxx"
#define WIFI_SSID "MyRed"
#define WIFI_PASSWORD "clave123456"

// Declare the Firebase Data object in the global scope
FirebaseData firebaseData;

// Declare global variable to store value
double sensor_movimiento = 0;

void setup() {
  Serial.begin(9600);                                   // Select the same baud rate if you want to see the datas on Serial Monitor

  Serial.println("Serial communication started\n\n");  
           
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                 //try to connect with wifi
  Serial.print("Conectado A ");
  Serial.print(WIFI_SSID);


  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Conectado A: ");
  Serial.println(WIFI_SSID);
  Serial.print("Diricción IP : ");                 //print local IP address
  Serial.println(WiFi.localIP());                                            
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect to firebase
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.reconnectWiFi(true);
  delay(1000);
  Serial.println("Leendo temperatura del servidor");
  if (Firebase.RTDB.getInt(&firebaseData, "/casa/temperatura")) {

    if (firebaseData.dataType() == "int") {
      sensor_movimiento = firebaseData.intData();
      
      Serial.println(firebaseData.intData());
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }
}

void loop() {
 //update data
sensor_movimiento++;
 
if (Firebase.setInt(firebaseData, "/casa/temperatura", sensor_movimiento)) {    // On successful Write operation, function returns 1  
               Serial.println("Temperature updated");
               delay(1000);

  }else {        
    Serial.println(firebaseData.errorReason());
  }

  
  //Esperar 10 segundos para volver a actualizar..
  delay(10000);


}
