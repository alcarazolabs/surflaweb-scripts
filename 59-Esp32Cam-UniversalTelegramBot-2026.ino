/*
 * ESP32-CAM Motion Detection with Telegram Notification
 * 
 * Description:
 *   - Detects motion using a PIR sensor connected to GPIO 13
 *   - Captures an image using the OV2640 camera on the ESP32-CAM (AI-Thinker)
 *   - Sends the captured image to a Telegram chat via Bot API
 * 
 * Hardware:
 *   - ESP32-CAM (AI-Thinker module)
 *   - PIR Motion Sensor (HC-SR501 or similar)
 *     - VCC  -> 3.3V or 5V (depending on sensor)
 *     - GND  -> GND
 *     - OUT  -> GPIO 13
 * 
 * Libraries Required:
 *   - Install "UniversalTelegramBot" by Brian Lough via Arduino Library Manager
 *   - Install "ArduinoJson" by Benoit Blanchon (v6.x) via Arduino Library Manager
 * 
 * Board Settings (Arduino IDE):
 *   - Board: "AI Thinker ESP32-CAM"
 *   - Partition Scheme: "Huge APP (3MB No OTA / 1MB SPIFFS)"
 * 
 * Setup Instructions:
 *  
 *   Steps to get Telegram credentials:
 *    1. Open Telegram → search @BotFather → /newbot
 *    2. Copy the BOT TOKEN given to you.
 *    3. Start a chat with your new bot, then open:
 *       https://api.telegram.org/bot<TOKEN>/getUpdates
 *       Find your "chat":{"id": XXXXXXXXX} value.
 *    4. Paste both values below.
 *   3. Fill in your WiFi credentials, Bot Token, and Chat ID below
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// ========================== USER CONFIGURATION ==========================
// WiFi Credentials
const char* ssid     = "**********";       // <-- Replace with your WiFi name
const char* password = "**********";    // <-- Replace with your WiFi password

// Telegram Bot Configuration
const char* botToken = "*********************";        // <-- Replace with your Bot Token from @BotFather
const char* chatId   = "**********";          // <-- Replace with your Chat ID

// ========================== PIN DEFINITIONS =============================
#define PIR_SENSOR_PIN   13   // PIR sensor output pin (GPIO 13)
#define FLASH_LED_PIN     4   // Onboard flash LED (GPIO 4 on AI-Thinker)

// ========================== CAMERA PIN DEFINITIONS ======================
// Pin definition for AI-Thinker ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ========================== TIMING CONFIGURATION ========================
#define MOTION_COOLDOWN_MS   10000  // 10 seconds cooldown between captures
#define BOT_CHECK_INTERVAL   1000   // Check for new Telegram messages every 1 second
#define WIFI_TIMEOUT_MS      20000  // WiFi connection timeout (20 seconds)

// ========================== GLOBAL VARIABLES ============================
WiFiClientSecure clientSecure;
UniversalTelegramBot bot(botToken, clientSecure);

unsigned long lastMotionTime  = 0;
unsigned long lastBotCheck    = 0;
bool flashEnabled             = true;   // Use flash LED when capturing
bool motionDetectionEnabled   = true;   // Motion detection toggle

// ========================== FUNCTION DECLARATIONS =======================
bool initCamera();
bool connectWiFi();
camera_fb_t* captureImage();
bool sendPhotoToTelegram(camera_fb_t* fb);
void handleNewMessages(int numNewMessages);
void flashLED(bool state);

// ========================================================================
//                              SETUP
// ========================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n\n========================================");
  Serial.println("  ESP32-CAM Motion Detection + Telegram");
  Serial.println("========================================\n");

  // Configure PIR sensor pin
  pinMode(PIR_SENSOR_PIN, INPUT);
  
  // Configure flash LED pin
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, LOW);

  // Initialize Camera
  if (!initCamera()) {
    Serial.println("[ERROR] Camera initialization failed! Restarting...");
    delay(3000);
    ESP.restart();
  }
  Serial.println("[OK] Camera initialized successfully.");

  // Connect to WiFi
  if (!connectWiFi()) {
    Serial.println("[ERROR] WiFi connection failed! Restarting...");
    delay(3000);
    ESP.restart();
  }

  // Configure secure client (skip certificate verification for simplicity)
  clientSecure.setInsecure();

  // Send startup notification to Telegram
  bot.sendMessage(chatId, "🟢 *ESP32-CAM Online!*\n\nMotion detection is active.\n\nCommands:\n/photo - Capture & send photo\n/flash\\_on - Enable flash\n/flash\\_off - Disable flash\n/motion\\_on - Enable motion detection\n/motion\\_off - Disable motion detection\n/status - Show current status", "Markdown");

  Serial.println("\n[READY] System is ready. Monitoring for motion...\n");
  
  // Allow PIR sensor to stabilize (some PIR sensors need warmup)
  Serial.println("[INFO] Waiting 5 seconds for PIR sensor to stabilize...");
  delay(5000);
  Serial.println("[INFO] PIR sensor ready.\n");
}

// ========================================================================
//                              MAIN LOOP
// ========================================================================
void loop() {
  // --- Check for motion ---
  if (motionDetectionEnabled) {
    
   //int pirState = digitalRead(PIR_SENSOR_PIN); // LEER ESTADO DEL SENSOR PIR.
    
    int pirState = HIGH; // PONER EN ALTO PARA PROBAR ENVIO DE IMAGEN..
    
    if (pirState == HIGH) {
      unsigned long currentTime = millis();
      
      // Cooldown check to avoid spamming
      if (currentTime - lastMotionTime > MOTION_COOLDOWN_MS) {
        lastMotionTime = currentTime;
        
        Serial.println("🔴 [MOTION DETECTED] Capturing image...");
        
        // Notify Telegram about motion
        bot.sendMessage(chatId, "🚨 *Motion Detected!*\nCapturing image...", "Markdown");
        
        // Flash LED on (if enabled)
        if (flashEnabled) {
          flashLED(true);
          delay(100);  // Brief delay to let the flash illuminate
        }
        
        // Capture image
        camera_fb_t* fb = captureImage();
        
        // Flash LED off
        if (flashEnabled) {
          flashLED(false);
        }
        
        if (fb) {
          Serial.printf("[INFO] Image captured: %d bytes (%dx%d)\n", fb->len, fb->width, fb->height);
          
          // Send image to Telegram
          if (sendPhotoToTelegram(fb)) {
            Serial.println("[OK] Image sent to Telegram successfully.");
          } else {
            Serial.println("[ERROR] Failed to send image to Telegram.");
            bot.sendMessage(chatId, "⚠️ Failed to send image.", "");
          }
          
          // Release frame buffer
          esp_camera_fb_return(fb);
        } else {
          Serial.println("[ERROR] Failed to capture image.");
          bot.sendMessage(chatId, "⚠️ Failed to capture image.", "");
        }
      }
    }
  }

  // --- Check for incoming Telegram messages ---
  if (millis() - lastBotCheck > BOT_CHECK_INTERVAL) {
    lastBotCheck = millis();
    
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  }
}

// ========================================================================
//                       CAMERA INITIALIZATION
// ========================================================================
bool initCamera() {
  camera_config_t config;
  
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode    = CAMERA_GRAB_LATEST;
  
  // Use higher resolution if PSRAM is available
  if (psramFound()) {
    config.frame_size   = FRAMESIZE_UXGA;   // 1600x1200
    config.jpeg_quality = 10;               // Lower number = higher quality (0-63)
    config.fb_count     = 2;
    config.fb_location  = CAMERA_FB_IN_PSRAM;
    Serial.println("[INFO] PSRAM found. Using high resolution (UXGA).");
  } else {
    config.frame_size   = FRAMESIZE_SVGA;   // 800x600
    config.jpeg_quality = 12;
    config.fb_count     = 1;
    config.fb_location  = CAMERA_FB_IN_DRAM;
    Serial.println("[INFO] No PSRAM. Using lower resolution (SVGA).");
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[ERROR] Camera init failed with error 0x%x\n", err);
    return false;
  }

  // Adjust camera sensor settings for better image quality
  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor) {
    sensor->set_brightness(sensor, 1);    // Slightly brighter (-2 to 2)
    sensor->set_contrast(sensor, 1);      // Slightly more contrast (-2 to 2)
    sensor->set_saturation(sensor, 0);    // Default saturation (-2 to 2)
    sensor->set_whitebal(sensor, 1);      // Enable white balance
    sensor->set_awb_gain(sensor, 1);      // Enable AWB gain
    sensor->set_wb_mode(sensor, 0);       // Auto white balance mode
    sensor->set_aec2(sensor, 1);          // Enable AEC DSP
    sensor->set_gain_ctrl(sensor, 1);     // Enable AGC
    sensor->set_exposure_ctrl(sensor, 1); // Enable AEC
  }

  return true;
}

// ========================================================================
//                       WIFI CONNECTION
// ========================================================================
bool connectWiFi() {
  Serial.printf("[INFO] Connecting to WiFi: %s", ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
    if (millis() - startTime > WIFI_TIMEOUT_MS) {
      Serial.println("\n[ERROR] WiFi connection timed out!");
      return false;
    }
  }
  
  Serial.println();
  Serial.printf("[OK] WiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("[INFO] Signal strength (RSSI): %d dBm\n", WiFi.RSSI());
  
  return true;
}

// ========================================================================
//                       CAPTURE IMAGE
// ========================================================================
camera_fb_t* captureImage() {
  // Discard first frame (it may be old/stale from the buffer)
  camera_fb_t* fb = esp_camera_fb_get();
  if (fb) {
    esp_camera_fb_return(fb);
  }
  
  // Capture a fresh frame
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[ERROR] Camera capture failed!");
    return nullptr;
  }
  
  return fb;
}

// ========================================================================
//                   SEND PHOTO TO TELEGRAM
// ========================================================================
bool sendPhotoToTelegram(camera_fb_t* fb) {
  if (!fb || fb->len == 0) {
    Serial.println("[ERROR] Invalid frame buffer.");
    return false;
  }

  Serial.printf("[INFO] Sending photo (%d bytes) to Telegram...\n", fb->len);

  const char* host = "api.telegram.org";
  const int   port = 443;

  // Build the multipart/form-data request
  String boundary = "----ESP32CAMBoundary";
  
  // Construct the URL path
  String url = "/bot" + String(botToken) + "/sendPhoto";

  // Build body parts
  String bodyStart = "";
  bodyStart += "--" + boundary + "\r\n";
  bodyStart += "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n";
  bodyStart += String(chatId) + "\r\n";
  bodyStart += "--" + boundary + "\r\n";
  bodyStart += "Content-Disposition: form-data; name=\"caption\"\r\n\r\n";
  bodyStart += "📸 Motion Detected!\r\n";
  bodyStart += "--" + boundary + "\r\n";
  bodyStart += "Content-Disposition: form-data; name=\"photo\"; filename=\"capture.jpg\"\r\n";
  bodyStart += "Content-Type: image/jpeg\r\n\r\n";
  
  String bodyEnd = "\r\n--" + boundary + "--\r\n";

  int contentLength = bodyStart.length() + fb->len + bodyEnd.length();

  // Connect to Telegram API
  WiFiClientSecure client;
  client.setInsecure();  // Skip SSL certificate verification
  
  if (!client.connect(host, port)) {
    Serial.println("[ERROR] Connection to Telegram API failed!");
    return false;
  }

  // Send HTTP POST request
  client.println("POST " + url + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println("Content-Length: " + String(contentLength));
  client.println("Connection: close");
  client.println();
  
  // Send body start (text fields + file header)
  client.print(bodyStart);

  // Send image data in chunks
  const int chunkSize = 1024;
  size_t remaining = fb->len;
  uint8_t* bufPtr = fb->buf;
  
  while (remaining > 0) {
    size_t toSend = (remaining > chunkSize) ? chunkSize : remaining;
    size_t sent = client.write(bufPtr, toSend);
    
    if (sent == 0) {
      Serial.println("[ERROR] Failed to send image data chunk!");
      client.stop();
      return false;
    }
    
    bufPtr    += sent;
    remaining -= sent;
    
    // Yield to prevent watchdog timeout
    delay(1);
  }

  // Send body end
  client.print(bodyEnd);

  // Wait for and read response
  Serial.println("[INFO] Waiting for Telegram response...");
  
  unsigned long responseTimeout = millis();
  while (!client.available()) {
    if (millis() - responseTimeout > 15000) {
      Serial.println("[ERROR] Telegram response timeout!");
      client.stop();
      return false;
    }
    delay(100);
  }

  // Read response status
  String responseLine = client.readStringUntil('\n');
  Serial.printf("[INFO] Response: %s\n", responseLine.c_str());
  
  bool success = responseLine.indexOf("200") != -1;
  
  // Read and discard rest of response
  while (client.available()) {
    client.read();
  }
  
  client.stop();
  return success;
}

// ========================================================================
//                   HANDLE TELEGRAM COMMANDS
// ========================================================================
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String senderId = bot.messages[i].chat_id;
    String text     = bot.messages[i].text;
    String fromName = bot.messages[i].from_name;
    
    Serial.printf("[TELEGRAM] Message from %s: %s\n", fromName.c_str(), text.c_str());

    // Security check: Only respond to authorized chat
    if (senderId != String(chatId)) {
      bot.sendMessage(senderId, "⛔ Unauthorized access.", "");
      Serial.printf("[WARN] Unauthorized message from chat ID: %s\n", senderId.c_str());
      continue;
    }

    // --- /photo command: Capture and send image on demand ---
    if (text == "/photo") {
      bot.sendMessage(chatId, "📷 Capturing image...", "");
      
      if (flashEnabled) {
        flashLED(true);
        delay(100);
      }
      
      camera_fb_t* fb = captureImage();
      
      if (flashEnabled) {
        flashLED(false);
      }
      
      if (fb) {
        if (sendPhotoToTelegram(fb)) {
          Serial.println("[OK] Manual photo sent successfully.");
        } else {
          bot.sendMessage(chatId, "⚠️ Failed to send photo.", "");
        }
        esp_camera_fb_return(fb);
      } else {
        bot.sendMessage(chatId, "⚠️ Failed to capture photo.", "");
      }
    }
    
    // --- /flash_on command ---
    else if (text == "/flash_on") {
      flashEnabled = true;
      bot.sendMessage(chatId, "💡 Flash LED *enabled*.", "Markdown");
    }
    
    // --- /flash_off command ---
    else if (text == "/flash_off") {
      flashEnabled = false;
      bot.sendMessage(chatId, "🔅 Flash LED *disabled*.", "Markdown");
    }
    
    // --- /motion_on command ---
    else if (text == "/motion_on") {
      motionDetectionEnabled = true;
      bot.sendMessage(chatId, "✅ Motion detection *enabled*.", "Markdown");
    }
    
    // --- /motion_off command ---
    else if (text == "/motion_off") {
      motionDetectionEnabled = false;
      bot.sendMessage(chatId, "⏸ Motion detection *disabled*.", "Markdown");
    }
    
    // --- /status command ---
    else if (text == "/status") {
      String statusMsg = "📊 *System Status*\n\n";
      statusMsg += "🔌 WiFi: Connected\n";
      statusMsg += "📶 RSSI: " + String(WiFi.RSSI()) + " dBm\n";
      statusMsg += "🌐 IP: " + WiFi.localIP().toString() + "\n";
      statusMsg += "💡 Flash: " + String(flashEnabled ? "Enabled" : "Disabled") + "\n";
      statusMsg += "🔍 Motion Detection: " + String(motionDetectionEnabled ? "Enabled" : "Disabled") + "\n";
      statusMsg += "⏱ Uptime: " + String(millis() / 1000) + " seconds\n";
      statusMsg += "📦 Free Heap: " + String(ESP.getFreeHeap()) + " bytes\n";
      
      if (psramFound()) {
        statusMsg += "🧠 Free PSRAM: " + String(ESP.getFreePsram()) + " bytes\n";
      }
      
      bot.sendMessage(chatId, statusMsg, "Markdown");
    }
    
    // --- /start or /help command ---
    else if (text == "/start" || text == "/help") {
      String helpMsg = "🤖 *ESP32-CAM Bot Commands*\n\n";
      helpMsg += "/photo - Capture & send photo now\n";
      helpMsg += "/flash\\_on - Enable flash LED\n";
      helpMsg += "/flash\\_off - Disable flash LED\n";
      helpMsg += "/motion\\_on - Enable motion detection\n";
      helpMsg += "/motion\\_off - Disable motion detection\n";
      helpMsg += "/status - Show system status\n";
      helpMsg += "/help - Show this help message\n";
      
      bot.sendMessage(chatId, helpMsg, "Markdown");
    }
    
    // --- Unknown command ---
    else {
      bot.sendMessage(chatId, "❓ Unknown command. Type /help for available commands.", "");
    }
  }
}

// ========================================================================
//                       FLASH LED CONTROL
// ========================================================================
void flashLED(bool state) {
  digitalWrite(FLASH_LED_PIN, state ? HIGH : LOW);
}
