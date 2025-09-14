#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "index.h"

// PCA9685 Servo Driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Initialize I2C LCD (Check your I2C address: 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD Address, Columns, Rows

// WiFi Network Credentials (Connect to an Existing Router)
const char* ssid = "oppo";          // WiFi Network Name
const char* password = "shakib3301"; // WiFi Password

// Static IP Address Configuration (Make sure your router allows this IP)
IPAddress local_IP(192, 168, 43, 72); 
IPAddress gateway(192, 168, 43, 1);   // Check this using "ipconfig" on Windows
IPAddress subnet(255, 255, 255, 0);   // Standard subnet mask

// Web Server
AsyncWebServer server(80);

// Servo Config
#define SERVO_MIN  102
#define SERVO_MAX  512
#define SERVO_CLUTCH  0
#define SERVO_2  1
#define SERVO_3  2

// Function to Move Servo
void moveServo(int channel, int angle) {
  int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(channel, 0, pulse);
}

// Function to Move Clutch Servo (Limited to 0-90°)
void moveClutchServo(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 90) angle = 90;
  moveServo(SERVO_CLUTCH, angle);
}

// Function to Update LCD Display
void updateLCD(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  updateLCD("Connecting to", ssid);

  // Connect to Existing WiFi Network
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  
  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  updateLCD("WiFi Connected", WiFi.localIP().toString());

  // Initialize PCA9685
  pwm.begin();
  pwm.setPWMFreq(50); // Set frequency to 50Hz for servos

  // Set default positions
  moveClutchServo(0);
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
    updateLCD("Moving Left", "S2:180 S3:0");
    request->send(200, "text/plain", "Moved Left");
  });

  server.on("/right", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(90);
    moveServo(SERVO_2, 0);
    moveServo(SERVO_3, 0);
    updateLCD("Moving Right", "S2:0 S3:0");
    request->send(200, "text/plain", "Moved Right");
  });

  server.on("/up", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(90);
    moveServo(SERVO_2, 90);
    moveServo(SERVO_3, 90);
    updateLCD("Moving Up", "S2:90 S3:90");
    request->send(200, "text/plain", "Moved Up");
  });

  server.on("/down", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(90);
    moveServo(SERVO_2, 180);
    moveServo(SERVO_3, 90);
    updateLCD("Moving Down", "S2:180 S3:90");
    request->send(200, "text/plain", "Moved Down");
  });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    moveClutchServo(0);
    moveServo(SERVO_2, 90);
    moveServo(SERVO_3, 90);
    updateLCD("Arm Reset", "Ready...");
    request->send(200, "text/plain", "Arm Reset");
  });

  // Start the server
  server.begin();
}

void loop() {
  // No need for loop processing since ESPAsyncWebServer handles requests asynchronously.
}
