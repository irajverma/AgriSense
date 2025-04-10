#include <ESP8266WiFi.h>
#include <espnow.h>

// ----- Data Structure Matching the Transmitter -----
typedef struct struct_message {
  float temperature;
  float humidity;
  int soilMoisture;
} struct_message;

struct_message sensorData;

// Callback function when data is received via ESP‑NOW
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&sensorData, incomingData, sizeof(sensorData));

  Serial.print("Received data from: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Convert data to JSON format
  Serial.print("{\"temperature\":");
  Serial.print(sensorData.temperature);
  Serial.print(", \"humidity\":");
  Serial.print(sensorData.humidity);
  Serial.print(", \"soilMoisture\":");
  Serial.print(sensorData.soilMoisture);
  Serial.println("}");
}

void setup() {
  Serial.begin(115200);
  
  // Set ESP8266 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP‑NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP‑NOW");
    return;
  }

  // Register for incoming ESP‑NOW messages
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("ESP8266 ESP-NOW Receiver Setup Complete.");
}

void loop() {
  // The receiver only waits and triggers callback when data is received.
  delay(5000);
}
