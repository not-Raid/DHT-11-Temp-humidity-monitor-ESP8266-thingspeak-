#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT11.h>

// Primary and Secondary WiFi Credentials
const char* ssid_primary = "bruuh";        // Replace with your primary WiFi SSID
const char* password_primary = "95750163"; // Replace with your primary WiFi Password
const char* ssid_secondary = "Samima Khatun"; // Replace with your secondary WiFi SSID (with space)
const char* password_secondary = "19900000"; // Replace with your secondary WiFi Password

// ThingSpeak API
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "D9TPKQT6T5RI52B2"; // Replace with your ThingSpeak API key

// DHT11 sensor setup
#define DHT_PIN 2 // GPIO2 (D4 on NodeMCU)
DHT11 dht11(DHT_PIN);

void connectToWiFi() {
    Serial.print("Connecting to primary WiFi: ");
    Serial.println(ssid_primary);
    WiFi.begin(ssid_primary, password_primary);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 15) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nPrimary WiFi failed! Connecting to secondary WiFi...");
        WiFi.begin(ssid_secondary, password_secondary);
        
        attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 15) {
            delay(1000);
            Serial.print(".");
            attempts++;
        }
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
    } else {
        Serial.println("\nFailed to connect to any WiFi");
    }
}

void setup() {
    Serial.begin(115200);
    connectToWiFi();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected! Reconnecting...");
        connectToWiFi();
    }
    
    int temperature = 0;
    int humidity = 0;

    // Read temperature and humidity from DHT11
    int result = dht11.readTemperatureHumidity(temperature, humidity);
    
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");

        // Send data to ThingSpeak
        if (WiFi.status() == WL_CONNECTED) {
            WiFiClient client;
            HTTPClient http;
            String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(temperature) + "&field2=" + String(humidity);
            
            http.begin(client, url);
            int httpResponseCode = http.GET();
            
            if (httpResponseCode > 0) {
                Serial.print("ThingSpeak Response: ");
                Serial.println(httpResponseCode);
            } else {
                Serial.print("Error sending data: ");
                Serial.println(httpResponseCode);
            }

            http.end();
        } else {
            Serial.println("WiFi not connected!");
        }
    } else {
        Serial.println(DHT11::getErrorString(result));
    }

    delay(900000); // ThingSpeak allows updates every 15 minutes (900,000 ms)
}
