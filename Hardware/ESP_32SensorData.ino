#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHTPIN 4              
#define DHTTYPE DHT11         
#define SOIL_MOISTURE_PIN 34  

DHT dht(DHTPIN, DHTTYPE);

// ----- Data Structure -----
typedef struct struct_message {
  float temperature;
  float humidity;
  int soilMoisture;
} struct_message;

struct_message sensorData;

// ----- Receiver's MAC Address -----
uint8_t receiverMAC[] = {0x80, 0x64, 0x6F, 0xAD, 0x31, 0xE7};

// Callback function for ESP-NOW data send status
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  
  // ✅ 1. Set ESP32 to Wi-Fi Station mode (VERY IMPORTANT)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Ensure it's not connected to any network

  // ✅ 2. Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // ✅ 3. Register callback
  esp_now_register_send_cb(OnDataSent);

  // ✅ 4. Prepare Peer Info
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));  // Clear previous data
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;  

  // ✅ 5. Add Peer to ESP-NOW
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  dht.begin();
  Serial.println("ESP32 ESP-NOW Transmitter Setup Complete.");
}

void loop() {
  sensorData.temperature = dht.readTemperature();
  sensorData.humidity = dht.readHumidity();
  sensorData.soilMoisture = analogRead(SOIL_MOISTURE_PIN);

  if (!isnan(sensorData.temperature) && !isnan(sensorData.humidity)) {
    Serial.print("Sending -> Temp: ");
    Serial.print(sensorData.temperature);
    Serial.print(" C, Humidity: ");
    Serial.print(sensorData.humidity);
    Serial.print(" %, Soil Moisture: ");
    Serial.println(sensorData.soilMoisture);

    // ✅ 6. Send ESP-NOW Data
    esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&sensorData, sizeof(sensorData));
    Serial.println(result == ESP_OK ? "ESP-NOW message sent successfully" : "Error sending ESP-NOW message");
  }

  delay(5000);
}
