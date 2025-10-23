/*
* 
* Board: ESP32-S3 Dev Module (ESP32 Boards lib version tested: 3.0.6)
* Partition Schema: Huge App (3MB No OTA / 1MB SPIFFS)
* PSRAM: OPI PSRAM
* Erase all flash: Enable/Disabled
* Tutorial sobre como crear el Api Key: https://youtu.be/ZZyn1sYE2hM
*/
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_heap_caps.h>
#include "driver/i2s.h"

// WiFi credentials
const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxxx";

// ElevenLabs API configuration
const char* elevenlabs_api_key = "xxxxxxxxxxxx";
const char* elevenlabs_stt_url = "https://api.elevenlabs.io/v1/speech-to-text";

// Audio recording settings
#define SAMPLE_RATE 16000
#define SAMPLE_BITS 16
#define WAV_HEADER_SIZE 44
#define VOLUME_GAIN 2
#define MAX_RECORD_TIME 30  // segundos

// Pines Microfono INMP441 para ESP32-S3
#define I2S_WS            4
#define I2S_SD            6
#define I2S_SCK           5

// Pin del botón
#define BUTTON_PIN 13

// Pines LEDs

const int LED1 = 1;
const int LED2 = 2;

// I2S Configuration
#define I2S_PORT I2S_NUM_0

// PSRAM buffer para almacenar el audio
uint8_t* psram_audio_buffer = NULL;
size_t psram_buffer_size = 0;
const size_t MAX_AUDIO_SIZE = 1024 * 1024; // 1MB máximo

// Global variables
bool recording_active = false;
bool isPressed = false;

bool wifi_connected = false;

// ===== DECLARACIÓN DE FUNCIONES =====
bool connectToWiFi();
void initI2S();
void deinitI2S();
void record_wav_to_psram();
void process_recording();
String send_to_elevenlabs_stt();
void generate_wav_header(uint8_t* wav_header, uint32_t wav_size, uint32_t sample_rate);
bool init_psram();

// ===== IMPLEMENTACIÓN =====

bool init_psram() {
  
  // Verificar si PSRAM está disponible
  if (!psramFound()) {
    Serial.println("ERROR: PSRAM no encontrada!");
    return false;
  }
  
  size_t psram_size = ESP.getPsramSize();
  Serial.printf("PSRAM encontrada: %d bytes (%.2f MB)\n", psram_size, psram_size / 1024.0 / 1024.0);
  
  // Reservar buffer en PSRAM
  psram_audio_buffer = (uint8_t*)ps_malloc(MAX_AUDIO_SIZE);
  if (psram_audio_buffer == NULL) {
    Serial.println("ERROR: No se pudo asignar memoria PSRAM!");
    return false;
  }
  
  Serial.printf("Buffer PSRAM asignado: %d bytes (%.2f MB)\n", MAX_AUDIO_SIZE, MAX_AUDIO_SIZE / 1024.0 / 1024.0);
  return true;
}

void initI2S() {
  Serial.println("Inicializando I2S para INMP441...");
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };
  
  esp_err_t result = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (result != ESP_OK) {
    Serial.printf("ERROR: No se pudo instalar driver I2S (error %d)\n", result);
    return;
  }
  
  result = i2s_set_pin(I2S_PORT, &pin_config);
  if (result != ESP_OK) {
    Serial.printf("ERROR: No se pudieron configurar pines I2S (error %d)\n", result);
    return;
  }
  
  // Limpiar buffer DMA
  i2s_zero_dma_buffer(I2S_PORT);
  
  Serial.println("✅ I2S INMP441 inicializado correctamente");
}

void deinitI2S() {
  i2s_driver_uninstall(I2S_PORT);
  Serial.println("I2S desininstalado");
}

bool connectToWiFi() {
  Serial.println("Conectando a WiFi...");
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    wifi_connected = true;
    return true;
  } else {
    Serial.println("\n❌ Conexión WiFi fallida");
    wifi_connected = false;
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  //Setup led pins
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  
  Serial.println("\n╔════════════════════════════════════════════════════╗");
  Serial.println("║   ESP32-S3 Audio Recording to ElevenLabs STT     ║");
  Serial.println("╚════════════════════════════════════════════════════╝\n");
  
  // Configurar pin del botón
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.printf("🔘 Botón configurado en GPIO %d\n", BUTTON_PIN);

  // Inicializar PSRAM
  if (!init_psram()) {
    Serial.println("❌ ERROR CRÍTICO: No se pudo inicializar PSRAM!");
    while (1) {
      delay(1000);
    }
  }

  // Inicializar I2S
  initI2S();

  // Conectar a WiFi
  connectToWiFi();
  
  Serial.println("\n✅ Sistema listo. Presiona el botón para grabar.\n");
}

void loop() {
  bool currentState = digitalRead(BUTTON_PIN) == LOW;

  if (currentState && !isPressed) {
    isPressed = true;
    Serial.println("\n>>> 🔴 Botón presionado → Iniciando grabación...");
    record_wav_to_psram();
  }

  if (!currentState && isPressed) {
    isPressed = false;
    Serial.println(">>> ⏹ Botón liberado → Procesando audio...");
    process_recording();
  }

  delay(50);
}

void record_wav_to_psram() {
  if (psram_audio_buffer == NULL) {
    Serial.println("ERROR: PSRAM no inicializada");
    return;
  }

  // Generar encabezado WAV temporal (se actualizará después)
  uint8_t wav_header[WAV_HEADER_SIZE];
  generate_wav_header(wav_header, 0, SAMPLE_RATE);
  memcpy(psram_audio_buffer, wav_header, WAV_HEADER_SIZE);

  // Buffer temporal para lectura I2S
  const size_t buffer_size = 1024;
  uint8_t* buffer = (uint8_t*)malloc(buffer_size);
  if (!buffer) {
    Serial.println("ERROR: No se pudo asignar buffer temporal");
    return;
  }

  recording_active = true;
  size_t total_bytes = 0;
  size_t write_position = WAV_HEADER_SIZE;
  unsigned long startTime = millis();

  Serial.println("🎙 Grabando... (mantén presionado el botón)");

  // Limpiar buffer I2S antes de empezar
  i2s_zero_dma_buffer(I2S_PORT);

  while (digitalRead(BUTTON_PIN) == LOW && (millis() - startTime < MAX_RECORD_TIME * 1000)) {
    // Verificar que no excedamos el buffer
    if (write_position + buffer_size >= MAX_AUDIO_SIZE) {
      Serial.println("⚠ ADVERTENCIA: Buffer PSRAM lleno!");
      break;
    }

    size_t bytes_read = 0;
    esp_err_t result = i2s_read(I2S_PORT, buffer, buffer_size, &bytes_read, portMAX_DELAY);
    
    if (result != ESP_OK || bytes_read == 0) {
      continue;
    }

    // Aplicar ganancia de volumen
    for (size_t i = 0; i < bytes_read; i += 2) {
      int16_t* sample = (int16_t*)&buffer[i];
      int32_t amp = (*sample) << VOLUME_GAIN;
      if (amp > 32767) amp = 32767;
      if (amp < -32768) amp = -32768;
      *sample = (int16_t)amp;
    }

    // Escribir directamente en PSRAM
    memcpy(psram_audio_buffer + write_position, buffer, bytes_read);
    write_position += bytes_read;
    total_bytes += bytes_read;
  }

  recording_active = false;
  free(buffer);

  // Actualizar encabezado WAV con el tamaño real
  generate_wav_header(wav_header, total_bytes, SAMPLE_RATE);
  memcpy(psram_audio_buffer, wav_header, WAV_HEADER_SIZE);
  
  psram_buffer_size = total_bytes + WAV_HEADER_SIZE;

  float duration = (millis() - startTime) / 1000.0;
  Serial.printf("✅ Grabación finalizada: %.2f seg, %d bytes (%.2f KB)\n", 
                duration, psram_buffer_size, psram_buffer_size / 1024.0);
}

void process_recording() {
  if (psram_buffer_size == 0) {
    Serial.println("⚠  No hay audio para procesar");
    return;
  }

  Serial.println("\n📤 Enviando audio a ElevenLabs STT...");
  String transcription = send_to_elevenlabs_stt();

  if (transcription.length() > 0) {
    Serial.println("\n╔════════════════════════════════════════════════════╗");
    Serial.println("║              ✅ TRANSCRIPCIÓN                      ║");
    Serial.println("╚════════════════════════════════════════════════════╝");
    Serial.println(transcription);
    Serial.println("════════════════════════════════════════════════════\n");
    
    transcription.toLowerCase(); // Convertir todo el texto en minusculas
    
    String comando = transcription; 
    if(comando == "enciende luz uno." || comando == "enciende luz 1."){
      digitalWrite(LED1, HIGH);
    }else if(comando == "enciende luz dos." || comando == "enciende luz 2."){
      digitalWrite(LED2, HIGH);
    }else if(comando == "apaga luz uno." || comando == "apaga luz 1."){
      digitalWrite(LED1, LOW);
    }else if(comando == "apaga luz dos." || comando == "apaga luz 2."){
      digitalWrite(LED2, LOW);
    }else{
      //no hacer nada.
    }

    
    
  } else {
    Serial.println("❌ Error en la transcripción STT\n");
  }

  // Limpiar buffer
  psram_buffer_size = 0;
}

String send_to_elevenlabs_stt() {
  uint32_t t_start = millis();
  
  if (!wifi_connected || WiFi.status() != WL_CONNECTED) {
    Serial.println("ERROR: WiFi no conectado");
    return "";
  }

  if (psram_buffer_size == 0 || psram_audio_buffer == NULL) {
    Serial.println("ERROR: No hay datos de audio en PSRAM");
    return "";
  }

  if (psram_buffer_size > 500000) {
    Serial.printf("ERROR: Archivo demasiado grande (%d bytes > 500KB)\n", psram_buffer_size);
    return "";
  }

  uint32_t t_data_ready = millis();

  HTTPClient http;
  if (!http.begin(elevenlabs_stt_url)) {
    Serial.println("ERROR: No se pudo inicializar conexión HTTP");
    return "";
  }

  http.setTimeout(30000);
  http.setConnectTimeout(10000);
  http.addHeader("xi-api-key", elevenlabs_api_key);

  String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

  // Construir el cuerpo de la petición
  String body_start = "--" + boundary + "\r\n";
  body_start += "Content-Disposition: form-data; name=\"model_id\"\r\n\r\n";
  body_start += "scribe_v1\r\n";
  body_start += "--" + boundary + "\r\n";
  body_start += "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n";
  body_start += "Content-Type: audio/wav\r\n\r\n";

  String body_end = "\r\n--" + boundary + "--\r\n";
  size_t total_size = body_start.length() + psram_buffer_size + body_end.length();

  // Crear buffer completo para la petición
  uint8_t* complete_body = (uint8_t*)malloc(total_size);
  if (!complete_body) {
    Serial.println("ERROR: No se pudo asignar memoria para petición HTTP");
    http.end();
    return "";
  }

  memcpy(complete_body, body_start.c_str(), body_start.length());
  memcpy(complete_body + body_start.length(), psram_audio_buffer, psram_buffer_size);
  memcpy(complete_body + body_start.length() + psram_buffer_size, body_end.c_str(), body_end.length());

  uint32_t t_request_prepared = millis();

  Serial.println("📡 Enviando petición HTTP...");

  // Enviar petición
  uint32_t t_request_sent = millis();
  int httpResponseCode = http.POST(complete_body, total_size);
  uint32_t t_response_received = millis();

  free(complete_body);

  String transcription = "";
  String response = http.getString();

  uint32_t t_response_parsed = millis();

  if (httpResponseCode == 200) {
    Serial.println("✅ HTTP 200 OK");
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
      Serial.printf("ERROR: No se pudo parsear JSON: %s\n", error.c_str());
      Serial.println("Respuesta recibida: " + response);
    } else {
      if (doc.containsKey("text")) {
        transcription = doc["text"].as<String>();
      } else {
        Serial.println("ERROR: Respuesta no contiene campo 'text'");
        Serial.println("Respuesta: " + response);
      }
    }
  } else {
    Serial.printf("❌ ERROR HTTP: %d\n", httpResponseCode);
    Serial.println("Respuesta: " + response);
  }

  http.end();

  // Imprimir información de latencia detallada
  Serial.println("\n╔════════════════════════════════════════════════════╗");
  Serial.println("║           📊 ESTADÍSTICAS DE RENDIMIENTO          ║");
  Serial.println("╚════════════════════════════════════════════════════╝");
  Serial.printf("📊 Tamaño audio:                       %d bytes (%.2f KB)\n", 
                psram_buffer_size, psram_buffer_size / 1024.0);
  Serial.printf("⏱ Latencia preparación datos:        %.3f seg\n", 
                (float)(t_data_ready - t_start) / 1000);
  Serial.printf("⏱ Latencia preparación request:      %.3f seg\n", 
                (float)(t_request_prepared - t_data_ready) / 1000);
  Serial.printf("⏱ Latencia respuesta ElevenLabs:     %.3f seg\n", 
                (float)(t_response_received - t_request_sent) / 1000);
  Serial.printf("⏱ Latencia parseo respuesta:         %.3f seg\n", 
                (float)(t_response_parsed - t_response_received) / 1000);
  Serial.printf("⏰ DURACIÓN TOTAL:                     %.3f seg\n", 
                (float)(t_response_parsed - t_start) / 1000);
  Serial.printf("📦 Longitud respuesta servidor:        %d bytes\n", response.length());
  Serial.println("════════════════════════════════════════════════════\n");

  return transcription;
}

void generate_wav_header(uint8_t* wav_header, uint32_t wav_size, uint32_t sample_rate) {
  uint32_t file_size = wav_size + WAV_HEADER_SIZE - 8;
  uint32_t byte_rate = sample_rate * SAMPLE_BITS / 8;

  const uint8_t header[] = {
    'R', 'I', 'F', 'F',
    (uint8_t)(file_size & 0xFF), (uint8_t)((file_size >> 8) & 0xFF), 
    (uint8_t)((file_size >> 16) & 0xFF), (uint8_t)((file_size >> 24) & 0xFF),
    'W', 'A', 'V', 'E',
    'f', 'm', 't', ' ',
    0x10, 0x00, 0x00, 0x00,  // Tamaño del chunk fmt
    0x01, 0x00,              // Audio format (1 = PCM)
    0x01, 0x00,              // Número de canales (1 = mono)
    (uint8_t)(sample_rate & 0xFF), (uint8_t)((sample_rate >> 8) & 0xFF),
    (uint8_t)((sample_rate >> 16) & 0xFF), (uint8_t)((sample_rate >> 24) & 0xFF),
    (uint8_t)(byte_rate & 0xFF), (uint8_t)((byte_rate >> 8) & 0xFF),
    (uint8_t)((byte_rate >> 16) & 0xFF), (uint8_t)((byte_rate >> 24) & 0xFF),
    0x02, 0x00,              // Block align
    0x10, 0x00,              // Bits por muestra (16)
    'd', 'a', 't', 'a',
    (uint8_t)(wav_size & 0xFF), (uint8_t)((wav_size >> 8) & 0xFF),
    (uint8_t)((wav_size >> 16) & 0xFF), (uint8_t)((wav_size >> 24) & 0xFF),
  };
  
  memcpy(wav_header, header, sizeof(header));
}
