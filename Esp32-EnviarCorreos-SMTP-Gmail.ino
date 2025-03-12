#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#define WIFI_SSID "XXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXX"
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "XXXXXXXXXX@gmail.com"
#define AUTHOR_PASSWORD "XXXXXXXXXX"
#define RECIPIENT_EMAIL "XXXXXXXXXX@hotmail.com" //destinatario
SMTPSession smtp;
    
  void smtpCallback(SMTP_Status status);
    
  void setup() {
    Serial.begin(115200);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
    }

    Serial.println("Wifi connected!");
    
    smtp.debug(1);
    smtp.callback(smtpCallback);
    Session_Config config;
    config.server.host_name = SMTP_HOST;
    config.server.port = SMTP_PORT;
    config.login.email = AUTHOR_EMAIL;
    config.login.password = AUTHOR_PASSWORD;
    config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
    config.time.gmt_offset = 3;
    config.time.day_light_offset = 0;
    
    SMTP_Message message;
    message.sender.name = F("ESP32");
    message.sender.email = AUTHOR_EMAIL;
    message.subject = F("ESP32 Test Email Prueba 2");
    message.addRecipient(F("Recipient"), RECIPIENT_EMAIL);
    message.text.content = "Hello World! - Sent from ESP32";
    message.text.charSet = "us-ascii";
    
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    if (!smtp.connect(&config)) {
    Serial.printf("Connection error: %d, %s\n", smtp.statusCode(), smtp.errorReason().c_str());
    return;
    }
    if (!MailClient.sendMail(&smtp, &message))
    Serial.printf("Error: %d, %s\n", smtp.statusCode(), smtp.errorReason().c_str());
    }
 void loop() {}
    
    void smtpCallback(SMTP_Status status) {
        Serial.println(status.info());
    if (status.success()) {
        Serial.println("Email sent successfully.");
        smtp.sendingResult.clear();
    } else {
        Serial.println("Failed to send email.");
    }
}
