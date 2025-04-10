// This Code will take sensor data from the EPS32 and send it to the ESP8266 over ESP_NOW protocol
#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHTPIN 4         // DHT11 data pin connected to GPIO 4
#define DHTTYPE DHT11
#define SOIL_MOISTURE_PIN 34  // Soil moisture sensor connected to GPIO 34 (analog input)

DHT dht(DHTPIN, DHTTYPE);

// Structure to hold sensor data
typedef struct SensorData {
    float temperature;
    float humidity;
    int soilMoisture;
} SensorData;

SensorData sensorData;

// MAC Address of the ESP32 receiver
uint8_t receiverMac[] = {0x80, 0x64, 0x6F, 0xAD, 0x31, 0xE7}; // Refer other code in repo to check for MAC Adress - 80:64:6F:AD:31:E7

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Successfuly sent to PROCESS MODULE" : "Fail");
}

void setup() {
    Serial.begin(115200);
    dht.begin();

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Initialization Failed!");
        return;
    }

    esp_now_register_send_cb(OnDataSent);
    
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    sensorData.temperature = dht.readTemperature();
    sensorData.humidity = dht.readHumidity();
    sensorData.soilMoisture = analogRead(SOIL_MOISTURE_PIN);

    Serial.print("Temperature: "); Serial.print(sensorData.temperature); Serial.print(" °C");
    Serial.print(" Humidity: "); Serial.print(sensorData.humidity); Serial.print(" %");
    Serial.print(" Soil Moisture: "); Serial.println(sensorData.soilMoisture);

    esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&sensorData, sizeof(sensorData));
    
    if (result == ESP_OK) {
        Serial.println("Data sent successfully");
    } else {
        Serial.println("Failed to send data");
    }

    delay(2000); // Send data every 2 seconds
}
