#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "index.h"

// PCA9685 Servo Driver Setup
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// WiFi credentials
const char* ssid = "NISHAT-2.4G";
const char* password = "nishat2020";

// Web Server on port 80
AsyncWebServer server(80);

// MG996R Servo configuration (Corrected for PCA9685)
#define SERVO_MIN  102   // 0 degrees (500µs)
#define SERVO_MAX  512   // 180 degrees (2500µs)

// Servo channel mapping
#define SERVO_CLUTCH  0  // Clutch Servo (0° to 90° only)
#define SERVO_2  1       // Normal Servo (0° to 180°)
#define SERVO_3  2       // Normal Servo (0° to 180°)

// Move a servo to a specific angle
void moveServo(int channel, int angle) {
  int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(channel, 0, pulse);
}

// Move Clutch Servo (S1) - Limited to 0-90°
void moveClutchServo(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 90) angle = 90;
  moveServo(SERVO_CLUTCH, angle);
}

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Initialize PCA9685
  pwm.begin();
  pwm.setPWMFreq(50); // Set frequency to 50Hz for servos

  // Set default positions (Initial Arm Position)
  moveClutchServo(0); // Clutch starts at 0° (closed)
  moveServo(SERVO_2, 90);
  moveServo(SERVO_3, 90);

  // Serve Web Page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", MAIN_page);
  });

  // Handle Servo Control via Web Buttons
  server.on("/left", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(90);
    moveServo(SERVO_2, 180);
    moveServo(SERVO_3, 0);
    request->send(200, "text/plain", "Moved Left");
  });

  server.on("/right", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(90);
    moveServo(SERVO_2, 0);
    moveServo(SERVO_3, 0);
    request->send(200, "text/plain", "Moved Right");
  });

  server.on("/up", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(90);
    moveServo(SERVO_2, 90);
    moveServo(SERVO_3, 90);
    request->send(200, "text/plain", "Moved Up");
  });

  server.on("/down", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(90);
    moveServo(SERVO_2, 180);
    moveServo(SERVO_3, 90);
    request->send(200, "text/plain", "Moved Down");
  });

  // Reset all servos to default positions
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(0);
    moveServo(SERVO_2, 90);
    moveServo(SERVO_3, 90);
    request->send(200, "text/plain", "Arm Reset");
  });

  // Start the server
  server.begin();
}

void loop() {
  // No need for loop processing since ESPAsyncWebServer handles requests asynchronously.
}
