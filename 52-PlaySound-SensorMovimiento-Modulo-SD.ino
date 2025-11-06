/*
* Library used:  ESP32-AudioI2S v.2.0.0 https://github.com/schreibfaul1/ESP32-audioI2S
*/
#include "Audio.h"
#include <SD.h>
#include "FS.h"

//Definir pines para lector de memoria SD Card Module:
// vcc -> 5v
#define SD_MISO     19
#define SD_MOSI     23
#define SD_SCLK     18
#define SD_CS       5

//MAX98357 PINS:
//vin -> 3.3v
#define I2S_DOUT      25 //DIN PIN
#define I2S_BCLK      27
#define I2S_LRC       26

Audio audio;
void setup(){
  Serial.begin(115200);
  
  pinMode(SD_CS, OUTPUT);
  
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
  
  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed!");
    return;
  }
  Serial.println("SD mounted OK");
  
  // Setup amplificador:
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(21); // 0...21
  
  Serial.println("MAX98357 Setup OK");
  
  audio.connecttoFS(SD, "alarma.mp3"); //Reproducir archivo alojado en la tarjeta SD
  
}

void loop(){
  audio.loop();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}
