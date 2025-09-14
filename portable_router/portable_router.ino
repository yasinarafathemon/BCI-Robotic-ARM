#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "NISHAT";  // WiFi Network Name
const char* password = "12345678";    // WiFi Password

ESP8266WebServer server(80);  // Web server on port 80

void handleRoot() {
    server.send(200, "text/html", "<h1>ESP8266 Local Router</h1><p>Connected Devices Can Communicate!</p>");
}

void handleDevice() {
    String msg = "Your IP: " + server.client().remoteIP().toString();
    server.send(200, "text/plain", msg);
}

void setup() {
    Serial.begin(115200);
    
    // Create ESP8266 as a Router (Access Point)
    WiFi.softAP(ssid, password);

    Serial.println("ESP8266 Router Started!");
    Serial.print("Connect to WiFi: ");
    Serial.println(ssid);
    Serial.print("Router IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Routes for Web Server
    server.on("/", handleRoot);
    server.on("/device", handleDevice);

    // Start Web Server
    server.begin();
    Serial.println("Web server started.");
}

void loop() {
    server.handleClient();  // Handle incoming requests
}
