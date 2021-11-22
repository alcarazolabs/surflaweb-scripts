// By surflaweb - Youtube | @alcarazolabs - Github
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Set these to run example.
#define FIREBASE_HOST "myfirstiotv2-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "1busiJZ9bMWptrygatRw1qI15sYtgEEhP37WFYv4"
#define WIFI_SSID "Tokyo"
#define WIFI_PASSWORD "942918777"

// Declare the Firebase Data object in the global scope
FirebaseData firebaseData;

// Declare global variable to store value
double temperature_sensor=0;
double humidity_sensor=0;

void setup() {
  Serial.begin(9600);                                   // Select the same baud rate if you want to see the datas on Serial Monitor

  Serial.println("Serial communication started\n\n");  
           
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
  Serial.print("Conectado A ");
  Serial.print(WIFI_SSID);


  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Conectado A: ");
  Serial.println(WIFI_SSID);
  Serial.print("Diricción IP : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect to firebase
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.reconnectWiFi(true);
  delay(1000);
}

void loop() {
 //update data
if (Firebase.setDouble(firebaseData, "/casa/temperatura", temperature_sensor)) {    // On successful Write operation, function returns 1  
               Serial.println("Temperature updated");
              
               
               temperature_sensor++;
               delay(1000);

  }else {        
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setDouble(firebaseData, "/casa/humedad", humidity_sensor)) {    // On successful Write operation, function returns 1  
               Serial.println("Humidity updated");
               humidity_sensor++;
               delay(1000);

  }else {        
    Serial.println(firebaseData.errorReason());
  }

  //Log the data
  
  if (Firebase.pushDouble(firebaseData, "/casa_log/temperatura", temperature_sensor)) {    // On successful Write operation, function returns 1  
               Serial.println("Temperature updated");
              
               
               temperature_sensor++;
               delay(1000);

  }else {        
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.pushDouble(firebaseData, "/casa_log/humedad", humidity_sensor)) {    // On successful Write operation, function returns 1  
               Serial.println("Humidity updated");
               humidity_sensor++;
               delay(1000);

  }else {        
    Serial.println(firebaseData.errorReason());
  }


  
  //Esperar 5 segundos para volver a actualizar..
  delay(5000);


}
