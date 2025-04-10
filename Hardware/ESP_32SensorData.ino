#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

// ----- Sensor and Pin Settings -----
#define DHTPIN 4              // GPIO pin connected to DHT11 data pin
#define DHTTYPE DHT11         // Define sensor type: DHT11
#define SOIL_MOISTURE_PIN 34  // GPIO34 (ADC1) for the soil moisture sensor

DHT dht(DHTPIN, DHTTYPE);

// ----- Data Structure for ESP‑NOW Communication -----
typedef struct struct_message {
  float temperature;
  float humidity;
  int soilMoisture;
} struct_message;

struct_message sensorData;

// ----- Updated with the Receiver's MAC Address (ESP8266) -----
uint8_t receiverMAC[] = {0x80, 0x64, 0x6F, 0xAD, 0x31, 0xE7};

// Callback function for ESP-NOW data send status
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP‑NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP‑NOW");
    return;
  }

  // Register callback for sending data
  esp_now_register_send_cb(OnDataSent);

  // Add the receiver peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Initialize the DHT sensor
  dht.begin();
  
  Serial.println("ESP32 ESP-NOW Transmitter Setup Complete.");
}

void loop() {
  // Read sensor values
  sensorData.temperature = dht.readTemperature();
  sensorData.humidity    = dht.readHumidity();
  sensorData.soilMoisture = analogRead(SOIL_MOISTURE_PIN);

  
 
    Serial.print("Sending data -> Temp: ");
    Serial.print(sensorData.temperature);
    Serial.print(" C, Humidity: ");
    Serial.print(sensorData.humidity);
    Serial.print(" %, Soil Moisture: ");
    Serial.println(sensorData.soilMoisture);

    // Send data via ESP‑NOW
    esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&sensorData, sizeof(sensorData));
    if (result == ESP_OK) {
      Serial.println("ESP-NOW message sent successfully");
    } else {
      Serial.println("Error sending ESP-NOW message");
    }
  

  
  delay(10000);
}
