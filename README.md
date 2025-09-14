# 🧠 Brain-Controlled Robotic Arm Using Muse EEG & ESP8266

## 🚀 Project Overview
This project enables hands-free robotic arm control using **brain signals**. It integrates:
- **Muse EEG Headband** (for detecting eye blinks)
- **ESP8266 (NodeMCU)** (for robotic arm control via Wi-Fi)
- **Python** (for EEG signal processing & command transmission)

## 🎯 Features
✅ **EEG-based Robotic Arm Control**  
✅ **Wi-Fi Communication (ESP8266 Web Server)**  
✅ **Blink Detection using Muse EEG**  
✅ **Real-time Signal Processing with Python**  

---

## 🛠️ Hardware Components
| Component | Description |
|------------|------------------------------------------------|
| **Muse EEG Headband** | Brainwave sensing device |
| **ESP8266 (NodeMCU)** | Wi-Fi module controlling robotic arm |
| **Servo Motors (x2)** | Moves robotic arm based on EEG input |
| **External Power (5V, 2A)** | Powers servos (if high-torque) |
| **Breadboard & Jumper Wires** | For easy prototyping |

---

## 📡 System Architecture
1. **Muse EEG Headband** captures brain activity (eye blinks).
2. **Mind Monitor App** streams EEG data via OSC (Open Sound Control).
3. **Python Script (`live.py`)** processes EEG signals & detects blinks.
4. **ESP8266 Web Server** receives movement commands.
5. **Robotic Arm (Servo Motors)** moves accordingly.

---

## 🎛️ Circuit Connections (ESP8266 + Servo Motors)
| **ESP8266 Pin** | **Servo Motor 1** (Base Rotation) | **Servo Motor 2** (Arm Up/Down) |
|---------------|----------------|----------------|
| **Vin (5V)** | **Red Wire (+5V)** | **Red Wire (+5V)** |
| **GND** | **Black Wire (GND)** | **Black Wire (GND)** |
| **D1 (GPIO5)** | **Yellow Wire (Signal)** | - |
| **D2 (GPIO4)** | - | **Yellow Wire (Signal)** |

⚠️ **Power Considerations:** Use **external 5V 2A power** for high-torque servos.

### 📷 Wiring Diagram
![IMG-20250207-WA0010](https://github.com/user-attachments/assets/8a9ddf20-2bb4-4000-9773-fd30d03b7a18)


---

## 📲 Setting Up Muse EEG with Mind Monitor
1. **Download Mind Monitor** ([Android](https://play.google.com/store/apps/details?id=com.electroencephalography.mindmonitor) / [iOS](https://apps.apple.com/us/app/mind-monitor/id1190998549)).
2. **Pair Muse EEG Headband** via Bluetooth.
3. **Configure OSC Streaming:**
   - OSC Stream Target IP: `192.168.4.2`
   - OSC Stream Port: `5000`
   - Enable **Raw EEG Data Streaming**
4. **Start EEG Streaming** from the Mind Monitor app.

---

## 📡 IP Address Configuration
| **Device** | **IP Address** | **Purpose** |
|------------|--------------|----------------------|
| **ESP8266 (Robotic Arm Server)** | `192.168.4.3` | Receives movement commands |
| **Laptop/PC (Running Python script)** | `192.168.4.2` | Hosts OSC server for EEG processing |
| **Muse EEG (via Mind Monitor)** | `192.168.4.2:5000` | Streams EEG data via OSC |

---

## 📡 ESP8266 Wi-Fi Access Point Code (`portable_router.ino`)
```cpp
#include <ESP8266WiFi.h>
const char *ssid = "EEG_Control";
const char *password = "12345678";
WiFiServer server(80);
void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    Serial.println("WiFi AP Started");
    server.begin();
}
void loop() {
    WiFiClient client = server.available();
    if (client) {
        String request = client.readStringUntil('\r');
        Serial.println(request);
        client.flush();
    }
}
```

---

## 🤖 ESP8266 Robotic Arm Control Code (`Final_code_arm_with_muse_eeg_control.ino`)
```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
ESP8266WebServer server(80);
Servo servo1, servo2;
void setup() {
    WiFi.begin("EEG_Control", "12345678");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    server.on("/left", []() { servo1.write(45); server.send(200, "text/plain", "Moved Left"); });
    server.on("/right", []() { servo1.write(135); server.send(200, "text/plain", "Moved Right"); });
    server.on("/up", []() { servo2.write(45); server.send(200, "text/plain", "Moved Up"); });
    server.on("/down", []() { servo2.write(135); server.send(200, "text/plain", "Moved Down"); });
    server.on("/reset", []() { servo1.write(90); servo2.write(90); server.send(200, "text/plain", "Reset Position"); });
    server.begin();
}
void loop() { server.handleClient(); }
```

---

## 🎉 Impact
This project successfully enables **brain-controlled robotic arm movement** using EEG signals. It demonstrates a real-time **brain-computer interface** leveraging **Python, OSC, and ESP8266 Wi-Fi communication**.

🚀 **Next Steps:** Add **gesture-based controls** for more intuitive movement!

# STEP BY STEP SETUP

# Step 01: Make a portable wifi
## Must be a single ESP8266 microcontroller board
![router](https://github.com/user-attachments/assets/1bd71552-8082-443f-8c27-80cd60b284e4)

# Step 02: Build the arm properly

## ESP8266 to PCA9685 Servo Driver (I2C Communication)

| **ESP8266 Pin** | **PCA9685 Pin** | **Purpose** |
|---------------|---------------|------------|
| GPIO 4 (SDA) | SDA | I2C Data Line |
| GPIO 5 (SCL) | SCL | I2C Clock Line |
| 3.3V | VCC | Power Supply (Logic) |
| GND | GND | Common Ground |

## PCA9685 to Servo Motors

| **PCA9685 Channel** | **Connected Component** | **Signal Type** |
|----------------|----------------------|----------------|
| Channel 0 | Clutch Servo | PWM Signal |
| Channel 1 | Servo 2 | PWM Signal |
| Channel 2 | Servo 3 | PWM Signal |
| V+ (5V) | Servos Power | 5V Power Supply |
| GND | Servos GND | Common Ground |

## ESP8266 to I2C LCD Display

| **ESP8266 Pin** | **LCD Pin** | **Purpose** |
|---------------|------------|------------|
| GPIO 4 (SDA) | SDA | I2C Data Line |
| GPIO 5 (SCL) | SCL | I2C Clock Line |
| 5V | VCC | Power Supply |
| GND | GND | Common Ground |

## Power Management

| **Component** | **Power Source** | **Voltage** |
|-------------|--------------|---------|
| ESP8266 | Micro USB or 3.3V Input | 3.3V |
| PCA9685 | External Power Supply | 5V |
| Servos | External Power Supply | 5V |
| LCD | ESP8266 5V Pin | 5V |

### 🔹 **Notes**
- **Common Ground:** Ensure all components share a common ground.
- **Servo Power:** Use an external 5V power source for servos.
- **LCD I2C Address:**  `0x27`,  module.


## Use a variable adapter
## power supply should be stable
## Use 3 DOF to 6 DOF
![IMG-20250220-WA0007](https://github.com/user-attachments/assets/d6ace258-7eb0-4280-ac19-78bfcb893d60)
![IMG-20250220-WA0008](https://github.com/user-attachments/assets/0ad52c16-194b-4c5b-8f81-b6312c2311cb)


# Step 03: Block firewall
## From cmd 
## To stop the firewall in CMD as an **administrator** on Windows, use the command: "netsh advfirewall set allprofiles state off". 
## reenable firewall "netsh advfirewall set allprofiles state on".

# Step 04: Connect with wifi
## check the default gateway in cmd with "**ipconfig**" [there **IPv4** and **Default gateway** **IP** addresses will be shown , note that]
![WhatsApp Image 2025-02-20 at 01 00 46_7fe3bae3](https://github.com/user-attachments/assets/86dd9e57-a641-4b9c-8c6b-8f24bed3e7cf)


# Step 05: Assign the IP addresses
## Paste the default gateway IP address in Arduino IDE, where already the robotic arm controlling code is written, pick an available IP address and write there
![image](https://github.com/user-attachments/assets/861f92da-8622-4cbb-aa65-2451c4b564e8)

![image](https://github.com/user-attachments/assets/dad049c0-050e-42dd-845f-cb013d777eba)



# Step 06: Upload
## Upload the code in the arm microcontroller (esp8266) with the data cable
## Check the IP address in the LCD display
## Connect to the server with IP
![image](https://github.com/user-attachments/assets/8c929ed8-ed98-4577-96ff-d663edc55e77)
![image](https://github.com/user-attachments/assets/ee082f17-52a5-4078-97e1-421c9dc9c913)


# Step 07: Connect
## Connect the whole system(arm,PC,muse,mobile etc etc) internet on to the portable router wifi
![image](https://github.com/user-attachments/assets/3d730bc7-88c6-48c2-a742-ee2b904a7a22)
![WhatsApp Image 2025-02-20 at 03 25 32_b76242dd](https://github.com/user-attachments/assets/cf9bdd78-a93a-4134-ac40-36c255319642)


# Step 08: BCI 
# Turn on mobile Bluetooth
# Turn on the muse device
# Turn on the Muse monitor app
# Connect the BCI muce device with the app
![IMG-20250220-WA0002](https://github.com/user-attachments/assets/6c25bee9-988f-44e7-9e9e-8bea9bc31dbf)

# Go to settings
![IMG-20250220-WA0003](https://github.com/user-attachments/assets/7a1b809a-52f0-46fe-936c-10c6ce0b609b)

# Click on **OSC Stream Target IP**
# write down the **IP4 address** which we already got on **step 04**
![IMG-20250220-WA0001](https://github.com/user-attachments/assets/a42af120-d68e-4747-8c8a-e82d21083eb0)

# Back
# Click on "OSC stream" button, it will stream data to the target ip in real time
![image](https://github.com/user-attachments/assets/40086e4e-4c4e-4f4b-aef6-9efe80e89bf4)

![image](https://github.com/user-attachments/assets/260a2ffb-eedd-417e-8157-595179fad61d)

# Seat in relax position

# Step 08:  Live control
## open the live.py folder 
![image](https://github.com/user-attachments/assets/3793720d-50f5-4811-975e-75ea550e3f95)

## write "cmd" in the search bar
![image](https://github.com/user-attachments/assets/b855c42f-5111-4e84-ad99-540ce9b77f76)

## Now the system is ready to run with blink

![image](https://github.com/user-attachments/assets/dd9a63f5-7801-4960-8d47-8106d84bec32)

![image](https://github.com/user-attachments/assets/43e417a3-28bb-4ed2-b488-74fc4f8d1adb)

# Setup Video
[Watch the video](https://youtu.be/ZB38iElaanM)

# Controlling Video
[Watch the video](https://youtu.be/DXIi5MIBfzM)



