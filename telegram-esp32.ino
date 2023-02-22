#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> //Version Instalada: 1.3.0 depende de ArduinoJson instalar version 6.15.2

// Reemplazar con los datos de tu red wifi
#define WIFI_SSID "WILLIAN-2.4G-pJAt"
#define WIFI_PASSWORD "942900777"


//Token de Telegram BOT se obtenienen desde Botfather en telegram
#define BOT_TOKEN "6042934386:AAEpMY6E7WqFroNRKB55CKUCEDKBa729YhA"
const unsigned long tiempo = 1000; //tiempo medio entre mensajes de escaneo
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

unsigned long tiempoAnterior; //última vez que se realizó el análisis de mensajes
const int led1 = 05;
const int led2 = 18;
int estadoLed1 = 0;
int estadoLed2 = 0;
int inicio = 1;
String chat_id;

#define ID_Chat "1439102486"//ID_Chat se obtiene de telegram

void mensajesNuevos(int numerosMensajes)
{
  for (int i = 0; i < numerosMensajes; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    
//////////Luz 1 en el pin 5//////
    if (text == "Luz1on")
    {
      digitalWrite(led1, HIGH); // 
      estadoLed1 = 1;
      bot.sendMessage(chat_id, "Luz 1 encendida", "");
    }

    if (text == "Luz1off")
    {
      estadoLed1 = 0;
      digitalWrite(led1, LOW); // 
      bot.sendMessage(chat_id, "Luz 1 apagada", "");
    }
//////////Luz 2 en el pin 14//////
    if (text == "Luz2on")
    {
      digitalWrite(led2, HIGH); 
      estadoLed2 = 1;
      bot.sendMessage(chat_id, "Luz 2 encendida", "");
    }

    if (text == "Luz2off")
    {
      estadoLed2 = 0;
      digitalWrite(led2, LOW); 
      bot.sendMessage(chat_id, "Luz 2 apagada", "");
    }  

////////Estado de las luces  ///////     

    if (text == "Estado")
    {
      ////Estado luz 1////
      if (estadoLed1)
      {
        bot.sendMessage(chat_id, "Luz 1 encendida", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Luz 1 apagada", "");
      }
      ////Estado luz 2////
      if (estadoLed2)
      {
        bot.sendMessage(chat_id, "Luz 2 encendida", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Luz 2 apagada", "");
      }
    }

    if (text == "Ayuda")
    {
      String ayuda = "Bienvenido al sistema de control luces con Esp32, " ".\n";
      ayuda += "Estas son tus opciones.\n\n";
      ayuda += "Luz1on: para encender la Luz 1 \n";
      ayuda += "Luz1off: para apagar la luz 1 \n";
      ayuda += "Luz2on: para encender la Luz 2 \n";
      ayuda += "Luz2off: para apagar la luz 2 \n";
      ayuda += "Estado : devuelve el estado actual de las luces\n";
      ayuda += "Ayuda: Imprime este menú \n";
      ayuda += "Recuerda el sistema distingue entre mayuculas y minusculas \n";
      bot.sendMessage(chat_id, ayuda, "");
    }
    
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(led1, OUTPUT); //inicializar  pin 05 digital como salida.
  pinMode(led2, OUTPUT); //inicializar pin 18 digital como salida.
  
  digitalWrite(led1, LOW); //Apagamos el foco
  digitalWrite(led2, LOW); //Apagamos el foco
  
  // Intenta conectarse a la red wifi
  Serial.print("Conectando a la red ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); //Agregar certificado raíz para api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConectado a la red wifi. Dirección IP: ");
  Serial.println(WiFi.localIP());
   if(inicio == 1){
    Serial.println("Sistema preparado");
    bot.sendMessage(ID_Chat, "Sistema preparado!!!, escribe Ayuda para ver las opciones", "");//Enviamos un mensaje a telegram para informar que el sistema está listo
    inicio = 0;
    }
}

void loop()
{
  //Verifica si hay datos nuevos en telegram cada 1 segundo
  if (millis() - tiempoAnterior > tiempo)
  {
    int numerosMensajes = bot.getUpdates(bot.last_message_received + 1);

    while (numerosMensajes)
    {
      Serial.println("Comando recibido");
      mensajesNuevos(numerosMensajes);
      numerosMensajes = bot.getUpdates(bot.last_message_received + 1);
    }

    tiempoAnterior = millis();
  }
  
}
